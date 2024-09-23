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
				"ApplicationCore",
				"AssetDefinition",
				"AssetRegistry",
				"BlueprintGraph",
				"CinematicCamera",
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"EditorFramework",
				"EditorSubsystem",
				"Engine",
				"GameplayCameras",
				"GraphEditor",
				"InputCore",
				"InteractiveToolsFramework",
				"Kismet",
				"LevelEditor",
				"Projects",
				"RewindDebuggerInterface",
				"Slate",
				"SlateCore",
				"TimeManagement",
				"ToolMenus",
				"TraceAnalysis",
				"TraceInsights",
				"TraceLog",
				"TraceServices",
				"UnrealEd",
			}
		);

		var DynamicModuleNames = new string[] {
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

