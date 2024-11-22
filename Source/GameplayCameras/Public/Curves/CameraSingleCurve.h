// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/RichCurve.h"
#include "Math/MathFwd.h"

#include "CameraSingleCurve.generated.h"

USTRUCT()
struct FCameraSingleCurve
{
	GENERATED_BODY()

	UPROPERTY()
	FRichCurve Curve;

	float GetValue(float InTime) const;
};

