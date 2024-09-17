// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/LensParametersCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LensParametersCameraNode)

namespace UE::Cameras
{

class FLensParametersCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FLensParametersCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	TCameraParameterReader<float> FocalLengthReader;
	TCameraParameterReader<float> FocusDistanceReader;
	TCameraParameterReader<float> ApertureReader;
	TCameraParameterReader<float> ShutterSpeedReader;
	TCameraParameterReader<bool> EnablePhysicalCameraReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FLensParametersCameraNodeEvaluator)

void FLensParametersCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const ULensParametersCameraNode* LensParametersNode = GetCameraNodeAs<ULensParametersCameraNode>();
	FocalLengthReader.Initialize(LensParametersNode->FocalLength);
	FocusDistanceReader.Initialize(LensParametersNode->FocusDistance);
	ApertureReader.Initialize(LensParametersNode->Aperture);
	ShutterSpeedReader.Initialize(LensParametersNode->ShutterSpeed);
	EnablePhysicalCameraReader.Initialize(LensParametersNode->EnablePhysicalCamera);
}

void FLensParametersCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FCameraPose& OutPose = OutResult.CameraPose;

	float FocalLength = FocalLengthReader.Get(OutResult.VariableTable);
	if (FocalLength > 0)
	{
		OutPose.SetFocalLength(FocalLength);
		OutPose.SetFieldOfView(-1);
	}
	float FocusDistance = FocusDistanceReader.Get(OutResult.VariableTable);
	if (FocusDistance > 0)
	{
		OutPose.SetFocusDistance(FocusDistance);
	}
	float Aperture = ApertureReader.Get(OutResult.VariableTable);
	if (Aperture > 0)
	{
		OutPose.SetAperture(Aperture);
	}
	float ShutterSpeed = ShutterSpeedReader.Get(OutResult.VariableTable);
	if (ShutterSpeed > 0)
	{
		OutPose.SetShutterSpeed(ShutterSpeed);
	}

	const bool bEnablePhysicalCamera = EnablePhysicalCameraReader.Get(OutResult.VariableTable);
	OutPose.SetEnablePhysicalCamera(bEnablePhysicalCamera);
	OutPose.SetPhysicalCameraBlendWeight(bEnablePhysicalCamera ? 1.f : 0.f);
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr ULensParametersCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FLensParametersCameraNodeEvaluator>();
}

