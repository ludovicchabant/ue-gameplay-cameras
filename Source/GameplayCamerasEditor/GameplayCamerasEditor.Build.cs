// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameplayCamerasEditor : ModuleRules
{
	public GameplayCamerasEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateIncludePathModuleNames.AddRange(
			new string[] 
			{
				"AssetTools",
				"Kismet",
				"EditorWidgets",
				"MessageLog",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetDefinition",
				"AssetRegistry",
				"BlueprintGraph",
				"Core",
				"CoreUObject",
				"EditorFramework",
				"EditorSubsystem",
				"Engine",
				"GameplayCameras",
				"InputCore",
				"InteractiveToolsFramework",
				"Kismet",
				"Projects",
				"Slate",
				"SlateCore",
				"TimeManagement",
				"ToolMenus",
				"UnrealEd",
			}
		);

		var DynamicModuleNames = new string[] {
			"LevelEditor",
			"PropertyEditor",
			"WorkspaceMenuStructure",
		};

		foreach (var Name in DynamicModuleNames)
		{
			PrivateIncludePathModuleNames.Add(Name);
			DynamicallyLoadedModuleNames.Add(Name);
		}
	}
}

