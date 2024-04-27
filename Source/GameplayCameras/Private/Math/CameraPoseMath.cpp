// Copyright Epic Games, Inc. All Rights Reserved.

#include "Math/CameraPoseMath.h"

#include "Core/CameraPose.h"
#include "CoreGlobals.h"
#include "Math/InverseRotationMatrix.h"
#include "Math/PerspectiveMatrix.h"
#include "Math/TranslationMatrix.h"
#include "RHI.h"

namespace UE::Cameras
{

FMatrix FCameraPoseMath::BuildProjectionMatrix(const FCameraPose& CameraPose)
{
	const double NearClippingPlane = CameraPose.GetNearClippingPlane() > 0.0f ? 
		CameraPose.GetNearClippingPlane() : GNearClippingPlane;
	const double FieldOfView = FMath::Max(CameraPose.GetEffectiveFieldOfView(), 0.001f);
	const float AspectRatio = CameraPose.GetSensorAspectRatio();

	FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
			FMath::DegreesToRadians(FieldOfView / 2.f),
			AspectRatio,
			1.0f,
			NearClippingPlane);
	//ProjectionMatrix = AdjustProjectionMatrixForRHI(ProjectionMatrix);
	return ProjectionMatrix;
}

FMatrix FCameraPoseMath::BuildViewProjectionMatrix(const FCameraPose& CameraPose)
{
	const FTranslationMatrix InverseOrigin(-CameraPose.GetLocation());

	// Somehow we need to also transpose... code stolen from ULocalPlayer::GetProjectionData...
	const FMatrix InverseRotation = FInverseRotationMatrix(CameraPose.GetRotation()) * FMatrix(
		FPlane(0,	0,	1,	0),
		FPlane(1,	0,	0,	0),
		FPlane(0,	1,	0,	0),
		FPlane(0,	0,	0,	1));

	const FMatrix ProjectionMatrix = BuildProjectionMatrix(CameraPose);

	return InverseOrigin * InverseRotation * ProjectionMatrix;
}

TOptional<FVector2d> FCameraPoseMath::ProjectWorldToScreen(
		const FCameraPose& CameraPose, const FVector3d& WorldLocation, bool bForceLocationInsideFrustum)
{
	const FMatrix ViewProjectionMatrix = BuildViewProjectionMatrix(CameraPose);
	return ProjectToScreen(ViewProjectionMatrix, WorldLocation, bForceLocationInsideFrustum);
}

TOptional<FVector2d> FCameraPoseMath::ProjectCameraToScreen(
			const FCameraPose& CameraPose, const FVector3d& CameraSpaceLocation, bool bForceLocationInsideFrustum)
{
	const FMatrix ProjectionMatrix = BuildProjectionMatrix(CameraPose);
	return ProjectToScreen(ProjectionMatrix, CameraSpaceLocation, bForceLocationInsideFrustum);
}

TOptional<FVector2d> FCameraPoseMath::ProjectToScreen(
		const FMatrix& ViewProjectionMatrix, const FVector3d& Location, bool bForceLocationInsideFrustum)
{
	const FVector4d ProjectedLocation = ViewProjectionMatrix.TransformFVector4(FVector4(Location, 1.f));

	// See if we need to handle the case of a point outside of the view frustum.
	const bool bIsInsideFrustum = (ProjectedLocation.W > 0.f);
	double W = ProjectedLocation.W;
	if (!bIsInsideFrustum)
	{
		if (!bForceLocationInsideFrustum)
		{
			return TOptional<FVector2d>();
		}

		W = FMath::Abs(W);
	}

	// The result of this will be coordinates in -1..1 projection space.
	const double RHW = 1.0f / W;
	const FVector4d ScreenSpaceLocation(
			ProjectedLocation.X * RHW, 
			ProjectedLocation.Y * RHW, 
			ProjectedLocation.Z * RHW, 
			ProjectedLocation.W);

	// Move from projection space to normalized 0..1 UI space.
	const double ScreenSpaceX = (ScreenSpaceLocation.X / 2.f) + 0.5f;
	const double ScreenSpaceY = 1.f - (ScreenSpaceLocation.Y / 2.f) - 0.5f;

	return FVector2d(ScreenSpaceX, ScreenSpaceY);
}

FRay3d FCameraPoseMath::UnprojectScreenToCamera(const FCameraPose& CameraPose, const FVector2D& ScreenSpacePoint)
{
	const FMatrix ProjectionMatrix = BuildProjectionMatrix(CameraPose);
	const FMatrix InvProjectionMatrix = ProjectionMatrix.InverseFast();
	return UnprojectFromScreen(InvProjectionMatrix, ScreenSpacePoint);
}

