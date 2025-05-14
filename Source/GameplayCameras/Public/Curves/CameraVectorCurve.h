// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Curves/RichCurve.h"
#include "Math/MathFwd.h"

#include "CameraVectorCurve.generated.h"

#define UE_API GAMEPLAYCAMERAS_API

USTRUCT()
struct FCameraVectorCurve
{
	GENERATED_BODY()

	UPROPERTY()
	FRichCurve Curves[3];

	UE_API FVector GetValue(float InTime) const;
	UE_API bool HasAnyData() const;
};

#undef UE_API
