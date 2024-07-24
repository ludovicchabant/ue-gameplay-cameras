// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraValueInterpolator.h"
#include "Math/CriticalDamper.h"

#include "CriticalDamperValueInterpolator.generated.h"

/**
 * A value interpolator that uses a spring-mass system to converge towards the target value.
 */
UCLASS(meta=(DisplayName="Critical Damper"))
class UCriticalDamperValueInterpolator : public UCameraValueInterpolator
{
	GENERATED_BODY()

	UE_DECLARE_CAMERA_VALUE_INTERPOLATOR()

public:

	/** The damping factor of the spring-mass system. */
	UPROPERTY(EditAnywhere, Category="Damping")
	float DampingFactor = 1.f;
};

