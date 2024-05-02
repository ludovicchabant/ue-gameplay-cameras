// Copyright Epic Games, Inc. All Rights Reserved.

#include "Math/CameraFramingMath.h"

#include "Math/CameraFramingZoneMath.h"
#include "Math/CameraPoseMath.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector2D.h"
#include "Math/Vector4.h"

namespace UE::Cameras
{

FVector2d FCameraFramingMath::GetTargetAngles(const FVector2d& Target, const FCameraFieldsOfView& FieldsOfView)
{
	const float TanHalfHorizontalFOV = FMath::Tan(FMath::DegreesToRadians(FieldsOfView.HorizontalFieldOfView / 2.f));
	const float TanHalfVerticalFOV = FMath::Tan(FMath::DegreesToRadians(FieldsOfView.VerticalFieldOfView / 2.f));

	// Target is in 0..1 UI space... convert to -1..1 space.
	const double NormalizedTargetX = (Target.X - 0.5) * 2.0;
	const double NormalizedTargetY = (Target.Y - 0.5) * 2.0;

	const double BoundAngleYaw = GetBoundAngle(NormalizedTargetX, TanHalfHorizontalFOV);
	const double BoundAnglePitch = GetBoundAngle(NormalizedTargetY, TanHalfVerticalFOV);

	return FVector2d(
			FMath::RadiansToDegrees(BoundAngleYaw),
			FMath::RadiansToDegrees(BoundAnglePitch));
}

FFramingZoneAngles FCameraFramingMath::GetFramingZoneAngles(const FFramingZone& FramingZone, const FCameraFieldsOfView& FieldsOfView)
{
	const float TanHalfHorizontalFOV = FMath::Tan(FMath::DegreesToRadians(FieldsOfView.HorizontalFieldOfView / 2.f));
	const float TanHalfVerticalFOV = FMath::Tan(FMath::DegreesToRadians(FieldsOfView.VerticalFieldOfView / 2.f));

	const FVector4d BoundFactorsFromCenter = FramingZone.GetNormalizedBounds();

	const double LeftHalfAngleRad = GetBoundAngle(BoundFactorsFromCenter.X, TanHalfHorizontalFOV);
	const double TopHalfAngleRad = GetBoundAngle(BoundFactorsFromCenter.Y, TanHalfVerticalFOV);
	const double RightHalfAngleRad = GetBoundAngle(BoundFactorsFromCenter.Z, TanHalfHorizontalFOV);
	const double BottomHalfAngleRad = GetBoundAngle(BoundFactorsFromCenter.W, TanHalfVerticalFOV);

	FFramingZoneAngles Angles;
	Angles.LeftHalfAngle = FMath::DegreesToRadians(LeftHalfAngleRad); 
	Angles.TopHalfAngle = FMath::DegreesToRadians(TopHalfAngleRad); 
	Angles.RightHalfAngle = FMath::DegreesToRadians(RightHalfAngleRad); 
	Angles.BottomHalfAngle = FMath::DegreesToRadians(BottomHalfAngleRad); 
	return Angles;
}

double FCameraFramingMath::GetBoundAngle(float FactorFromCenter, double TanHalfFOV)
{
	// The FactorFromCenter should always be a percentage from the center of the screen.
	// So a factor of zero is the center, a factor of 0.5 is half way between the center and the edge,
	// and a factor of 1 is at the edge.
	//
	// Now here is what we do: let's consider a plane, orthogonal to the camera's aim direction, and
	// located at a distance L from the camera:
	//
	//		tan(fov/2) = W/L
	//
	// Where W is the half-width of that plane, and fov is the horizontal field of view angle.
	//
	// We can do the same for the bound whose angle we're trying to determine. Since this factor (let's
	// call it m) is a percentage of the plane's half-width from the center, we also have:
	// 
	//		tan(x) = W*m/L
	//
	// Where x is what we want (the angle for that bound).
	//
	// So:
	//	
	//		W = tan(fov/2) * L
	//		tan(x) = tan(fov/2) * L * m/L
	//		tan(x) = tan(fov/2) * m
	//		x = atan(tan(fov/2) * m)
	//
	const float LocalFactor = FMath::Abs(FactorFromCenter);
	const double BoundAngle = FMath::Atan(TanHalfFOV * LocalFactor);
	return FactorFromCenter <= 0.f ? -BoundAngle : BoundAngle;
}

}  // namespace UE::Cameras

