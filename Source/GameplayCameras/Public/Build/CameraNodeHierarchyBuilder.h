// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "Core/CameraNodeHierarchy.h"

#define UE_API GAMEPLAYCAMERAS_API

class UBaseCameraObject;

namespace UE::Cameras
{

struct FCameraObjectBuildContext;

/**
 * A helper class that can build a hierarchy of camera nodes.
 */
class FCameraNodeHierarchyBuilder
{
public:

	/** Creates a new camera node hierarchy builder. */
	UE_API FCameraNodeHierarchyBuilder(FCameraBuildLog& InBuildLog, UBaseCameraObject* InCameraObject);

	/** Gets the camera node hierarchy. */
	const FCameraNodeHierarchy& GetHierarchy() const { return CameraNodeHierarchy; }

	/** Calls PreBuild on all the camera nodes. */
	UE_API void PreBuild();

	/** Calls Build on all the camera nodes and computes the allocation info. */
	UE_API void Build();

private:

	UE_API void CallBuild(FCameraObjectBuildContext& BuildContext, UCameraNode* CameraNode);
	UE_API void BuildParametersAllocationInfo(FCameraObjectBuildContext& BuildContext);

private:

	FCameraBuildLog& BuildLog;
	UBaseCameraObject* CameraObject = nullptr;
	FCameraNodeHierarchy CameraNodeHierarchy;
};

}  // namespace UE::Cameras

#undef UE_API
