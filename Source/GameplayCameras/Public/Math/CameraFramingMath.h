// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/MathFwd.h"

#define UE_API GAMEPLAYCAMERAS_API

namespace UE::Cameras
{

struct FCameraFieldsOfView;
struct FFramingZone;
struct FFramingZoneAngles;

/**
 * Utility class for mathematical functions related to framing zones.
 */
class FCameraFramingMath
{
public:

	UE_API static FVector2d GetTargetAngles(const FVector2d& Target, const FCameraFieldsOfView& FieldsOfView);

	/** Gets the framing zone's half-angles for a given camera FOV. */
	UE_API static FFramingZoneAngles GetFramingZoneAngles(const FFramingZone& FramingZone, const FCameraFieldsOfView& FieldsOfView);

private:

	UE_API static double GetBoundAngle(float FactorFromCenter, double TanHalfFOV);
};

}  // namespace UE::Cameras

#undef UE_API

