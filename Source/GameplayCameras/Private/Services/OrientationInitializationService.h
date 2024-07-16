// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationService.h"
#include "Core/CameraRigTransition.h"

namespace UE::Cameras
{

class FCameraEvaluationContext;
struct FCameraRigEvaluationInfo;

class FOrientationInitializationService : public FCameraEvaluationService
{
protected:

	// FCameraEvaluationService interface.
	virtual void OnInitialize(const FCameraEvaluationServiceInitializeParams& Params) override;
	virtual void OnPostUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult) override;
	virtual void OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent) override;

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
};

}  // namespace UE::Cameras

