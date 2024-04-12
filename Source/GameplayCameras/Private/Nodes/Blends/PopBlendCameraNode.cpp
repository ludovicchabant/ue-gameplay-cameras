// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Blends/PopBlendCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PopBlendCameraNode)

namespace UE::Cameras
{

class FPopBlendCameraNodeEvaluator : public FBlendCameraNodeEvaluator
{
	UE_DECLARE_BLEND_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FPopBlendCameraNodeEvaluator)

protected:
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnBlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult) override;
};

UE_DEFINE_BLEND_CAMERA_NODE_EVALUATOR(FPopBlendCameraNodeEvaluator)

void FPopBlendCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
}

void FPopBlendCameraNodeEvaluator::OnBlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult)
{
	const FCameraNodeEvaluationResult& ChildResult(Params.ChildResult);
	FCameraNodeEvaluationResult& BlendedResult(OutResult.BlendedResult);
	
	BlendedResult.CameraPose.OverrideChanged(ChildResult.CameraPose);
	if (ChildResult.bIsCameraCut || Params.ChildParams.bIsFirstFrame)
	{
		BlendedResult.bIsCameraCut = true;
	}

	OutResult.bIsBlendFull = true;
	OutResult.bIsBlendFinished = true;
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UPopBlendCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FPopBlendCameraNodeEvaluator>();
}

