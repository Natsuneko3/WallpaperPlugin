#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "GlassMaterial.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "ImageUtils.h"
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

namespace GlassHighlightTests
{
    constexpr int32 Padding = 16;
    constexpr int32 PixelTolerance = 3;

    struct FRenderedHighlight
    {
        FIntPoint BodySize = FIntPoint::ZeroValue;
        FIntPoint ImageSize = FIntPoint::ZeroValue;
        TArray<FColor> Pixels;

        int32 BrightnessAt(int32 BodyX, int32 BodyY) const
        {
            const FColor Pixel = Pixels[(BodyY + Padding) * ImageSize.X + BodyX + Padding];
            return FMath::Max3(int32(Pixel.R), int32(Pixel.G), int32(Pixel.B));
        }

        int32 MaximumBrightness() const
        {
            int32 Maximum = 0;
            for (const FColor& Pixel : Pixels)
            {
                Maximum = FMath::Max(Maximum, FMath::Max3(int32(Pixel.R), int32(Pixel.G), int32(Pixel.B)));
            }
            return Maximum;
        }

        void SavePreview(const TCHAR* Name) const
        {
            TArray<FColor> OpaquePixels = Pixels;
            // Additive pixels can have zero alpha. The preview intentionally shows RGB on black.
            for (FColor& Pixel : OpaquePixels) Pixel.A = 255;
            TArray64<uint8> PNG;
            FImageUtils::PNGCompressImageArray(ImageSize.X, ImageSize.Y, OpaquePixels, PNG);
            FFileHelper::SaveArrayToFile(PNG, *(FPaths::ProjectSavedDir() / Name));
        }
    };

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

        bool Initialize()
        {
            Material.Reset(CreateGlassHighlightMaterial());
            if (!Material.IsValid()) return false;
            Material->CacheShaders(EMaterialShaderPrecompileMode::Synchronous);
            Material->EnsureIsComplete();
            GShaderCompilingManager->FinishAllCompilation();
            Instance.Reset(UMaterialInstanceDynamic::Create(Material.Get(), GetTransientPackage()));
            Instance->SetVectorParameterValue(TEXT("Rim"), FLinearColor(2, 0.8f, 40, 0.12f));
            Instance->SetVectorParameterValue(TEXT("Glow"), FLinearColor(10, 0.25f, 0, 0));
            Instance->SetVectorParameterValue(TEXT("HighlightColor"), FLinearColor::White);
            return true;
        }

