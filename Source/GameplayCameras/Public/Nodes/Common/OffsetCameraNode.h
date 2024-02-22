// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Nodes/CameraNodeTypes.h"

#include "OffsetCameraNode.generated.h"

/**
 * A camera node that offsets the location of the camera.
 */
UCLASS(MinimalAPI)
class UOffsetCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** The offset to apply to the camera, in local space. */
	UPROPERTY(EditAnywhere, Category=Common)
	FVector3dCameraParameter Offset;

	/** The space in which to apply the offset. */
	UPROPERTY(EditAnywhere, Category=Common)
	ECameraNodeSpace OffsetSpace = ECameraNodeSpace::CameraPose;
};

