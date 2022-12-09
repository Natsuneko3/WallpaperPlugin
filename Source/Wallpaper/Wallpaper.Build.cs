// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Wallpaper : ModuleRules
{
	public Wallpaper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				
				"UMGEditor", 
				"MediaAssets"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				
				"CoreUObject",
				"Projects",
				"Engine",
				"SlateCore",
				"EditorStyle",
				"UnrealEd",
				"InputCore",
				"Blutility",
				"Slate",
				"ToolMenus",
				"WindowsTargetPlatform",
				
				
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
