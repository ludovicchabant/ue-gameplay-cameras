// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendCameraNode)

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendCameraNodeEvaluator)

void FBlendCameraNodeEvaluator::BlendParameters(const FCameraNodePreBlendParams& Params, FCameraNodePreBlendResult& OutResult)
{
	OnBlendParameters(Params, OutResult);
}

void FBlendCameraNodeEvaluator::BlendResults(const FCameraNodeBlendParams& Params, FCameraNodeBlendResult& OutResult)
{
	OnBlendResults(Params, OutResult);
}

}  // namespace UE::Cameras

