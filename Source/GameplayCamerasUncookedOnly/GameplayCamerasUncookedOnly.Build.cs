// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameplayCamerasUncookedOnly : ModuleRules
	{
		public GameplayCamerasUncookedOnly(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"GameplayCameras"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"BlueprintGraph",
					"Core",
					"CoreUObject",
					"Engine",
					"BlueprintGraph",
					"Kismet",
					"KismetCompiler",
					"SlateCore",
					"UnrealEd",
				}
			);
		}
	}
}

