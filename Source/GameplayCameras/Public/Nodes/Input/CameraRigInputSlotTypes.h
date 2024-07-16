// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

#include "CameraRigInputSlotTypes.generated.h"

/**
 * General options for an input slot.
 */
USTRUCT()
struct FCameraRigInputSlotParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input")
	bool bIsAccumulated = true;

	UPROPERTY(EditAnywhere, Category="Input")
	bool bIsPreBlended = true;
};

/**
 * Value clamping parameters.
 */
USTRUCT()
struct FCameraParameterClamping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Common", meta=(EditCondition="bClampMin"))
	double MinValue = 0;

	UPROPERTY(EditAnywhere, Category ="Common", meta=(EditCondition="bClampMax"))
	double MaxValue = 0;

	UPROPERTY(EditAnywhere, Category="Common")
	bool bClampMin = false;

	UPROPERTY(EditAnywhere, Category="Common")
	bool bClampMax = false;

public:

	/** Clamps the given value. */
	double ClampValue(double Value) const;

	/** 
	 * Gets the effective min/max values for this struct.
	 * If a bound is disabled, the effective value will be lowest or max
	 * double precision values.
	 */
	void GetEffectiveClamping(double& OutMinValue, double& OutMaxValue) const;
};

/**
 * Value normalization parameters.
 */
USTRUCT()
struct FCameraParameterNormalization
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Common", meta=(ClampMin=0, EditCondition="bNormalize"))
	double MaxValue = 0;

	UPROPERTY(EditAnywhere, Category="Common")
	bool bNormalize = false;

public:

	/** Normalizes the given value. */
	double NormalizeValue(double Value) const;
};

