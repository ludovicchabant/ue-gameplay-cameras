// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/DampenPositionCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNodeEvaluator.h"
#include "GameplayCameras.h"
#include "Math/CriticalDamper.h"
#include "Templates/Tuple.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DampenPositionCameraNode)

namespace UE::Cameras
{

class FDampenPositionCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FDampenPositionCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	FCriticalDamper ForwardDamper;
	FCriticalDamper LateralDamper;
	FCriticalDamper VerticalDamper;

	FVector3d PreviousLocation;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FDampenPositionCameraNodeEvaluator)

void FDampenPositionCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	const UDampenPositionCameraNode* DampenNode = GetCameraNodeAs<UDampenPositionCameraNode>();

	ForwardDamper.SetW0(DampenNode->ForwardDampingFactor);
	ForwardDamper.Reset(0, 0);

	LateralDamper.SetW0(DampenNode->LateralDampingFactor);
	LateralDamper.Reset(0, 0);

	VerticalDamper.SetW0(DampenNode->VerticalDampingFactor);
	VerticalDamper.Reset(0, 0);

	const FCameraNodeEvaluationResult& InitialResult = Params.EvaluationContext->GetInitialResult();
	PreviousLocation = InitialResult.CameraPose.GetLocation();
}

void FDampenPositionCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
#if WITH_EDITOR
	const UDampenPositionCameraNode* DampenNode = GetCameraNodeAs<UDampenPositionCameraNode>();
	ForwardDamper.SetW0(DampenNode->ForwardDampingFactor);
	LateralDamper.SetW0(DampenNode->LateralDampingFactor);
	VerticalDamper.SetW0(DampenNode->VerticalDampingFactor);
#endif

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

}  // namespace UE::Cameras

UDampenPositionCameraNode::UDampenPositionCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

FCameraNodeEvaluatorPtr UDampenPositionCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FDampenPositionCameraNodeEvaluator>();
}

