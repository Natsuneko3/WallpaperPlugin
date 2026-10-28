#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "GlassMaterial.h"
#include "GlassUV.h"
#include "Engine/Texture2D.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "MediaTexture.h"
#include "RHI.h"
#include "ShaderCompiler.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassUVTest, "Wallpaper.Glass.UVMapping",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassUVTest::RunTest(const FString&)
{
    for (float DPI : {1.0f, 1.5f, 2.0f})
    {
        for (float Zoom : {0.25f, 1.0f, 2.0f})
        {
            const FVector2D WindowOffset(-800, 240);
            const FVector2D NodeOffset(300, 150);
            const FGeometry Background = FGeometry::MakeRoot(FVector2D(1200, 800), FSlateLayoutTransform(DPI, WindowOffset));
            const FGeometry Node = FGeometry::MakeRoot(FVector2D(200, 100), FSlateLayoutTransform(DPI * Zoom, WindowOffset + NodeOffset * DPI));
            FGlassUV UV;
            TestTrue(TEXT("Valid geometry"), FGlassUV::Calculate(Node, Background, UV));
            TestTrue(TEXT("Top left is independent of DPI and node zoom"), UV.Map(FVector2D::ZeroVector).Equals(FVector2D(0.25, 0.1875), 1.e-6));
            const FVector2D Expected = (NodeOffset + FVector2D(200,100) * Zoom) / FVector2D(1200,800);
            TestTrue(TEXT("Bottom right includes graph zoom"), UV.Map(FVector2D(1,1)).Equals(Expected, 1.e-6));
        }
    }
    FGlassUV UV;
    TestFalse(TEXT("Zero sized background is rejected"), FGlassUV::Calculate(
        FGeometry::MakeRoot(FVector2D(100,100), FSlateLayoutTransform()),
        FGeometry::MakeRoot(FVector2D::ZeroVector, FSlateLayoutTransform()), UV));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlassMaterialTest, "Wallpaper.Glass.MaterialCompilation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGlassMaterialTest::RunTest(const FString&)
{
    if (!GShaderCompilingManager || GUsingNullRHI)
    {
        AddError(TEXT("Material verification requires a rendering RHI and shader compiler."));
        return false;
    }
    TStrongObjectPtr<UTexture2D> Image(UTexture2D::CreateTransient(16, 16));
    TStrongObjectPtr<UMediaTexture> Video(NewObject<UMediaTexture>());
    Image->UpdateResource();
    Video->UpdateResource();
    for (UTexture* Texture : {static_cast<UTexture*>(Image.Get()), static_cast<UTexture*>(Video.Get())})
    {
        TStrongObjectPtr<UMaterial> Material(CreateGlassMaterial(Texture));
        if (!TestNotNull(TEXT("Glass material created"), Material.Get())) return false;
        Material->CacheShaders(EMaterialShaderPrecompileMode::Synchronous);
        Material->EnsureIsComplete();
        GShaderCompilingManager->FinishAllCompilation();
        const FMaterialResource* Resource = Material->GetMaterialResource(GMaxRHIFeatureLevel);
        if (!TestNotNull(TEXT("Material resource"), Resource)) return false;
        AddInfo(FString::Printf(TEXT("Feature=%d Quality=%d Map=%d Complete=%d Compiling=%d"),
            int32(Resource->GetFeatureLevel()), int32(Resource->GetQualityLevel()),
            Resource->GetGameThreadShaderMap() != nullptr, Resource->IsGameThreadShaderMapComplete(), Material->IsCompiling()));
        for (const FString& Error : Resource->GetCompileErrors()) AddError(Error);
        TestNotNull(TEXT("Shader map exists"), Resource->GetGameThreadShaderMap());
        TestTrue(TEXT("Shader map compiled successfully"), Resource->IsGameThreadShaderMapComplete());
    }
    return true;
}
#endif
