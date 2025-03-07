// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Build/CameraBuildLog.h"
#include "Core/CameraNodeHierarchy.h"

class UBaseCameraObject;

namespace UE::Cameras
{

struct FCameraObjectBuildContext;

/**
 * A helper class that can build a hierarchy of camera nodes.
 */
class GAMEPLAYCAMERAS_API FCameraNodeHierarchyBuilder
{
public:

	/** Creates a new camera node hierarchy builder. */
	FCameraNodeHierarchyBuilder(FCameraBuildLog& InBuildLog, UBaseCameraObject* InCameraObject);

	/** Gets the camera node hierarchy. */
	const FCameraNodeHierarchy& GetHierarchy() const { return CameraNodeHierarchy; }

	/** Calls PreBuild on all the camera nodes. */
	void PreBuild();

	/** Calls Build on all the camera nodes and computes the allocation info. */
	void Build();

private:

	void CallBuild(FCameraObjectBuildContext& BuildContext, UCameraNode* CameraNode);
	void BuildParametersAllocationInfo(FCameraObjectBuildContext& BuildContext);

private:

	FCameraBuildLog& BuildLog;
	UBaseCameraObject* CameraObject = nullptr;
	FCameraNodeHierarchy CameraNodeHierarchy;
};

}  // namespace UE::Cameras

