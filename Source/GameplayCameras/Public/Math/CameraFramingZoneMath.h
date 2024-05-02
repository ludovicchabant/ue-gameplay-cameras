// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/MathFwd.h"

class FArchive;
struct FCameraPose;

namespace UE::Cameras
{

struct FCameraFieldsOfView;

/**
 * Effective margins for a rectangular screen-space zone.
 */
struct FFramingZoneMargins
{
	double LeftMargin = 0;
	double TopMargin = 0;
	double RightMargin = 0;
	double BottomMargin = 0;
};

/**
 * Effective coordinates for a rectangular screen-space zone.
 * Unlike FFramingZoneMargins, which stores margin values from the screen's edges,
 * this struct is expected to store actual screen coordinates in 0..1 UI space.
 */
struct FFramingZone
{
	double LeftBound = 0;
	double TopBound = 0;
	double RightBound = 0;
	double BottomBound = 0;

	/** Builds an empty framing zone. */
	FFramingZone();

	/** Build a framing zone from a set of margins. */
	FFramingZone(const FFramingZoneMargins& FramingZoneMargins);

	/** Makes sure all the bounds have valid values between 0 and 1. */
	void ClampBounds();

	/**
	 * Makes sure all the bounds have valid values between 0 and 1, and that the
	 * enclosed rectangle contains the given target point.
	 */
	void ClampBounds(const FVector2d& MustContain);

	/**
	 * Makes sure all the bounds have valid values between 0 and 1, and that the
	 * enclosed rectangle contains the given inner rectangle.
	 */
	void ClampBounds(const FFramingZone& MustContain);

	/** Checks whether the given point (in 0..1 UI space) is inside this zone. */
	bool Contains(const FVector2d& Point) const;

	/** 
	 * Computes intersections between a line and the zone's box, and returns the one closest
	 * to the line's origin point.
	 */
	FVector2d ComputeClosestIntersection(const FVector2d& Origin, const FVector2d& LineDir, bool bLineDirIsNormalized = false) const;

	/** Gets the inner margins of this zone compared to the screen's center. */
	FVector4d GetNormalizedBounds() const;

	/** Gets the coordinates of the top-left corner of the zone, in 0..Width/Height canvas units. */
	FVector2d GetCanvasPosition(const FVector2d& CanvasSize) const;
	/** Gets the size of the zone, in 0..Width/Height canvas units. */
	FVector2d GetCanvasSize(const FVector2d& CanvasSize) const;

	void Serialize(FArchive& Ar);

private:

	static double GetNormalizedBound(double Bound);
};

FArchive& operator <<(FArchive& Ar, FFramingZone& FramingZone);

/**
 * The half-angles (in radians) of a rectangular screen framing zone, relative to the
 * camera pose's aim direction.
 */
struct FFramingZoneAngles
{
	double LeftHalfAngle = 0;
	double TopHalfAngle = 0;
	double RightHalfAngle = 0;
	double BottomHalfAngle = 0;
};

}  // namespace UE::Cameras

