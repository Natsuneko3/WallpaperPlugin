#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "GlassMaterial.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "ImageUtils.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "ShaderCompiler.h"
#include "Slate/WidgetRenderer.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Images/SImage.h"

namespace GlassRenderingTests
{
	constexpr int32 ImageSize = 64;
	constexpr int32 ColorTolerance = 3;

	TStrongObjectPtr<UTexture2D> MakeTexture(TFunctionRef<FColor(int32, int32)> PixelAt)
	{
		TArray<FColor> Pixels;
		Pixels.SetNumUninitialized(ImageSize * ImageSize);
		for (int32 Y = 0; Y < ImageSize; ++Y)
		{
			for (int32 X = 0; X < ImageSize; ++X)
			{
				Pixels[Y * ImageSize + X] = PixelAt(X, Y);
			}
		}
		const TConstArrayView64<uint8> Bytes(reinterpret_cast<const uint8*>(Pixels.GetData()), Pixels.Num() * sizeof(FColor));
		TStrongObjectPtr<UTexture2D> Texture(UTexture2D::CreateTransient(ImageSize, ImageSize, PF_B8G8R8A8, NAME_None, Bytes));
		if (Texture.IsValid())
		{
			Texture->SRGB = false;
			Texture->Filter = TF_Bilinear;
			Texture->AddressX = Texture->AddressY = TA_Clamp;
			Texture->UpdateResource();
		}
		return Texture;
	}

	struct FRenderFixture
	{
		FWidgetRenderer* Renderer = new FWidgetRenderer(false, true);
		TStrongObjectPtr<UMaterial> Material;
		TStrongObjectPtr<UMaterialInstanceDynamic> Instance;

		~FRenderFixture()
		{
			BeginCleanup(Renderer);
			FlushRenderingCommands();
		}

		bool Initialize(FAutomationTestBase& Test, UTexture* Texture)
		{
			Material.Reset(CreateGlassMaterial(Texture));
			if (!Material.IsValid())
			{
				return false;
			}
			Material->CacheShaders(EMaterialShaderPrecompileMode::Synchronous);
			Material->EnsureIsComplete();
			GShaderCompilingManager->FinishAllCompilation();
			const FMaterialResource* Resource = Material->GetMaterialResource(GMaxRHIFeatureLevel);
			if (!Resource || !Resource->GetGameThreadShaderMap() || !Resource->IsGameThreadShaderMapComplete())
			{
				Test.AddError(TEXT("Glass shader compilation failed; refusing to render a fallback material."));
				if (Resource)
				{
					for (const FString& Error : Resource->GetCompileErrors())
					{
						Test.AddError(Error);
					}
				}
				return false;
			}
			Instance.Reset(UMaterialInstanceDynamic::Create(Material.Get(), GetTransientPackage()));
			Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture);
			Instance->SetVectorParameterValue(TEXT("Mapping"), FLinearColor(0, 0, 0, 0));
			Instance->SetVectorParameterValue(TEXT("Axes"), FLinearColor(1, 0, 0, 1));
			Instance->SetVectorParameterValue(TEXT("Size"), FLinearColor(ImageSize, ImageSize, 0, 0));
			Instance->SetVectorParameterValue(TEXT("Shape"), FLinearColor(0, 1, 0, 0));
			Instance->SetVectorParameterValue(TEXT("Mouse"), FLinearColor(0, 0, 0, 0));
			Instance->SetVectorParameterValue(TEXT("BlurStep"), FLinearColor(0, 0, 0, 0));
			Instance->SetVectorParameterValue(TEXT("Gaussian"), FLinearColor(0, 0.5f, 1, 0));
			Instance->SetVectorParameterValue(TEXT("Spectrum"), FLinearColor(0, 0, 1.2f, 0));
			Instance->SetVectorParameterValue(TEXT("DispersionShape"), FLinearColor(24, 0, 0, 0));
			Instance->SetVectorParameterValue(TEXT("ScatterStep"), FLinearColor(8.0f / ImageSize, 0, 0, 8.0f / ImageSize));
			Instance->SetVectorParameterValue(TEXT("Absorption"), FLinearColor::White);
			Instance->SetVectorParameterValue(TEXT("Finish"), FLinearColor(0, 1, 0, 0));
			SetScattering(0);
			return true;
		}