FVector3d FCameraPoseMath::UnprojectScreenToCamera(const FCameraPose& CameraPose, const FVector2D& ScreenSpacePoint, double PredictedDistance)
{
	const FRay3d UnprojectedRay = UnprojectScreenToCamera(CameraPose, ScreenSpacePoint);
	const FVector3d WorldPoint = UnprojectedRay.PointAt(PredictedDistance);
	return WorldPoint;
}

FRay3d FCameraPoseMath::UnprojectScreenToWorld(const FCameraPose& CameraPose, const FVector2D& ScreenSpacePoint)
{
	const FMatrix ViewProjectionMatrix = BuildViewProjectionMatrix(CameraPose);
	const FMatrix InvViewProjectionMatrix = ViewProjectionMatrix.InverseFast();
	return UnprojectFromScreen(InvViewProjectionMatrix, ScreenSpacePoint);
}

FVector3d FCameraPoseMath::UnprojectScreenToWorld(const FCameraPose& CameraPose, const FVector2D& ScreenSpacePoint, double PredictedDistance)
{
	const FRay3d UnprojectedRay = UnprojectScreenToWorld(CameraPose, ScreenSpacePoint);
	const FVector3d WorldPoint = UnprojectedRay.PointAt(PredictedDistance);
	return WorldPoint;
}

FRay3d FCameraPoseMath::UnprojectFromScreen(const FMatrix& InverseViewProjectionMatrix, const FVector2D& ScreenSpacePoint)
{
	// Convert the given screen-space point from 0..1 UI space to -1..1 projection space.
	const double ScreenSpaceX = (ScreenSpacePoint.X - 0.5f) * 2.f;
	const double ScreenSpaceY = ((1.f - ScreenSpacePoint.Y) - 0.5f) * 2.f;

	// Build a ray from the front of the frustum to the back of the frustum, starting at the screen-space point.
	// We use reverse-Z projection matrices for better precision, so near is Z=1, and far is Z=0.
	const FVector4 RayStartProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, 1.f, 1.f);
	const FVector4 RayEndProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, 0.01f, 1.f);

	// Unproject the ray points and normalize them.
	const FVector4 RayStartProjected = InverseViewProjectionMatrix.TransformFVector4(RayStartProjectionSpace);
	const FVector4 RayEndProjected = InverseViewProjectionMatrix.TransformFVector4(RayEndProjectionSpace);

	FVector RayStartWorldSpace(RayStartProjected.X, RayStartProjected.Y, RayStartProjected.Z);
	if (RayStartProjected.W != 0.f)
	{
		RayStartWorldSpace /= RayStartProjected.W;
	}
	FVector RayEndWorldSpace(RayEndProjected.X, RayEndProjected.Y, RayEndProjected.Z);
	if (RayEndProjected.W != 0.f)
	{
		RayEndWorldSpace /= RayEndProjected.W;
	}

	// Make the 3D ray.
	const bool bDirectionIsNormalized = true;
	const FVector RayDirWorldSpace = (RayEndWorldSpace - RayStartWorldSpace).GetSafeNormal();
	return FRay3d(RayStartWorldSpace, RayDirWorldSpace, bDirectionIsNormalized);
}

FVector3d FCameraPoseMath::UnprojectFromScreen(const FMatrix& InverseViewProjectionMatrix, const FVector2D& ScreenSpacePoint, double PredictedDistance)
{
	const FRay3d UnprojectedRay = UnprojectFromScreen(InverseViewProjectionMatrix, ScreenSpacePoint);
	const FVector3d WorldPoint = UnprojectedRay.PointAt(PredictedDistance);
	return WorldPoint;
}

FMatrix FCameraPoseMath::InverseProjectionMatrix(const FMatrix& ProjectionMatrix)
{
	// Stolen from SceneView.h
	if (ProjectionMatrix.M[1][0] == 0.0f &&
		ProjectionMatrix.M[3][0] == 0.0f &&
		ProjectionMatrix.M[0][1] == 0.0f &&
		ProjectionMatrix.M[3][1] == 0.0f &&
		ProjectionMatrix.M[0][2] == 0.0f &&
		ProjectionMatrix.M[1][2] == 0.0f &&
		ProjectionMatrix.M[0][3] == 0.0f &&
		ProjectionMatrix.M[1][3] == 0.0f &&
		ProjectionMatrix.M[2][3] == 1.0f &&
		ProjectionMatrix.M[3][3] == 0.0f)
	{
		double a = ProjectionMatrix.M[0][0];
		double b = ProjectionMatrix.M[1][1];
		double c = ProjectionMatrix.M[2][2];
		double d = ProjectionMatrix.M[3][2];
		double s = ProjectionMatrix.M[2][0];
		double t = ProjectionMatrix.M[2][1];

		return FMatrix(
			FPlane(1.0 / a, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 1.0 / b, 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 0.0f, 1.0 / d),
			FPlane(-s/a, -t/b, 1.0f, -c/d)
		);
	}
	else
	{
		return ProjectionMatrix.Inverse();
	}
}

}  // namespace UE::Cameras

