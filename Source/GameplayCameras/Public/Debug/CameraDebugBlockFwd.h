// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameras.h"

namespace UE::Cameras
{

#if UE_GAMEPLAY_CAMERAS_DEBUG

struct FCameraDebugBlockBuilder;

/**
 * Structure for creating the node evaluator's debug blocks.
 */
struct FCameraDebugBlockBuildParams
{
	// Empty for now, but defined for later API changes.
};

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

