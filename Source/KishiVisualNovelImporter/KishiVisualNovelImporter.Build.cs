// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KishiVisualNovelImporter : ModuleRules
{
	public KishiVisualNovelImporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
			PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Json",
				//"Slate",
				//"SlateCore",
				"Engine",
				"UnrealEd",
				"AssetTools",
				"ContentBrowser",
        "EditorStyle",
				"ToolMenus",
				"KishiVisualNovel",
			});
			PrivateIncludePaths.AddRange(
				new string[] {
					//"KishiVisualNovelImporter/psd_sdk/src/Psd",
					//"KishiVisualNovelImporter/psd_sdk/src/Samples",
					// ... add other private include paths required here ...
				}
			);
		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"AssetRegistry"
			});

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"AssetRegistry"
			});
	}
}
