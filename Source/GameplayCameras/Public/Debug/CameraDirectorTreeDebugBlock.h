// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Debug/CameraDebugBlock.h"
#include "Math/Transform.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

struct FCameraDebugBlockBuilder;
struct FCameraEvaluationContextStack;

/**
 * A debug block for showing the list of camera directors in the camera system's context stack.
 */
class FCameraDirectorTreeDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(GAMEPLAYCAMERAS_API, FCameraDirectorTreeDebugBlock)

public:

	FCameraDirectorTreeDebugBlock();

	void Initialize(const FCameraEvaluationContextStack& ContextStack, FCameraDebugBlockBuilder& Builder);

protected:

	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;
	virtual void OnSerialize(FArchive& Ar) override;

private:

	struct FDirectorDebugInfo
	{
		FString CameraAssetName;
		FTransform3d InitialContextTransform = FTransform3d::Identity;
		bool bIsValid = false;
	};
	TArray<FDirectorDebugInfo> CameraDirectors;

	friend FArchive& operator<< (FArchive&, FDirectorDebugInfo&);
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

