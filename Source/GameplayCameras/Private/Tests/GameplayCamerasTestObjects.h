// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraParameters.h"
#include "Nodes/CameraNodeTypes.h"

#include "GameplayCamerasTestObjects.generated.h"

namespace UE::Cameras::Test
{

struct FTrackedUpdateInfo
{
	float DeltaTime = 0.f;
	bool bIsFirstFrame = false;
	bool bIsCameraCut = false;
};

class FUpdateTrackerCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FUpdateTrackerCameraNodeEvaluator)

public:

	TArray<FTrackedUpdateInfo> ReceivedUpdates;

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
};

}  // namespace UE::Cameras::Tests

UCLASS(MinimalAPI, Hidden)
class UUpdateTrackerCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	UPROPERTY(EditAnywhere, Category=Common)
	FDoubleCameraParameter DoubleParameter;

	UPROPERTY(EditAnywhere, Category=Common)
	FVector3dCameraParameter VectorParameter;
};