		void SetScattering(float Strength)
		{
			Instance->SetVectorParameterValue(TEXT("Optics"), FLinearColor(Strength, 1, 0, 0));
		}

		void SetGaussian(float Strength, float Radius, float Sigma = 0.5f)
		{
			Instance->SetVectorParameterValue(TEXT("Gaussian"), FLinearColor(Strength, Sigma, 1, 0));
			Instance->SetVectorParameterValue(TEXT("BlurStep"), FLinearColor(Radius / ImageSize, 0, 0, Radius / ImageSize));
		}

		void SetSpectrum(float Radius, float Strength)
		{
			Instance->SetVectorParameterValue(TEXT("Spectrum"), FLinearColor(Radius, Strength, 1.2f, 0));
		}

		bool Render(UObject* Resource, TArray<FColor>& Pixels)
		{
			FSlateBrush Brush;
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.SetImageSize(FVector2D(ImageSize, ImageSize));
			Brush.SetResourceObject(Resource);
			Brush.TintColor = FLinearColor::White;
			const auto Image = SNew(SImage).Image(&Brush);
			TStrongObjectPtr<UTextureRenderTarget2D> Target(Renderer->DrawWidget(Image, FVector2D(ImageSize, ImageSize)));
			if (!Target.IsValid())
			{
				return false;
			}
			FlushRenderingCommands();
			FReadSurfaceDataFlags Flags(RCM_UNorm);
			Flags.SetLinearToGamma(false);
			return Target->GameThread_GetRenderTargetResource()->ReadPixels(Pixels, Flags)
				&& Pixels.Num() == ImageSize * ImageSize;
		}
	};

	int32 ColorError(const FColor& Left, const FColor& Right)
	{
		return FMath::Max3(FMath::Abs(int32(Left.R) - Right.R),
			FMath::Abs(int32(Left.G) - Right.G), FMath::Abs(int32(Left.B) - Right.B));
	}

	int32 InteriorError(const TArray<FColor>& Left, const TArray<FColor>& Right)
	{
		int32 MaxError = 0;
		// Exclude silhouette antialiasing: these assertions measure transmitted color.
		for (int32 Y = 8; Y < ImageSize - 8; ++Y)
		{
			for (int32 X = 8; X < ImageSize - 8; ++X)
			{
				MaxError = FMath::Max(MaxError, ColorError(Left[Y * ImageSize + X], Right[Y * ImageSize + X]));
			}
		}
		return MaxError;
	}

	bool CheckConstant(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32, int32)
		{
			return FColor(64, 96, 160, 255);
		});
		if (!Texture.IsValid() || !Fixture.Initialize(Test, Texture.Get()))
		{
			return false;
		}
		TArray<FColor> Reference, ClearGlass, ScatteredGlass;
		if (!Fixture.Render(Texture.Get(), Reference) || !Fixture.Render(Fixture.Instance.Get(), ClearGlass))
		{
			return false;
		}
		Test.TestTrue(TEXT("Clear glass preserves source color without a blue tint or brightness clamp"),
			InteriorError(Reference, ClearGlass) <= ColorTolerance);
		Fixture.SetScattering(1);
		if (!Fixture.Render(Fixture.Instance.Get(), ScatteredGlass))
		{
			return false;
		}
		Test.TestTrue(TEXT("Scattering preserves the energy and hue of uniform illumination"),
			InteriorError(ClearGlass, ScatteredGlass) <= ColorTolerance);
		return true;
	}

	bool CheckTonalRange(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32 X, int32)
		{
			const uint8 Value = uint8(X * 255 / (ImageSize - 1));
			return FColor(Value, Value, Value, 255);
		});
		if (!Texture.IsValid())
		{
			return false;
		}
		Fixture.Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture.Get());
		Fixture.SetScattering(0);
		TArray<FColor> Reference, Glass;
		if (!Fixture.Render(Texture.Get(), Reference) || !Fixture.Render(Fixture.Instance.Get(), Glass))
		{
			return false;
		}
		Test.TestTrue(TEXT("Dark and bright tones match the wallpaper instead of flattening into a plateau"),
			InteriorError(Reference, Glass) <= ColorTolerance);
		const int32 Row = ImageSize / 2 * ImageSize;
		Test.TestTrue(TEXT("Bright tonal detail is retained"), Glass[Row + 52].R > Glass[Row + 40].R + 20);
		return true;
	}

	bool CheckScattering(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32 X, int32)
		{
			return X < ImageSize / 2 ? FColor::Black : FColor::White;
		});
		if (!Texture.IsValid())
		{
			return false;
		}
		Fixture.Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture.Get());
		Fixture.SetScattering(0);
		TArray<FColor> ClearGlass, ScatteredGlass;
		if (!Fixture.Render(Fixture.Instance.Get(), ClearGlass))
		{
			return false;
		}
		Fixture.SetScattering(1);
		if (!Fixture.Render(Fixture.Instance.Get(), ScatteredGlass))
		{
			return false;
		}
		const int32 Row = ImageSize / 2 * ImageSize;
		const int32 Dark = Row + ImageSize / 2 - 2;
		const int32 Bright = Row + ImageSize / 2 + 1;
		const int32 ClearContrast = int32(ClearGlass[Bright].R) - ClearGlass[Dark].R;
		const int32 DiffuseContrast = int32(ScatteredGlass[Bright].R) - ScatteredGlass[Dark].R;
		Test.TestTrue(TEXT("Test fixture contains a resolved black-white edge"), ClearContrast > 180);
		Test.TestTrue(TEXT("Scattering softens a high-contrast edge"), DiffuseContrast < ClearContrast - 30);
		Test.TestTrue(TEXT("Scattering transfers light into the dark side"), ScatteredGlass[Dark].R > ClearGlass[Dark].R + 10);
		Test.TestTrue(TEXT("Scattering removes the same edge's sharp bright peak"), ScatteredGlass[Bright].R + 10 < ClearGlass[Bright].R);
		return true;
	}

	int32 RegionError(const TArray<FColor>& Left, const TArray<FColor>& Right, const FIntRect& Region)
	{
		int32 Maximum = 0;
		for (int32 Y = Region.Min.Y; Y < Region.Max.Y; ++Y)
		{
			for (int32 X = Region.Min.X; X < Region.Max.X; ++X)
			{
				Maximum = FMath::Max(Maximum, ColorError(Left[Y * ImageSize + X], Right[Y * ImageSize + X]));
			}
		}
		return Maximum;
	}

	void SaveComparison(FAutomationTestBase& Test, const TCHAR* Name,
		const TArray<FColor>& First, const TArray<FColor>& Second, const TArray<FColor>& Third)
	{
		constexpr int32 PreviewWidth = ImageSize * 3;
		TArray<FColor> Pixels;
		Pixels.SetNumUninitialized(PreviewWidth * ImageSize);
		for (int32 Y = 0; Y < ImageSize; ++Y)
		{
			FMemory::Memcpy(Pixels.GetData() + Y * PreviewWidth, First.GetData() + Y * ImageSize, ImageSize * sizeof(FColor));
			FMemory::Memcpy(Pixels.GetData() + Y * PreviewWidth + ImageSize, Second.GetData() + Y * ImageSize, ImageSize * sizeof(FColor));
			FMemory::Memcpy(Pixels.GetData() + Y * PreviewWidth + ImageSize * 2, Third.GetData() + Y * ImageSize, ImageSize * sizeof(FColor));
		}
		for (FColor& Pixel : Pixels)
		{
			Pixel.A = 255;
		}
		TArray64<uint8> PNG;
		FImageUtils::PNGCompressImageArray(PreviewWidth, ImageSize, Pixels, PNG);
		const FString Path = FPaths::ProjectSavedDir() / Name;
		if (!FFileHelper::SaveArrayToFile(PNG, *Path))
		{
			Test.AddWarning(FString::Printf(TEXT("Unable to save rendering preview: %s"), *Path));
		}
	}

	bool CheckGaussianConstant(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32, int32)
		{
			return FColor(43, 113, 187, 255);
		});
		if (!Texture.IsValid() || !Fixture.Initialize(Test, Texture.Get()))
		{
			return false;
		}
		Fixture.SetGaussian(1, 8);
		TArray<FColor> Reference, Filtered;
		if (!Fixture.Render(Texture.Get(), Reference) || !Fixture.Render(Fixture.Instance.Get(), Filtered))
		{
			return false;
		}
		Test.TestTrue(TEXT("Gaussian filtering preserves a uniform colored source"),
			InteriorError(Reference, Filtered) <= ColorTolerance);
		return true;
	}

	float RowVariation(const TArray<FColor>& Pixels)
	{
		float Variation = 0;
		const int32 Row = ImageSize / 2 * ImageSize;
		for (int32 X = 12; X < ImageSize - 12; ++X)
		{
			Variation += FMath::Abs(int32(Pixels[Row + X].R) - Pixels[Row + X - 1].R);
		}
		return Variation;
	}

	bool CheckGaussianHighFrequency(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32 X, int32)
		{
			return X % 8 < 4 ? FColor::Black : FColor::White;
		});
		if (!Texture.IsValid())
		{
			return false;
		}
		Fixture.Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture.Get());
		Fixture.SetGaussian(0, 8);
		TArray<FColor> Reference, StrengthOff, RadiusOff, Blurred;
		if (!Fixture.Render(Texture.Get(), Reference) || !Fixture.Render(Fixture.Instance.Get(), StrengthOff))
		{
			return false;
		}
		Fixture.SetGaussian(1, 0);
		if (!Fixture.Render(Fixture.Instance.Get(), RadiusOff))
		{
			return false;
		}
		Fixture.SetGaussian(1, 8);
		if (!Fixture.Render(Fixture.Instance.Get(), Blurred))
		{
			return false;
		}
		Test.TestTrue(TEXT("Zero Gaussian strength preserves sharp source pixels with nonzero radius"),
			InteriorError(Reference, StrengthOff) <= ColorTolerance);
		Test.TestTrue(TEXT("Zero Gaussian radius preserves sharp source pixels with full strength"),
			InteriorError(Reference, RadiusOff) <= ColorTolerance);
		const float SharpVariation = RowVariation(StrengthOff);
		Test.TestTrue(TEXT("The frequency fixture has resolved alternating bars"), SharpVariation > 1000);
		Test.TestTrue(TEXT("Gaussian filtering reduces high-frequency contrast"), RowVariation(Blurred) < SharpVariation * 0.65f);
		SaveComparison(Test, TEXT("GlassGaussianFrequency.png"), Reference, StrengthOff, Blurred);
		return true;
	}

	bool CheckGaussianStep(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32 X, int32)
		{
			return X < ImageSize / 2 ? FColor::Black : FColor::White;
		});
		if (!Texture.IsValid())
		{
			return false;
		}
		Fixture.Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture.Get());
		Fixture.SetGaussian(0, 8);
		TArray<FColor> Sharp, Narrow, Broad;
		if (!Fixture.Render(Fixture.Instance.Get(), Sharp))
		{
			return false;
		}
		Fixture.SetGaussian(1, 8, 0.2f);
		if (!Fixture.Render(Fixture.Instance.Get(), Narrow))
		{
			return false;
		}
		Fixture.SetGaussian(1, 8, 1);
		if (!Fixture.Render(Fixture.Instance.Get(), Broad))
		{
			return false;
		}
		const int32 Row = ImageSize / 2 * ImageSize;
		const int32 NearDark = Row + ImageSize / 2 - 2;
		const int32 NearBright = Row + ImageSize / 2 + 1;
		const int32 FarDark = Row + ImageSize / 2 - 6;
		Test.TestTrue(TEXT("Gaussian filtering spreads light across a black-white step"), Broad[NearDark].R > Sharp[NearDark].R + 20);
		Test.TestTrue(TEXT("Gaussian filtering softens the bright side of the step"), Broad[NearBright].R + 20 < Sharp[NearBright].R);
		Test.TestTrue(TEXT("A larger sigma broadens the effective diffusion at fixed radius"), Broad[FarDark].R > Narrow[FarDark].R + 15);
		Test.TestTrue(TEXT("Gaussian filtering leaves distant constant regions unchanged"),
			ColorError(Broad[Row + 12], Sharp[Row + 12]) <= ColorTolerance
			&& ColorError(Broad[Row + ImageSize - 13], Sharp[Row + ImageSize - 13]) <= ColorTolerance);
		SaveComparison(Test, TEXT("GlassGaussianStep.png"), Sharp, Narrow, Broad);
		return true;
	}

	bool CheckSpectralConstant(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32, int32)
		{
			return FColor(43, 113, 187, 255);
		});
		if (!Texture.IsValid() || !Fixture.Initialize(Test, Texture.Get()))
		{
			return false;
		}
		Fixture.SetGaussian(1, 8);
		Fixture.SetScattering(1);
		Fixture.SetSpectrum(12, 1);
		TArray<FColor> Reference, Filtered;
		if (!Fixture.Render(Texture.Get(), Reference) || !Fixture.Render(Fixture.Instance.Get(), Filtered))
		{
			return false;
		}
		Test.TestTrue(TEXT("Spectrum with Gaussian and scattering preserves uniform color at the edge"),
			RegionError(Reference, Filtered, FIntRect(2, 2, ImageSize - 2, ImageSize - 2)) <= ColorTolerance);
		return true;
	}

	struct FFringeMetrics
	{
		int32 ColoredPixels = 0;
		int32 MaximumRedOverBlue = 0;
		int32 MaximumBlueOverRed = 0;
		TSet<uint32> DistinctColors;
	};

	FFringeMetrics MeasureFringes(const TArray<FColor>& Pixels)
	{
		FFringeMetrics Metrics;
		const int32 Row = ImageSize / 2 * ImageSize;
		for (int32 X = 2; X < ImageSize - 2; ++X)
		{
			const FColor Pixel = Pixels[Row + X];
			const int32 Chroma = FMath::Max3(int32(Pixel.R), int32(Pixel.G), int32(Pixel.B))
				- FMath::Min3(int32(Pixel.R), int32(Pixel.G), int32(Pixel.B));
			Metrics.MaximumRedOverBlue = FMath::Max(Metrics.MaximumRedOverBlue, int32(Pixel.R) - Pixel.B);
			Metrics.MaximumBlueOverRed = FMath::Max(Metrics.MaximumBlueOverRed, int32(Pixel.B) - Pixel.R);
			if (Chroma > 12)
			{
				++Metrics.ColoredPixels;
				Metrics.DistinctColors.Add((uint32(Pixel.R / 32) << 6) | (uint32(Pixel.G / 32) << 3) | uint32(Pixel.B / 32));
			}
		}
		return Metrics;
	}

	bool CheckSpectralFringes(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		auto Texture = MakeTexture([](int32 X, int32)
		{
			return X % 12 < 6 ? FColor::Black : FColor::White;
		});
		if (!Texture.IsValid())
		{
			return false;
		}
		Fixture.Instance->SetTextureParameterValue(TEXT("Wallpaper"), Texture.Get());
		Fixture.SetGaussian(0, 0);
		Fixture.SetScattering(0);
		Fixture.SetSpectrum(12, 0);
		TArray<FColor> Clear, Spectral, Scattered, ScatteredSpectral;
		if (!Fixture.Render(Fixture.Instance.Get(), Clear))
		{
			return false;
		}
		Fixture.SetSpectrum(12, 1);
		if (!Fixture.Render(Fixture.Instance.Get(), Spectral))
		{
			return false;
		}
		const FFringeMetrics ClearMetrics = MeasureFringes(Clear);
		const FFringeMetrics SpectralMetrics = MeasureFringes(Spectral);
		Test.TestEqual(TEXT("The spectral fixture starts with a neutral grayscale image"), ClearMetrics.ColoredPixels, 0);
		Test.TestTrue(TEXT("A grayscale edge develops several distinct spectral colors"),
			SpectralMetrics.ColoredPixels >= 6 && SpectralMetrics.DistinctColors.Num() >= 4);
		Test.TestTrue(TEXT("Spectral fringes contain both red-leading and blue-leading regions"),
			SpectralMetrics.MaximumRedOverBlue > 12 && SpectralMetrics.MaximumBlueOverRed > 12);
		const FIntRect Center(26, 26, 38, 38);
		Test.TestTrue(TEXT("Edge dispersion leaves the center unchanged"), RegionError(Clear, Spectral, Center) <= ColorTolerance);

		Fixture.SetScattering(1);
		Fixture.SetSpectrum(12, 0);
		if (!Fixture.Render(Fixture.Instance.Get(), Scattered))
		{
			return false;
		}
		Fixture.SetSpectrum(12, 1);
		if (!Fixture.Render(Fixture.Instance.Get(), ScatteredSpectral))
		{
			return false;
		}
		Test.TestTrue(TEXT("Full scattering retains visible spectral fringes"), MeasureFringes(ScatteredSpectral).ColoredPixels >= 6);
		Test.TestTrue(TEXT("Full scattering still responds to spectral strength"), InteriorError(Scattered, ScatteredSpectral) > 12);
		Test.TestTrue(TEXT("Scattered center remains unchanged when spectrum is enabled"),
			RegionError(Scattered, ScatteredSpectral, Center) <= ColorTolerance);
		SaveComparison(Test, TEXT("GlassSpectrum.png"), Clear, Spectral, ScatteredSpectral);
		return true;
	}

	bool CheckSpectralSwitches(FAutomationTestBase& Test, FRenderFixture& Fixture)
	{
		// Keep other filters active: radius zero must not replace them with the sharp source.
		Fixture.SetGaussian(1, 6);
		Fixture.SetScattering(0.5f);
		Fixture.SetSpectrum(0, 0);
		TArray<FColor> Disabled, StrengthOff, RadiusOff;
		if (!Fixture.Render(Fixture.Instance.Get(), Disabled))
		{
			return false;
		}
		Fixture.SetSpectrum(12, 0);
		if (!Fixture.Render(Fixture.Instance.Get(), StrengthOff))
		{
			return false;
		}
		Fixture.SetSpectrum(0, 1);
		if (!Fixture.Render(Fixture.Instance.Get(), RadiusOff))
		{
			return false;
		}
		const FIntRect Interior(2, 2, ImageSize - 2, ImageSize - 2);
		Test.TestTrue(TEXT("Zero spectral strength completely disables dispersion with nonzero radius"),
			RegionError(Disabled, StrengthOff, Interior) <= ColorTolerance);
		Test.TestTrue(TEXT("Zero spectral radius completely disables dispersion while preserving other filters"),
			RegionError(Disabled, RadiusOff, Interior) <= ColorTolerance);
		return true;
	}

	bool CanRender(FAutomationTestBase& Test)
	{
		if (GUsingNullRHI || !GShaderCompilingManager || !FSlateApplication::IsInitialized())
		{
			Test.AddError(TEXT("Glass GPU verification requires Slate, a rendering RHI, and the shader compiler."));
			return false;
		}
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassColorAndScatteringTest, "Wallpaper.Glass.ColorAndScattering",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassColorAndScatteringTest::RunTest(const FString&)
{
	if (GUsingNullRHI || !GShaderCompilingManager || !FSlateApplication::IsInitialized())
	{
		AddError(TEXT("Color and scattering verification requires Slate, a rendering RHI, and the shader compiler."));
		return false;
	}
	GlassRenderingTests::FRenderFixture Fixture;
	if (!GlassRenderingTests::CheckConstant(*this, Fixture)
		|| !GlassRenderingTests::CheckTonalRange(*this, Fixture)
		|| !GlassRenderingTests::CheckScattering(*this, Fixture))
	{
		AddError(TEXT("Unable to create or read back a glass rendering fixture."));
		return false;
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassGaussianTest, "Wallpaper.Glass.Gaussian",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassGaussianTest::RunTest(const FString&)
{
	if (!GlassRenderingTests::CanRender(*this))
	{
		return false;
	}
	GlassRenderingTests::FRenderFixture Fixture;
	if (!GlassRenderingTests::CheckGaussianConstant(*this, Fixture)
		|| !GlassRenderingTests::CheckGaussianHighFrequency(*this, Fixture)
		|| !GlassRenderingTests::CheckGaussianStep(*this, Fixture))
	{
		AddError(TEXT("Unable to create or read back a Gaussian rendering fixture."));
		return false;
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassSpectralDispersionTest, "Wallpaper.Glass.SpectralDispersion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassSpectralDispersionTest::RunTest(const FString&)
{
	if (!GlassRenderingTests::CanRender(*this))
	{
		return false;
	}
	GlassRenderingTests::FRenderFixture Fixture;
	if (!GlassRenderingTests::CheckSpectralConstant(*this, Fixture)
		|| !GlassRenderingTests::CheckSpectralFringes(*this, Fixture)
		|| !GlassRenderingTests::CheckSpectralSwitches(*this, Fixture))
	{
		AddError(TEXT("Unable to create or read back a spectral rendering fixture."));
		return false;
	}
	return true;
}
#endif
