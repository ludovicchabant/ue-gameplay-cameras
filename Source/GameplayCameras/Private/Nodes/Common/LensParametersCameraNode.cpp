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

	TCameraParameterReader<float> FocusDistanceReader;
	TCameraParameterReader<float> FocalLengthReader;
	TCameraParameterReader<float> ApertureReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FLensParametersCameraNodeEvaluator)

void FLensParametersCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const ULensParametersCameraNode* LensParametersNode = GetCameraNodeAs<ULensParametersCameraNode>();
	FocusDistanceReader.Initialize(LensParametersNode->FocusDistance);
	FocalLengthReader.Initialize(LensParametersNode->FocalLength);
	ApertureReader.Initialize(LensParametersNode->Aperture);
}

void FLensParametersCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FCameraPose& OutPose = OutResult.CameraPose;

	float FocusDistance = FocusDistanceReader.Get(OutResult.VariableTable);
	if (FocusDistance > 0)
	{
		OutPose.SetFocusDistance(FocusDistance);
	}
	float FocalLength = FocalLengthReader.Get(OutResult.VariableTable);
	if (FocalLength > 0)
	{
		OutPose.SetFocalLength(FocalLength);
		OutPose.SetFieldOfView(-1);
	}
	float Aperture = ApertureReader.Get(OutResult.VariableTable);
	if (Aperture > 0)
	{
		OutPose.SetAperture(Aperture);
	}
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr ULensParametersCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FLensParametersCameraNodeEvaluator>();
}

