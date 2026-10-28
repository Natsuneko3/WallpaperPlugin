#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "GlassWindowBackdrop.h"
#include "Brushes/SlateColorBrush.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "Slate/WidgetRenderer.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"

namespace GlassBackdropTests
{
	constexpr int32 ImageSize = 64;
	constexpr int32 ColorTolerance = 3;
	const FIntPoint OldNodePosition(8, 8);
	const FIntPoint OldNodeSize(20, 20);
	const FIntPoint NewNodePosition(40, 32);
	const FIntPoint NewNodeSize(12, 18);

	TSharedRef<SWidget> MakeSolidColor(FLinearColor Color)
	{
		static const FSlateColorBrush Brush(FLinearColor::White);
		return SNew(SImage).Image(&Brush).ColorAndOpacity(Color);
	}

	struct FFrameDescription
	{
		FLinearColor Wallpaper;
		FIntPoint NodePosition;
		FIntPoint NodeSize;
		float InheritedOpacity = 1;
	};

	TSharedRef<SWidget> MakeFrame(const FFrameDescription& Frame, bool bIncludeBackdrop)
	{
		const TSharedRef<SOverlay> Layers = SNew(SOverlay);
		if (bIncludeBackdrop)
		{
			Layers->AddSlot()[MakeGlassWindowBackdrop()];
		}
		Layers->AddSlot()[MakeSolidColor(Frame.Wallpaper)];
		Layers->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(FMargin(Frame.NodePosition.X, Frame.NodePosition.Y, 0, 0))
			[
				SNew(SBox)
				.WidthOverride(Frame.NodeSize.X)
				.HeightOverride(Frame.NodeSize.Y)
				[
					MakeSolidColor(FLinearColor::White)
				]
			];
		Layers->SetRenderOpacity(Frame.InheritedOpacity);
		return Layers;
	}

	struct FRenderFixture
	{
		// Every draw uses ELoad on this same target, matching a regular Slate window.
		FWidgetRenderer* Renderer = new FWidgetRenderer(false, false);
		TStrongObjectPtr<UTextureRenderTarget2D> Target{
			FWidgetRenderer::CreateTargetFor(FVector2D(ImageSize, ImageSize), TF_Nearest, false)};

		~FRenderFixture()
		{
			BeginCleanup(Renderer);
			FlushRenderingCommands();
		}

		bool Draw(const TSharedRef<SWidget>& Widget, TArray<FColor>& Pixels)
		{
			if (!Target.IsValid())
			{
				return false;
			}
			Renderer->DrawWidget(Target.Get(), Widget, FVector2D(ImageSize, ImageSize), 0);
			FlushRenderingCommands();
			FReadSurfaceDataFlags Flags(RCM_UNorm);
			Flags.SetLinearToGamma(false);
			return Target->GameThread_GetRenderTargetResource()->ReadPixels(Pixels, Flags)
				&& Pixels.Num() == ImageSize * ImageSize;
		}

		bool ResetWithOpaquePaint()
		{
			// Prime a known background without changing the renderer's no-clear behavior.
			TArray<FColor> Pixels;
			return Draw(MakeSolidColor(FLinearColor::Black), Pixels);
		}
	};

	int32 ColorError(const FColor& Left, const FColor& Right)
	{
		return FMath::Max3(FMath::Abs(int32(Left.R) - Right.R),
			FMath::Abs(int32(Left.G) - Right.G), FMath::Abs(int32(Left.B) - Right.B));
	}

	int32 InteriorError(const TArray<FColor>& Left, const TArray<FColor>& Right)
	{
		int32 Maximum = 0;
		for (int32 Y = 2; Y < ImageSize - 2; ++Y)
		{
			for (int32 X = 2; X < ImageSize - 2; ++X)
			{
				Maximum = FMath::Max(Maximum, ColorError(Left[Y * ImageSize + X], Right[Y * ImageSize + X]));
			}
		}
		return Maximum;
	}

	struct FFramePair
	{
		TArray<FColor> First;
		TArray<FColor> Second;
	};

	bool DrawPair(FRenderFixture& Fixture, const FFrameDescription& First, const FFrameDescription& Second,
		bool bIncludeBackdrop, FFramePair& Frames)
	{
		return Fixture.ResetWithOpaquePaint()
			&& Fixture.Draw(MakeFrame(First, bIncludeBackdrop), Frames.First)
			&& Fixture.Draw(MakeFrame(Second, bIncludeBackdrop), Frames.Second);
	}

