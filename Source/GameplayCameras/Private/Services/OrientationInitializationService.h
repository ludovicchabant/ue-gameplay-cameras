// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationService.h"
#include "Core/CameraIKAim.h"
#include "Core/CameraRigTransition.h"

namespace UE::Cameras
{

class FCameraEvaluationContext;
struct FCameraRigEvaluationInfo;

class FOrientationInitializationService : public FCameraEvaluationService
{
	UE_DECLARE_CAMERA_EVALUATION_SERVICE(GAMEPLAYCAMERAS_API, FOrientationInitializationService)

protected:

	// FCameraEvaluationService interface.
	virtual void OnInitialize(const FCameraEvaluationServiceInitializeParams& Params) override;
	virtual void OnPostUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult) override;
	virtual void OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	void TryInitializeContextYawPitch(const FCameraRigEvaluationInfo& CameraRigInfo);
	void TryPreserveYawPitch(const FCameraRigEvaluationInfo& CameraRigInfo);
	void TryInitializeYawPitch(const FCameraRigEvaluationInfo& CameraRigInfo, TOptional<double> Yaw, TOptional<double> Pitch);
	void TryPreserveTarget(const FCameraRigEvaluationInfo& CameraRigInfo, bool bUseRelativeTarget);

private:

	FCameraSystemEvaluator* Evaluator = nullptr;

	TWeakPtr<FCameraEvaluationContext> PreviousEvaluationContext;
	FVector3d PreviousContextLocation;
	FRotator3d PreviousContextRotation;
	bool bHasPreviousContextTransform = false;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FVector3d DebugLastEvaluatedTarget;
	FCameraIKAimDebugInfo LastAimDebugInfo;

	friend class FOrientationInitializationDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

}  // namespace UE::Cameras

