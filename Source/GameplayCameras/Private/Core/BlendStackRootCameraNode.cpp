// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackRootCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraRigAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackRootCameraNode)

FCameraNodeChildrenView UBlendStackRootCameraNode::OnGetChildren()
{
	FCameraNodeChildrenView Children;
	if (Blend)
	{
		Children.Add(Blend);
	}
	if (RootNode)
	{
		Children.Add(RootNode);
	}
	return Children;
}

FCameraNodeEvaluatorPtr UBlendStackRootCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FBlendStackRootCameraNodeEvaluator>();
}

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendStackRootCameraNodeEvaluator)

FCameraNodeEvaluatorChildrenView FBlendStackRootCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView Children;
	if (BlendEvaluator)
	{
		Children.Add(BlendEvaluator);
	}
	if (RootEvaluator)
	{
		Children.Add(RootEvaluator);
	}
	return Children;
}

void FBlendStackRootCameraNodeEvaluator::OnBuild(const FCameraNodeEvaluatorBuildParams& Params)
{
	const UBlendStackRootCameraNode* RootNode = GetCameraNodeAs<UBlendStackRootCameraNode>();
	BlendEvaluator = Params.BuildEvaluatorAs<FBlendCameraNodeEvaluator>(RootNode->Blend);
	RootEvaluator = Params.BuildEvaluator(RootNode->RootNode);
}

void FBlendStackRootCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (BlendEvaluator)
	{
		BlendEvaluator->Run(Params, OutResult);
	}
	if (RootEvaluator)
	{
		RootEvaluator->Run(Params, OutResult);
	}
}

}  // namespace UE::Cameras

