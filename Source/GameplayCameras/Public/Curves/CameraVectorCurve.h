// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/RichCurve.h"
#include "Math/MathFwd.h"

#include "CameraVectorCurve.generated.h"

USTRUCT()
struct GAMEPLAYCAMERAS_API FCameraVectorCurve
{
	GENERATED_BODY()

	UPROPERTY()
	FRichCurve Curves[3];

	FVector GetValue(float InTime) const;
	bool HasAnyData() const;
};

