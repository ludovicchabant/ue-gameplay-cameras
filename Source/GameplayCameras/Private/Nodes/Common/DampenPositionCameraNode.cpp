// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/DampenPositionCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "GameplayCameras.h"
#include "Templates/Tuple.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DampenPositionCameraNode)

UDampenPositionCameraNode::UDampenPositionCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	SetNodeFlags(ECameraNodeFlags::RequiresInitialize);
}

void UDampenPositionCameraNode::OnInitialize(const FCameraNodeInitializeParams& Params)
{
	ForwardDamper.SetW0(ForwardDampingFactor);
	ForwardDamper.Reset(0, 0);

	LateralDamper.SetW0(LateralDampingFactor);
	LateralDamper.Reset(0, 0);

	VerticalDamper.SetW0(VerticalDampingFactor);
	VerticalDamper.Reset(0, 0);

	const FCameraNodeRunResult& InitialResult = Params.EvaluationContext->GetInitialResult();
	PreviousLocation = InitialResult.CameraPose.GetLocation();
}

void UDampenPositionCameraNode::OnRun(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult)
{
	// We want the dampen the given camera position, which means it's trying
	// to converge towards the one given in the result (which we set as our 
	// next target), but will be lagging behind.
	const FVector3d NextTarget = OutResult.CameraPose.GetLocation();
	FVector3d NextLocation = NextTarget;

	if (!Params.bIsFirstFrame && !OutResult.bIsCameraCut)
	{
		FTransform3d Transform = OutResult.CameraPose.GetTransform();
		FRotator3d Rotation = OutResult.CameraPose.GetRotation();

		using FAxisDamper = TTuple<FVector3d, FCriticalDamper*>;
		FAxisDamper AxisDampers[3]
		{
			{ Rotation.RotateVector(FVector3d::ForwardVector), &ForwardDamper },
			{ Rotation.RotateVector(FVector3d::RightVector),& LateralDamper },
			{ Rotation.RotateVector(FVector3d::UpVector), &VerticalDamper }
		};

		// The next target has moved further away compared to the previous target,
		// so we're lagging behind even more than before. Compute this new lag vector.
		const FVector3d NewLagVector = NextTarget - PreviousLocation;
		// Let's start at our previous (dampened) location, and see by how much we
		// can catch up on our lag this frame.
		FVector3d NewDampedLocation = PreviousLocation;

		for (FAxisDamper& AxisDamper : AxisDampers)
		{
			FVector3d Axis(AxisDamper.Key);
			FCriticalDamper* Damper(AxisDamper.Value);

			// Compute lag on the forward/lateral/vertical axis, and pass this new
			// lag distance as the new position of the damper. Update it to know 
			// how much we catch up, and offset last frame's position by that amount.
			double NewLagDistance = FVector3d::DotProduct(NewLagVector, Axis);
			// TODO: use GetWorld()->GetWorldSettings()->WorldToMeters
			Damper->Update(NewLagDistance / 100.0, Params.DeltaTime);
			NewDampedLocation += Axis * (NewLagDistance - Damper->GetX0() * 100.0);
		}
		
		NextLocation = NewDampedLocation;
	}

	PreviousLocation = NextLocation;

	OutResult.CameraPose.SetLocation(NextLocation);
}