	void SavePreview(FAutomationTestBase& Test, const FFramePair& WithoutBackdrop, const FFramePair& WithBackdrop)
	{
		constexpr int32 PreviewWidth = ImageSize * 2;
		TArray<FColor> Pixels;
		Pixels.SetNumUninitialized(PreviewWidth * ImageSize);
		for (int32 Y = 0; Y < ImageSize; ++Y)
		{
			FMemory::Memcpy(Pixels.GetData() + Y * PreviewWidth,
				WithoutBackdrop.Second.GetData() + Y * ImageSize, ImageSize * sizeof(FColor));
			FMemory::Memcpy(Pixels.GetData() + Y * PreviewWidth + ImageSize,
				WithBackdrop.Second.GetData() + Y * ImageSize, ImageSize * sizeof(FColor));
		}
		for (FColor& Pixel : Pixels)
		{
			Pixel.A = 255;
		}
		TArray64<uint8> PNG;
		FImageUtils::PNGCompressImageArray(PreviewWidth, ImageSize, Pixels, PNG);
		const FString Path = FPaths::ProjectSavedDir() / TEXT("GlassBackdropGhostBeforeAfter.png");
		if (!FFileHelper::SaveArrayToFile(PNG, *Path))
		{
			Test.AddWarning(FString::Printf(TEXT("Unable to save backdrop preview: %s"), *Path));
		}
	}

	bool CheckFrameReplacement(FAutomationTestBase& Test, FRenderFixture& Fixture, float WallpaperAlpha, float InheritedOpacity)
	{
		const FFrameDescription First{FLinearColor(0.08f, 0.16f, 0.30f, WallpaperAlpha),
			OldNodePosition, OldNodeSize, InheritedOpacity};
		const FFrameDescription Second{FLinearColor(0.35f, 0.12f, 0.04f, WallpaperAlpha),
			NewNodePosition, NewNodeSize, InheritedOpacity};
		FFramePair WithoutBackdrop, WithBackdrop;
		TArray<FColor> FreshFrame;
		if (!DrawPair(Fixture, First, Second, false, WithoutBackdrop)
			|| !DrawPair(Fixture, First, Second, true, WithBackdrop)
			|| !Fixture.ResetWithOpaquePaint()
			|| !Fixture.Draw(MakeFrame(Second, false), FreshFrame))
		{
			return false;
		}
		const FString Case = FString::Printf(TEXT("Wallpaper alpha %.2f, inherited opacity %.2f: "), WallpaperAlpha, InheritedOpacity);
		constexpr int32 OldNodeCenter = 18 * ImageSize + 18;
		constexpr int32 BackgroundPoint = 4 * ImageSize + 32;
		constexpr int32 NewNodeCenter = 40 * ImageSize + 46;
		Test.TestTrue(Case + TEXT("first frame contains a bright node at the old location"),
			ColorError(WithoutBackdrop.First[OldNodeCenter], WithoutBackdrop.First[BackgroundPoint]) > 20);
		Test.TestTrue(Case + TEXT("without a backdrop the moved node leaves visible pixels behind"),
			ColorError(WithoutBackdrop.Second[OldNodeCenter], FreshFrame[OldNodeCenter]) > 20);
		Test.TestTrue(Case + TEXT("backdrop replaces the old node with the current frame's background"),
			ColorError(WithBackdrop.Second[OldNodeCenter], FreshFrame[OldNodeCenter]) <= ColorTolerance);
		Test.TestTrue(Case + TEXT("the whole frame matches an independently primed render after moving and resizing the node"),
			InteriorError(WithBackdrop.Second, FreshFrame) <= ColorTolerance);
		Test.TestTrue(Case + TEXT("the moved node remains visible above the backdrop"),
			ColorError(WithBackdrop.Second[NewNodeCenter], WithBackdrop.Second[OldNodeCenter]) > 20);
		if (WallpaperAlpha > 0 && InheritedOpacity == 1)
		{
			SavePreview(Test, WithoutBackdrop, WithBackdrop);
		}
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassBackdropTest, "Wallpaper.Glass.WindowBackdrop",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassBackdropTest::RunTest(const FString&)
{
	if (GUsingNullRHI || !FSlateApplication::IsInitialized())
	{
		AddError(TEXT("Window backdrop verification requires Slate and a rendering RHI."));
		return false;
	}
	const TSharedRef<SWidget> Backdrop = MakeGlassWindowBackdrop();
	TestFalse(TEXT("The window backdrop cannot intercept pointer input"), Backdrop->GetVisibility().IsHitTestVisible());
	GlassBackdropTests::FRenderFixture Fixture;
	if (!GlassBackdropTests::CheckFrameReplacement(*this, Fixture, 0.35f, 1)
		|| !GlassBackdropTests::CheckFrameReplacement(*this, Fixture, 0, 1)
		|| !GlassBackdropTests::CheckFrameReplacement(*this, Fixture, 0.35f, 0.25f))
	{
		AddError(TEXT("Unable to create or read back the persistent window render target."));
		return false;
	}
	return true;
}
#endif
