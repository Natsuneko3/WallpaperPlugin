using UnrealBuildTool;

public class WallpaperGlass : ModuleRules
{
    public WallpaperGlass(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] {
            "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "GraphEditor",
            "UnrealEd", "Settings", "Projects", "MediaAssets", "WallPaper", "RenderCore", "RHI", "UMG"
        });
        if (Target.bBuildDeveloperTools)
        {
            PrivateDependencyModuleNames.AddRange(new[] { "MaterialEditor" });
        }
    }
}
