// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/RichCurve.h"
#include "Math/MathFwd.h"

#include "CameraRotatorCurve.generated.h"

USTRUCT()
struct GAMEPLAYCAMERAS_API FCameraRotatorCurve
{
	GENERATED_BODY()

	UPROPERTY()
	FRichCurve Curves[3];

	FRotator GetValue(float InTime) const;
	bool HasAnyData() const;
};

