// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/RichCurve.h"
#include "Math/MathFwd.h"

#include "CameraRotatorCurve.generated.h"

#define UE_API GAMEPLAYCAMERAS_API

USTRUCT()
struct FCameraRotatorCurve
{
	GENERATED_BODY()

	UPROPERTY()
	FRichCurve Curves[3];

	UE_API FRotator GetValue(float InTime) const;
	UE_API bool HasAnyData() const;
};

#undef UE_API
