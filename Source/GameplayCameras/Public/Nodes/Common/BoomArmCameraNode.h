// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Nodes/CameraNodeTypes.h"

#include "BoomArmCameraNode.generated.h"

/**
 * A camera node that can rotate the camera in yaw and pitch based on player input.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Transform"))
class UBoomArmCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	UPROPERTY(EditAnywhere, Category=Common)
	FVector3d BoomOffset;
};

