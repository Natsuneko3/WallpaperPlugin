// Copyright 2023 Natsu Neko, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WallPaper : ModuleRules
{
	public WallPaper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"DirectoryWatcher",
			"InputCore",
			"LevelEditor",
			"MediaAssets",
			"Settings",
			"Slate",
			"ToolMenus",
			"UnrealEd"
		});
	}
}
