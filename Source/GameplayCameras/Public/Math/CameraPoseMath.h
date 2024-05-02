// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/MathFwd.h"
#include "Math/Matrix.h"

class APlayerController;
struct FCameraFieldsOfView;
struct FCameraPose;

namespace UE::Cameras
{

/**
 * Simple struct for holding horizontal and vertical fields of view.
 */
struct FCameraFieldsOfView
{
	double HorizontalFieldOfView;
	double VerticalFieldOfView;
};

/**
 * A utility class for mathematical functions related to a camera pose.
 */
class FCameraPoseMath
{
public:

	/** Gets both horizontal and vertical effective fields of view, using the sensor aspect ratio. */
	static FCameraFieldsOfView GetEffectiveFieldsOfView(const FCameraPose& CameraPose);

	/** Gets both horizontal and vertical effective fields of view, using the given player controller's viewport aspect ratio. */
	static FCameraFieldsOfView GetEffectiveFieldsOfView(const FCameraPose& CameraPose, const APlayerController* PlayerController);

	/** Gets both horizontal and vertical effective fields of view, using the given aspect ratio. */
	static FCameraFieldsOfView GetEffectiveFieldsOfView(const FCameraPose& CameraPose, double AspectRatio);

	/** Gets the aspect ratio of the viewport associated with the given player controller. */
	static double GetEffectiveAspectRatio(const FCameraPose& CameraPose, const APlayerController* PlayerController);

	/** 
	 * Builds the projection matrix of the given camera pose. 
	 * This matrix is suitable for projecting camera-space points onto screen-space.
	 */
	static FMatrix BuildProjectionMatrix(const FCameraPose& CameraPose, double AspectRatio);
	/** 
	 * Builds the view-projection matrix of the given camera pose.
	 * This matrix combines the camera transform and the projection matrix, making it
	 * suitable for projecting world-space points onto screen-space.
	 */
	static FMatrix BuildViewProjectionMatrix(const FCameraPose& CameraPose, double AspectRatio);

	/** Projects the given world point onto screen-space. */
	static TOptional<FVector2d> ProjectWorldToScreen(
			const FCameraPose& CameraPose, double AspectRatio, 
			const FVector3d& WorldLocation, bool bForceLocationInsideFrustum = false);
	/** Projects the given camera-local point onto screen-space. */
	static TOptional<FVector2d> ProjectCameraToScreen(
			const FCameraPose& CameraPose, double AspectRatio, 
			const FVector3d& CameraSpaceLocation, bool bForceLocationInsideFrustum = false);

	/**
	 * Projects the given point onto screen-space.
	 * The caller is responsible for making sure the view-projection matrix and coordinate system 
	 * for the given point are compatible.
	 */
	static TOptional<FVector2d> ProjectToScreen(
			const FMatrix& ViewProjectionMatrix, const FVector3d& Location, bool bForceLocationInsideFrustum = false);

	/** Unproject a screen-space point into a camera-local ray. */
	static FRay3d UnprojectScreenToCamera(
			const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint);
	/** Unproject a screen-space point into a camera-local point given an expected distance. */
	static FVector3d UnprojectScreenToCamera(
			const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint, double PredictedDistance);
	/** Unproject a screen-space point into a world-space ray. */
	static FRay3d UnprojectScreenToWorld(
			const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint);
	/** Unproject a screen-space point into a world-space point given an expected distance. */
	static FVector3d UnprojectScreenToWorld(
			const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint, double PredictedDistance);

	/** Unprojects the given screen-space point into a ray. */
	static FRay3d UnprojectFromScreen(
			const FMatrix& InverseViewProjectionMatrix, const FVector2D& ScreenSpacePoint);
	/** Unprojects the given screen-space point into a point given an expected distance. */
	static FVector3d UnprojectFromScreen(
			const FMatrix& InverseViewProjectionMatrix, const FVector2D& ScreenSpacePoint, double PredictedDistance);

private:

	static FMatrix InverseProjectionMatrix(const FMatrix& ProjectionMatrix);
};

}  // namespace UE::Cameras

