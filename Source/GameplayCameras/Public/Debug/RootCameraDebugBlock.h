// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockStorage.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

class FCameraSystemEvaluator;
struct FCameraDebugBlockBuildParams;
struct FCameraDebugBlockBuilder;

GAMEPLAYCAMERAS_API extern bool GGameplayCamerasDebugEnable;
GAMEPLAYCAMERAS_API extern FString GGameplayCamerasDebugCategories;

/**
 * The root debug block for the camera system.
 */
class FRootCameraDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(GAMEPLAYCAMERAS_API, FRootCameraDebugBlock)

public:

	/** Build all debug blocks for the last evaluation frame. */
	GAMEPLAYCAMERAS_API void BuildDebugBlocks(const FCameraSystemEvaluator& CameraSystem, const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder);

	/** Initiate the debug drawing. */
	GAMEPLAYCAMERAS_API void RootDebugDraw(FCameraDebugRenderer& Renderer);
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

