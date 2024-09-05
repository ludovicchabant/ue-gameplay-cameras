// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/PostProcessCameraNode.h"

#include "Core/CameraParameterReader.h"
#include "Core/CameraPose.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PostProcessCameraNode)

namespace UE::Cameras
{

class FPostProcessCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FPostProcessCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	TCameraParameterReader<float> PostProcessBlendWeightReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FPostProcessCameraNodeEvaluator)

void FPostProcessCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UPostProcessCameraNode* PostProcessNode = GetCameraNodeAs<UPostProcessCameraNode>();
	PostProcessBlendWeightReader.Initialize(PostProcessNode->PostProcessBlendWeight);
}

void FPostProcessCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FCameraPose& OutPose = OutResult.CameraPose;

	float PostProcessBlendWeight = PostProcessBlendWeightReader.Get(OutResult.VariableTable);
	if (PostProcessBlendWeight > 0)
	{
		const UPostProcessCameraNode* PostProcessNode = GetCameraNodeAs<UPostProcessCameraNode>();
		OutResult.PostProcessSettings.Add(PostProcessNode->PostProcessSettings, PostProcessBlendWeight);
	}
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UPostProcessCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FPostProcessCameraNodeEvaluator>();
}

