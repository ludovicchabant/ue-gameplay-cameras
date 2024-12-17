// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Set.h"
#include "Core/CameraRigAsset.h"

namespace UE::Cameras
{

class FCameraBuildLog;

/**
 * Camera rig asset build context.
 */
struct FCameraRigBuildContext
{
	FCameraRigBuildContext(FCameraBuildLog& InBuildLog)
		: BuildLog(InBuildLog)
	{}

	/** The build log for emitting messages. */
	FCameraBuildLog& BuildLog;

	/** The allocation information for the camera rig. */
	FCameraRigAllocationInfo AllocationInfo;
};

}  // namespace UE::Cameras

