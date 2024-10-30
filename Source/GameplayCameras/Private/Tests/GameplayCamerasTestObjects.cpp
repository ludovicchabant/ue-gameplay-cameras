// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasTestObjects.h"

namespace UE::Cameras::Test
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FUpdateTrackerCameraNodeEvaluator)

void FUpdateTrackerCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	ReceivedUpdates.Add(FTrackedUpdateInfo
			{
				Params.DeltaTime,
				Params.bIsFirstFrame,
				OutResult.bIsCameraCut
			});
}

}  // namespace UE::Cameras::Tests

FCameraNodeEvaluatorPtr UUpdateTrackerCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras::Test;
	return Builder.BuildEvaluator<FUpdateTrackerCameraNodeEvaluator>();
}

