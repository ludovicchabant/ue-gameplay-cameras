// Copyright Epic Games, Inc. All Rights Reserved.

#include "Math/CameraFramingZoneMath.h"

#include "Core/CameraPose.h"
#include "Nodes/Framing/CameraFramingZone.h"

namespace UE::Cameras
{

FFramingZone::FFramingZone()
{
	LeftBound = 0;
	TopBound = 0;
	RightBound = 1;
	BottomBound = 1;
}

FFramingZone::FFramingZone(const FFramingZoneMargins& FramingZoneMargins)
{
	LeftBound = FramingZoneMargins.LeftMargin;
	TopBound = FramingZoneMargins.TopMargin;
	RightBound = 1.0f - FramingZoneMargins.RightMargin;
	BottomBound = 1.0f - FramingZoneMargins.BottomMargin;

	ClampBounds();
}

void FFramingZone::ClampBounds()
{
	LeftBound = FMath::Clamp(LeftBound, 0.f, 1.f);
	TopBound = FMath::Clamp(TopBound, 0.f, 1.f);
	RightBound = FMath::Clamp(RightBound, 0.f, 1.f);
	BottomBound = FMath::Clamp(BottomBound, 0.f, 1.f);

	RightBound = FMath::Max(LeftBound, RightBound);
	BottomBound = FMath::Max(TopBound, BottomBound);
}

void FFramingZone::ClampBounds(const FVector2d& MustContain)
{
	LeftBound = FMath::Clamp(LeftBound, 0.f, MustContain.X);
	TopBound = FMath::Clamp(TopBound, 0.f, MustContain.Y);
	RightBound = FMath::Clamp(RightBound, MustContain.X, 1.f);
	BottomBound = FMath::Clamp(BottomBound, MustContain.Y, 1.f);
}

void FFramingZone::ClampBounds(const FFramingZone& MustContain)
{
	LeftBound = FMath::Clamp(LeftBound, 0.f, MustContain.LeftBound);
	TopBound = FMath::Clamp(TopBound, 0.f, MustContain.TopBound);
	RightBound = FMath::Clamp(RightBound, MustContain.RightBound, 1.f);
	BottomBound = FMath::Clamp(BottomBound, MustContain.BottomBound, 1.f);
}

bool FFramingZone::Contains(const FVector2d& Point) const
{
	return Point.X >= LeftBound && Point.X <= RightBound &&
		Point.Y >= TopBound && Point.Y <= BottomBound;
}

FVector4d FFramingZone::GetMarginsFromCenter() const
{
	return FVector4d(
		0.5 - LeftBound,
		0.5 - TopBound,
		0.5 - (1.0 - RightBound),
		0.5 - (1.0 - BottomBound));
}

FVector2d FFramingZone::GetCanvasPosition(const FVector2d& CanvasSize) const
{
	return FVector2d(LeftBound * CanvasSize.X, TopBound * CanvasSize.Y);
}

FVector2d FFramingZone::GetCanvasSize(const FVector2d& CanvasSize) const
{
	return FVector2d((RightBound - LeftBound) * CanvasSize.X, (BottomBound - TopBound) * CanvasSize.Y);
}

void FFramingZone::Serialize(FArchive& Ar)
{
	Ar << LeftBound;
	Ar << TopBound;
	Ar << RightBound;
	Ar << BottomBound;
}

FArchive& operator <<(FArchive& Ar, FFramingZone& FramingZone)
{
	FramingZone.Serialize(Ar);
	return Ar;
}

FFramingZoneAngles FFramingZoneMath::GetFramingZoneAngles(const FFramingZone& FramingZone, const FCameraFieldsOfView& FieldsOfView)
{
	const float BackingHalfWidth = FMath::Tan(FMath::DegreesToRadians(FieldsOfView.HorizontalFieldOfView / 2.f));
	const float BackingHalfHeight = FMath::Tan(FMath::DegreesToRadians(FieldsOfView.VerticalFieldOfView / 2.f));

	const FVector4d MarginsFromCenter = FramingZone.GetMarginsFromCenter();

	FFramingZoneAngles Angles;
	Angles.LeftHalfAngle = GetFramingMarginAngle(MarginsFromCenter.X, BackingHalfWidth);
	Angles.TopHalfAngle = GetFramingMarginAngle(MarginsFromCenter.Y, BackingHalfHeight);
	Angles.RightHalfAngle = GetFramingMarginAngle(MarginsFromCenter.Z, BackingHalfWidth);
	Angles.BottomHalfAngle = GetFramingMarginAngle(MarginsFromCenter.W, BackingHalfHeight);
	return Angles;
}

double FFramingZoneMath::GetFramingMarginAngle(float MarginFromCenter, float BackingHalfSize)
{
	// The margin should always be a percentage from the center of the screen.
	// So a margin of zero is the center, a margin of 0.5 is half way between the center and the edge,
	// and a margin of 1 is at the edge.
	const double MarginSize = FMath::Clamp(MarginFromCenter, -1.f, 1.f) * BackingHalfSize;
	const double MarginAngle = FMath::Atan(MarginSize);
	return MarginAngle;
}

}  // namespace UE::Cameras

