// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"

#include "BoomArmCameraNode.generated.h"

class UCameraValueInterpolator;
class UInput2DCameraNode;

/**
 * A camera node that can rotate the camera in yaw and pitch based on player input.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Transform"))
class UBoomArmCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeChildrenView OnGetChildren() override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** The offset of the boom. Rotation occurs at the base (i.e. before the offset). */
	UPROPERTY(EditAnywhere, Category=Common)
	FVector3dCameraParameter BoomOffset;

	/** The interpolator to use for changing the boom length based on its pivot's movements. */
	UPROPERTY(EditAnywhere, Category=Common)
	TObjectPtr<UCameraValueInterpolator> BoomLengthInterpolator;

	/** 
	 * The maximum amount of forward movement the interpolator can introduce, expressed
	 * as a factor of the default boom length.
	 */
	UPROPERTY(EditAnywhere, Category=Common)
	FDoubleCameraParameter MaxForwardInterpolationFactor = -1.0;

	/** 
	 * The maximum amount of backward movement the interpolator can introduce, expressed
	 * as a factor of the default boom length.
	 */
	UPROPERTY(EditAnywhere, Category=Common)
	FDoubleCameraParameter MaxBackwardInterpolationFactor = -1.0;

	/**
	 * The input slot for controlling the boom arm.
	 * If no input slot is specified, the boom arm will use the player controller view rotation.
	 */
	UPROPERTY(meta=(ObjectTreeGraphPinDirection=Input))
	TObjectPtr<UInput2DCameraNode> InputSlot;
};

