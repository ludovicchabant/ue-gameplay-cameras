// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/MathFwd.h"

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

	static FVector2d GetTargetAngles(const FVector2d& Target, const FCameraFieldsOfView& FieldsOfView);

	/** Gets the framing zone's half-angles for a given camera FOV. */
	static FFramingZoneAngles GetFramingZoneAngles(const FFramingZone& FramingZone, const FCameraFieldsOfView& FieldsOfView);

private:

	static double GetBoundAngle(float FactorFromCenter, double TanHalfFOV);
};

}  // namespace UE::Cameras

