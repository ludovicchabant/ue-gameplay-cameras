// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"
#include "Math/MathFwd.h"
#include "Nodes/Input/CameraRigInput2DSlot.h"

#include "InputAxisBinding2DCameraNode.generated.h"

class UInputAction;

/**
 * An input node that reads player input from an input action.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Input"))
class UInputAxisBinding2DCameraNode : public UCameraRigInput2DSlot
{
	GENERATED_BODY()

public:

	/** The axis input action(s) to read from. */
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<TObjectPtr<UInputAction>> AxisActions;

	/** Whether to revert the X axis. */
	UPROPERTY(EditAnywhere, Category="Input Processing")
	FBooleanCameraParameter RevertAxisX = false;

	/** Whether to revert the Y axis. */
	UPROPERTY(EditAnywhere, Category="Input Processing")
	FBooleanCameraParameter RevertAxisY = false;

	/** A multiplier to use on the input values. */
	UPROPERTY(EditAnywhere, Category="Input Processing")
	FVector2dCameraParameter Multiplier;

public:

	UInputAxisBinding2DCameraNode(const FObjectInitializer& ObjInit);
	
protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

