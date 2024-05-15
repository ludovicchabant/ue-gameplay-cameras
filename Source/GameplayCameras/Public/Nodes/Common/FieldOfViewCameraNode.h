// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Nodes/CameraNodeTypes.h"

#include "FieldOfViewCameraNode.generated.h"

/**
 * A camera node that sets the field of view of the camera.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Lens"))
class UFieldOfViewCameraNode : public UCameraNode
{
	GENERATED_BODY()

public:

	/** The field of view, in degrees. */
	UPROPERTY(EditAnywhere, Category=Common)
	FFloatCameraParameter FieldOfView;

public:

	UFieldOfViewCameraNode(const FObjectInitializer& ObjectInit);

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

