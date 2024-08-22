// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GameplayCameras : ModuleRules
{
	public GameplayCameras(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"CinematicCamera",
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"EnhancedInput",
				"GameplayTags",
				"HeadMountedDisplay",
				"MovieScene",
				"MovieSceneTracks",
				"StateTreeModule",
				"TemplateSequence",
				"TraceLog"
			}
		);
	}
}
