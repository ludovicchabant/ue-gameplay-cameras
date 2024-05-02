// Copyright Epic Games, Inc. All Rights Reserved.

#include "Math/CameraFramingZoneMath.h"

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

FVector4f FFramingZone::GetNormalizedBounds() const
{
	// Returned margins are negative if in the left or upper halves, and
	// positive if in the right or lower halves.
	return FVector4f(
		GetNormalizedBound(LeftBound),
		GetNormalizedBound(TopBound),
		GetNormalizedBound(RightBound),
		GetNormalizedBound(BottomBound));
}

float FFramingZone::GetNormalizedBound(float Bound)
{
	return (Bound - 0.5f) * 2.f;
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

}  // namespace UE::Cameras

