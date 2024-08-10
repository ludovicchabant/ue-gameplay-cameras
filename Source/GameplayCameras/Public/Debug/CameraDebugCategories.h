// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/StringFwd.h"
#include "GameplayCameras.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

/**
 * Standard debug categories for showing different "debug modes".
 */
struct GAMEPLAYCAMERAS_API FCameraDebugCategories
{
	static const FString NodeTree;
	static const FString DirectorTree;
	static const FString BlendStacks;
	static const FString Services;
	static const FString PoseStats;
	static const FString Viewfinder;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

