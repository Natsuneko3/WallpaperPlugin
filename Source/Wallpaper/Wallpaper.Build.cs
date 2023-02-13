// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WallPaper : ModuleRules
{
	public WallPaper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
				
			}
		);


		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"Blutility",
				"UMGEditor", 
				"MediaAssets",
				

				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Projects",
				"Engine",
				"SlateCore",
				"EditorStyle",
				"UnrealEd",
				"InputCore",
				"EditorFramework",
				"Slate",
				"ToolMenus",
				


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