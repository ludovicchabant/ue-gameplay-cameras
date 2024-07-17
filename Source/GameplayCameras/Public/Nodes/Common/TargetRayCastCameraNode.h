// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"

#include "TargetRayCastCameraNode.generated.h"

/**
 * A camera node that determines and sets the camera's target by running a ray-cast
 * from the current camera position.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Target"))
class UTargetRayCastCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

