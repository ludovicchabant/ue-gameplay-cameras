// Copyright Epic Games, Inc. All Rights Reserved.

#include "Math/CameraPoseMath.h"

#include "Core/CameraPose.h"
#include "CoreGlobals.h"
#include "GameFramework/PlayerController.h"
#include "Math/InverseRotationMatrix.h"
#include "Math/PerspectiveMatrix.h"
#include "Math/TranslationMatrix.h"
#include "RHI.h"

namespace UE::Cameras
{

FCameraFieldsOfView FCameraPoseMath::GetEffectiveFieldsOfView(const FCameraPose& CameraPose)
{
	return GetEffectiveFieldsOfView(CameraPose, CameraPose.GetSensorAspectRatio());
}

FCameraFieldsOfView FCameraPoseMath::GetEffectiveFieldsOfView(const FCameraPose& CameraPose, const APlayerController* PlayerController)
{
	const double AspectRatio = GetEffectiveAspectRatio(CameraPose, PlayerController);
	return GetEffectiveFieldsOfView(CameraPose, AspectRatio);
}

FCameraFieldsOfView FCameraPoseMath::GetEffectiveFieldsOfView(const FCameraPose& CameraPose, double AspectRatio)
{
	if (CameraPose.GetConstrainAspectRatio() || 
			!ensureMsgf(AspectRatio > 0.0, TEXT("Invalid aspect ratio! Using sensor aspect ratio instead.")))
	{
		// Aspect ratio is constrained, there will be black bars to enforce it to be equal to
		// our sensor aspect ratio.
		return GetEffectiveFieldsOfView(CameraPose, CameraPose.GetSensorAspectRatio());
	}

	const double HorizontalFOV = CameraPose.GetEffectiveFieldOfView();

	// Check the sort of aspect ratio axis constraint we have.
	const EAspectRatioAxisConstraint Constraint(CameraPose.GetAspectRatioAxisConstraint());
	if (Constraint == AspectRatio_MaintainYFOV ||
			(Constraint == AspectRatio_MajorAxisFOV && AspectRatio < 1.0))
	{
		FCameraFieldsOfView FOVs;

		// We need to maintain vertical FOV... the horizontal FOV we have is for our "ideal" aspect ratio,
		// i.e. our sensor's aspect ratio. Now we need to compute the vertical FOV in this ideal situation
		// and re-compute the effective horizontal FOV using the effective aspect ratio.
		const double VerticalFOVRad = 2.0 * FMath::Atan(
				FMath::Tan(FMath::DegreesToRadians(HorizontalFOV / 2.0)) / AspectRatio);
		const double HorizontalFOVRad = 2.0 * FMath::Atan(
				FMath::Tan(VerticalFOVRad / 2.0) * AspectRatio);

		FOVs.HorizontalFieldOfView = FMath::RadiansToDegrees(HorizontalFOVRad);
		FOVs.VerticalFieldOfView = FMath::RadiansToDegrees(VerticalFOVRad);

		return FOVs;
	}
	else
	{
		// Our horizontal FOV is the effective one, so just compute the vertical FOV.
		FCameraFieldsOfView FOVs;

		FOVs.HorizontalFieldOfView = HorizontalFOV;
		FOVs.VerticalFieldOfView = FMath::RadiansToDegrees(2.0 * FMath::Atan(
					FMath::Tan(FMath::DegreesToRadians(FOVs.HorizontalFieldOfView / 2.0)) / AspectRatio));

		return FOVs;
	}
}

double FCameraPoseMath::GetEffectiveAspectRatio(const FCameraPose& CameraPose, const APlayerController* PlayerController)
{
	if (CameraPose.GetConstrainAspectRatio() || PlayerController == nullptr)
	{
		return CameraPose.GetSensorAspectRatio();
	}
	else
	{
		int32 ViewportSizeX = 0, ViewportSizeY = 0;
		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
		if (ensureMsgf(ViewportSizeX > 0 && ViewportSizeY > 0,
				TEXT("Can't get viewport aspect ratio! Using sensor aspect ratio instead.")))
		{
			return (double)ViewportSizeX / (double)ViewportSizeY;
		}
		else
		{
			return CameraPose.GetSensorAspectRatio();
		}
	}
}

FMatrix FCameraPoseMath::BuildProjectionMatrix(const FCameraPose& CameraPose, double AspectRatio)
{
	const double NearClippingPlane = CameraPose.GetNearClippingPlane() > 0.0f ? 
		CameraPose.GetNearClippingPlane() : GNearClippingPlane;
	const double FieldOfView = FMath::Max(CameraPose.GetEffectiveFieldOfView(), 0.001f);

	FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
			FMath::DegreesToRadians(FieldOfView / 2.0),
			AspectRatio,
			1.0f,
			NearClippingPlane);
	//ProjectionMatrix = AdjustProjectionMatrixForRHI(ProjectionMatrix);
	return ProjectionMatrix;
}

FMatrix FCameraPoseMath::BuildViewProjectionMatrix(const FCameraPose& CameraPose, double AspectRatio)
{
	const FTranslationMatrix InverseOrigin(-CameraPose.GetLocation());

	// Somehow we need to also transpose... code stolen from ULocalPlayer::GetProjectionData...
	const FMatrix InverseRotation = FInverseRotationMatrix(CameraPose.GetRotation()) * FMatrix(
		FPlane(0,	0,	1,	0),
		FPlane(1,	0,	0,	0),
		FPlane(0,	1,	0,	0),
		FPlane(0,	0,	0,	1));

	const FMatrix ProjectionMatrix = BuildProjectionMatrix(CameraPose, AspectRatio);

	return InverseOrigin * InverseRotation * ProjectionMatrix;
}

