// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ShakeCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ShakeCameraNode)

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FShakeCameraNodeEvaluator)

void FShakeCameraNodeEvaluator::ShakeResult(const FCameraNodeShakeParams& Params, FCameraNodeShakeResult& OutResult)
{
	OnShakeResult(Params, OutResult);
}

void FShakeCameraNodeEvaluator::RestartShake(const FCameraNodeShakeRestartParams& Params)
{
	OnRestartShake(Params);
}

}  // namespace UE::Cameras