        bool Render(FIntPoint BodySize, FVector2D Mouse, FRenderedHighlight& Output)
        {
            Output.BodySize = BodySize;
            Output.ImageSize = BodySize + FIntPoint(2 * Padding, 2 * Padding);
            Instance->SetVectorParameterValue(TEXT("Size"), FLinearColor(BodySize.X, BodySize.Y, Padding, 12));
            Instance->SetVectorParameterValue(TEXT("Mouse"), FLinearColor(Mouse.X, Mouse.Y, 0, 0));
            FSlateBrush Brush;
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.SetImageSize(FVector2D(Output.ImageSize));
            Brush.SetResourceObject(Instance.Get());
            Brush.TintColor = FLinearColor::White;
            const auto Image = SNew(SImage).Image(&Brush);
            TStrongObjectPtr<UTextureRenderTarget2D> Target(Renderer->DrawWidget(Image, FVector2D(Output.ImageSize)));
            if (!Target.IsValid()) return false;
            FlushRenderingCommands();
            FReadSurfaceDataFlags Flags(RCM_UNorm);
            Flags.SetLinearToGamma(false);
            return Target->GameThread_GetRenderTargetResource()->ReadPixels(Output.Pixels, Flags)
                && Output.Pixels.Num() == Output.ImageSize.X * Output.ImageSize.Y;
        }
    };

    bool CheckCursorResponse(FAutomationTestBase& Test, FRenderFixture& Fixture)
    {
        constexpr int32 Width = 128;
        constexpr int32 Height = 80;
        FRenderedHighlight Left, Right, Distant;
        if (!Fixture.Render(FIntPoint(Width, Height), FVector2D(0, Height / 2), Left)
            || !Fixture.Render(FIntPoint(Width, Height), FVector2D(Width, Height / 2), Right)
            || !Fixture.Render(FIntPoint(Width, Height), FVector2D(10000, 10000), Distant)) return false;

        const int32 LeftNear = Left.BrightnessAt(0, Height / 2);
        const int32 LeftFar = Left.BrightnessAt(Width - 1, Height / 2);
        const int32 RightNear = Right.BrightnessAt(Width - 1, Height / 2);
        const int32 RightFar = Right.BrightnessAt(0, Height / 2);
        Test.TestTrue(TEXT("Left cursor lights the left edge"), LeftNear > LeftFar + 20);
        Test.TestTrue(TEXT("Right cursor lights the right edge"), RightNear > RightFar + 20);
        Test.TestTrue(TEXT("Moving the cursor changes the same edge's brightness"), LeftNear > RightFar + 20);
        const int32 Ambient = Distant.BrightnessAt(0, Height / 2);
        Test.TestTrue(TEXT("Distant cursor loses the dynamic contribution"), Ambient < LeftNear / 2);
        Test.TestTrue(TEXT("Distant cursor retains the ambient outline"), Ambient > PixelTolerance);
        Test.TestTrue(TEXT("Distant cursor has no directional bias"),
            FMath::Abs(Ambient - Distant.BrightnessAt(Width - 1, Height / 2)) <= PixelTolerance);

        Test.TestTrue(TEXT("Soft halo extends outside the node"), Left.BrightnessAt(-4, Height / 2) > PixelTolerance);
        Test.TestTrue(TEXT("Halo ends before the expanded quad boundary"), Left.BrightnessAt(-Padding, Height / 2) <= PixelTolerance);
        Test.TestTrue(TEXT("Highlight does not tint the node center"), Left.BrightnessAt(Width / 2, Height / 2) <= PixelTolerance);
        Left.SavePreview(TEXT("GlassHighlightCursorLeft.png"));
        Right.SavePreview(TEXT("GlassHighlightCursorRight.png"));
        return true;
    }

    bool CheckLocalDistance(FAutomationTestBase& Test, FRenderFixture& Fixture)
    {
        FRenderedHighlight Small, Large;
        const FVector2D Mouse(32, 0);
        if (!Fixture.Render(FIntPoint(128, 80), Mouse, Small)
            || !Fixture.Render(FIntPoint(256, 80), Mouse, Large)) return false;
        const int32 NearSmall = Small.BrightnessAt(32, 0);
        const int32 FarSmall = Small.BrightnessAt(72, 0);
        Test.TestTrue(TEXT("Cursor light attenuates over local Slate distance"), NearSmall > FarSmall + 20);
        Test.TestTrue(TEXT("A larger body preserves the near-edge intensity at the same local distance"),
            FMath::Abs(NearSmall - Large.BrightnessAt(32, 0)) <= PixelTolerance);
        Test.TestTrue(TEXT("A larger body preserves attenuation at the same local distance"),
            FMath::Abs(FarSmall - Large.BrightnessAt(72, 0)) <= PixelTolerance);
        return true;
    }

    bool CheckDisabledLight(FAutomationTestBase& Test, FRenderFixture& Fixture)
    {
        FRenderedHighlight NoAmbient, Disabled;
        Fixture.Instance->SetVectorParameterValue(TEXT("Rim"), FLinearColor(2, 0.8f, 40, 0));
        if (!Fixture.Render(FIntPoint(128, 80), FVector2D(10000, 10000), NoAmbient)) return false;
        Test.TestTrue(TEXT("No ambient and a distant cursor produce no light"), NoAmbient.MaximumBrightness() <= PixelTolerance);
        Fixture.Instance->SetVectorParameterValue(TEXT("Rim"), FLinearColor(2, 0, 40, 0.12f));
        Fixture.Instance->SetVectorParameterValue(TEXT("Glow"), FLinearColor(10, 0.25f, 0, 0));
        if (!Fixture.Render(FIntPoint(128, 80), FVector2D(0, 40), Disabled)) return false;
        Test.TestTrue(TEXT("Zero overall strength disables the halo even with nonzero glow strength"), Disabled.MaximumBrightness() <= PixelTolerance);
        return true;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassCursorHighlightTest, "Wallpaper.Glass.CursorHighlight",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassCursorHighlightTest::RunTest(const FString&)
{
    if (GUsingNullRHI || !GShaderCompilingManager || !FSlateApplication::IsInitialized())
    {
        AddError(TEXT("Cursor highlight verification requires Slate, a rendering RHI, and the shader compiler."));
        return false;
    }
    GlassHighlightTests::FRenderFixture Fixture;
    if (!Fixture.Initialize() || !GlassHighlightTests::CheckCursorResponse(*this, Fixture)
        || !GlassHighlightTests::CheckLocalDistance(*this, Fixture)
        || !GlassHighlightTests::CheckDisabledLight(*this, Fixture))
    {
        AddError(TEXT("Unable to create or read back a cursor-highlight rendering fixture."));
        return false;
    }
    return true;
}
#endif
