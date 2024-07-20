// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Math/MathFwd.h"
#include "Nodes/Input/Input2DCameraNode.h"

#include "InputAxisBinding2DCameraNode.generated.h"

class UInputAction;

/**
 * An input node that reads player input from an input action.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Input"))
class UInputAxisBinding2DCameraNode : public UInput2DCameraNode
{
	GENERATED_BODY()

public:

	/** The axis input action(s) to read from. */
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<TObjectPtr<UInputAction>> AxisActions;

	/** Whether to revert the X axis. */
	UPROPERTY(EditAnywhere, Category="Input Processing")
	bool RevertAxisX = false;

	/** Whether to revert the Y axis. */
	UPROPERTY(EditAnywhere, Category="Input Processing")
	bool RevertAxisY = false;

	/** A multiplier to use on the input values. */
	UPROPERTY(EditAnywhere, Category="Input Processing")
	FVector2D Multiplier;

public:

	UInputAxisBinding2DCameraNode(const FObjectInitializer& ObjInit);
	
protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

