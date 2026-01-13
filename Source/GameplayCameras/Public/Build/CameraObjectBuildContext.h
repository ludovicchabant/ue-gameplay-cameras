// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildContext.h"
#include "Core/BaseCameraObject.h"

namespace UE::Cameras
{

/**
 * Camera object build context.
 */
struct FCameraObjectBuildContext : public FCameraBuildContext
{
	FCameraObjectBuildContext(const FCameraBuildContext& InParentContext)
		: FCameraBuildContext(InParentContext)
	{}

	/** The allocation information for the camera rig. */
	FCameraObjectAllocationInfo AllocationInfo;
};

}  // namespace UE::Cameras

