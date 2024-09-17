// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/PostProcessCameraNode.h"

#include "Core/CameraPose.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PostProcessCameraNode)

namespace UE::Cameras
{

class FPostProcessCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FPostProcessCameraNodeEvaluator)

protected:

	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FPostProcessCameraNodeEvaluator)

void FPostProcessCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FCameraPose& OutPose = OutResult.CameraPose;

	const UPostProcessCameraNode* PostProcessNode = GetCameraNodeAs<UPostProcessCameraNode>();
	OutResult.PostProcessSettings.OverrideChanged(PostProcessNode->PostProcessSettings);
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UPostProcessCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FPostProcessCameraNodeEvaluator>();
}