TOptional<FVector2d> FCameraPoseMath::ProjectWorldToScreen(
		const FCameraPose& CameraPose, double AspectRatio, 
		const FVector3d& WorldLocation, bool bForceLocationInsideFrustum)
{
	const FMatrix ViewProjectionMatrix = BuildViewProjectionMatrix(CameraPose, AspectRatio);
	return ProjectToScreen(ViewProjectionMatrix, WorldLocation, bForceLocationInsideFrustum);
}

TOptional<FVector2d> FCameraPoseMath::ProjectCameraToScreen(
			const FCameraPose& CameraPose, double AspectRatio, 
			const FVector3d& CameraSpaceLocation, bool bForceLocationInsideFrustum)
{
	const FMatrix ProjectionMatrix = BuildProjectionMatrix(CameraPose, AspectRatio);
	return ProjectToScreen(ProjectionMatrix, CameraSpaceLocation, bForceLocationInsideFrustum);
}

TOptional<FVector2d> FCameraPoseMath::ProjectToScreen(
		const FMatrix& ViewProjectionMatrix, const FVector3d& Location, bool bForceLocationInsideFrustum)
{
	const FVector4d ProjectedLocation = ViewProjectionMatrix.TransformFVector4(FVector4(Location, 1.0));

	// See if we need to handle the case of a point outside of the view frustum.
	const bool bIsInsideFrustum = (ProjectedLocation.W > 0);
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
	const double RHW = 1.0 / W;
	const FVector4d ScreenSpaceLocation(
			ProjectedLocation.X * RHW, 
			ProjectedLocation.Y * RHW, 
			ProjectedLocation.Z * RHW, 
			ProjectedLocation.W);

	// Move from projection space to normalized 0..1 UI space.
	const double ScreenSpaceX = (ScreenSpaceLocation.X / 2.0) + 0.5;
	const double ScreenSpaceY = 1.0 - (ScreenSpaceLocation.Y / 2.0) - 0.5;

	return FVector2d(ScreenSpaceX, ScreenSpaceY);
}

FRay3d FCameraPoseMath::UnprojectScreenToCamera(const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint)
{
	const FMatrix ProjectionMatrix = BuildProjectionMatrix(CameraPose, AspectRatio);
	const FMatrix InvProjectionMatrix = ProjectionMatrix.InverseFast();
	return UnprojectFromScreen(InvProjectionMatrix, ScreenSpacePoint);
}

FVector3d FCameraPoseMath::UnprojectScreenToCamera(const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint, double PredictedDistance)
{
	const FRay3d UnprojectedRay = UnprojectScreenToCamera(CameraPose, AspectRatio, ScreenSpacePoint);
	const FVector3d WorldPoint = UnprojectedRay.PointAt(PredictedDistance);
	return WorldPoint;
}

FRay3d FCameraPoseMath::UnprojectScreenToWorld(const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint)
{
	const FMatrix ViewProjectionMatrix = BuildViewProjectionMatrix(CameraPose, AspectRatio);
	const FMatrix InvViewProjectionMatrix = ViewProjectionMatrix.InverseFast();
	return UnprojectFromScreen(InvViewProjectionMatrix, ScreenSpacePoint);
}

FVector3d FCameraPoseMath::UnprojectScreenToWorld(const FCameraPose& CameraPose, double AspectRatio, const FVector2D& ScreenSpacePoint, double PredictedDistance)
{
	const FRay3d UnprojectedRay = UnprojectScreenToWorld(CameraPose, AspectRatio, ScreenSpacePoint);
	const FVector3d WorldPoint = UnprojectedRay.PointAt(PredictedDistance);
	return WorldPoint;
}

FRay3d FCameraPoseMath::UnprojectFromScreen(const FMatrix& InverseViewProjectionMatrix, const FVector2D& ScreenSpacePoint)
{
	// Convert the given screen-space point from 0..1 UI space to -1..1 projection space.
	const double ScreenSpaceX = (ScreenSpacePoint.X - 0.5) * 2.0;
	const double ScreenSpaceY = ((1.0 - ScreenSpacePoint.Y) - 0.5) * 2.0;

	// Build a ray from the front of the frustum to the back of the frustum, starting at the screen-space point.
	// We use reverse-Z projection matrices for better precision, so near is Z=1, and far is Z=0.
	const FVector4 RayStartProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, 1.0, 1.0);
	const FVector4 RayEndProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, 0.01, 1.0);

	// Unproject the ray points and normalize them.
	const FVector4 RayStartProjected = InverseViewProjectionMatrix.TransformFVector4(RayStartProjectionSpace);
	const FVector4 RayEndProjected = InverseViewProjectionMatrix.TransformFVector4(RayEndProjectionSpace);

	FVector RayStartWorldSpace(RayStartProjected.X, RayStartProjected.Y, RayStartProjected.Z);
	if (RayStartProjected.W != 0)
	{
		RayStartWorldSpace /= RayStartProjected.W;
	}
	FVector RayEndWorldSpace(RayEndProjected.X, RayEndProjected.Y, RayEndProjected.Z);
	if (RayEndProjected.W != 0)
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

