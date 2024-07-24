// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraValueInterpolator.h"

#include "DoubleIIRValueInterpolator.generated.h"

/**
 * Double infinite impulse response filter interpolator.
 */
UCLASS(meta=(DisplayName="Double-IIR"))
class UDoubleIIRValueInterpolator : public UCameraValueInterpolator
{
	GENERATED_BODY()

	UE_DECLARE_CAMERA_VALUE_INTERPOLATOR()

public:

	/** The primary speed of interpolation. */
	UPROPERTY(EditAnywhere, Category="Speed")
	float PrimarySpeed = 1.f;

	/** The intermediate speed of interpolation. */
	UPROPERTY(EditAnywhere, Category="Speed")
	float IntermediateSpeed = 1.f;

	/** Whether to use fixed-step evaluation. */
	UPROPERTY(EditAnywhere, Category="Interpolation")
	bool bUseFixedStep = true;
};

