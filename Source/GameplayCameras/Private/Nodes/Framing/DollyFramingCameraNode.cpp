// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Framing/DollyFramingCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCameras.h"
#include "Math/CameraFramingZoneMath.h"
#include "Math/CameraPoseMath.h"
#include "Math/InverseRotationMatrix.h"

#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "SceneView.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DollyFramingCameraNode)

namespace UE::Cameras
{

class FDollyFramingCameraNodeEvaluator : public FBaseFramingCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FDollyFramingCameraNodeEvaluator, FBaseFramingCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	FVector3d ComputeFramingTranslation(const FCameraPose& CameraPose);

private:

	TCameraParameterReader<bool> CanMoveLaterallyReader;
	TCameraParameterReader<bool> CanMoveVerticallyReader;

	FVector2d DollyPosition;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FVector3d DebugUnprojectedNextScreenTarget;
	FVector2d DebugDollyCorrection;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FDollyFramingCameraNodeEvaluator)

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FDollyFramingCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FVector2d, DollyPosition);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FVector2d, DollyCorrection);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FVector3d, WorldTarget);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FVector3d, UnprojectedNextScreenTarget);
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FDollyFramingCameraDebugBlock)

void FDollyFramingCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	Super::OnInitialize(Params);

	const UDollyFramingCameraNode* DollyNode = GetCameraNodeAs<UDollyFramingCameraNode>();
	CanMoveLaterallyReader.Initialize(DollyNode->CanMoveLaterally);
	CanMoveVerticallyReader.Initialize(DollyNode->CanMoveVertically);

	DollyPosition = FVector2d::ZeroVector;
}

void FDollyFramingCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// TODO get the target some other way
	APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController();
	APawn* Pawn = PlayerController->GetPawn();
	const FVector3d TargetLocation = Pawn->GetActorLocation();

	// Let the base class figure out all the screen-space framing stuff.
	UpdateFramingState(OutResult, TargetLocation);
	ComputeDesiredState(Params.DeltaTime);

	// If we need to reframe the target this tick, figure out how much we need to move the dolly
	// to accomplish that.
	if (Desired.bHasCorrection)
	{
		const FTransform& LastFraming = GetLastFraming();
		FCameraPose LastFramingPose(OutResult.CameraPose);
		LastFramingPose.SetTransform(LastFraming);

		FVector3d DesiredLocalOffset = ComputeFramingTranslation(LastFramingPose);

		// We never bring the dolly forward or backward (we only move it vertically or horizontally).
		DesiredLocalOffset.X = 0.f;

		if (!CanMoveLaterallyReader.Get(OutResult.VariableTable))
		{
			DesiredLocalOffset.Y = 0.f;
		}
		if (!CanMoveVerticallyReader.Get(OutResult.VariableTable))
		{
			DesiredLocalOffset.Z = 0.f;
		}

		const FVector2d DollyCorrection = FVector2d(DesiredLocalOffset.Y, DesiredLocalOffset.Z);
		DollyPosition += DollyCorrection;

#if UE_GAMEPLAY_CAMERAS_DEBUG
		DebugDollyCorrection = DollyCorrection;
#endif  //UE_GAMEPLAY_CAMERAS_DEBUG
	}

	// Build the new shot transform and register it with the base class for next tick.
	FTransform3d Transform = OutResult.CameraPose.GetTransform();
	Transform = FTransform3d(FVector3d(0, DollyPosition.X, DollyPosition.Y)) * Transform;
	OutResult.CameraPose.SetTransform(Transform);

	RegisterNewFraming(Transform);
}

FVector3d FDollyFramingCameraNodeEvaluator::ComputeFramingTranslation(const FCameraPose& CameraPose)
{
	// Unproject the desired screen-space position of our target for this tick. This normally gives us
	// a ray into the camera frustum, since there is an infinity of world-space points that correspond
	// to a point on the screen... we arbitrarily pick a point along this ray that is at the same 
	// distance as our target, since generally it's not far from there.
	const FVector3d CameraToTarget(State.WorldTarget - CameraPose.GetLocation());
	const double TargetDist(CameraToTarget.Length());

	const FVector2D DesiredScreenTarget(Desired.ScreenTarget);
	FVector3d RoughDesiredWorldTarget = FCameraPoseMath::UnprojectScreenToWorld(CameraPose, DesiredScreenTarget, TargetDist);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	DebugUnprojectedNextScreenTarget = RoughDesiredWorldTarget;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

	// Now we know that we need to translate the dolly by that much in world-space. Convert that to
	// a camera-space offset, since we want to decompose that into lateral and vertical dolly 
	// movement.
	const FVector3d DesiredWorldOffset(State.WorldTarget - RoughDesiredWorldTarget);
	const FInverseRotationMatrix InverseRotation(CameraPose.GetRotation());
	const FVector3d DesiredLocalOffset = InverseRotation.TransformVector(DesiredWorldOffset);
	return DesiredLocalOffset;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FDollyFramingCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	Super::OnBuildDebugBlocks(Params, Builder);

	FDollyFramingCameraDebugBlock& DebugBlock = Builder.AttachDebugBlock<FDollyFramingCameraDebugBlock>();
	DebugBlock.DollyPosition = DollyPosition;
	DebugBlock.DollyCorrection = DebugDollyCorrection;
	DebugBlock.WorldTarget = State.WorldTarget;
	DebugBlock.UnprojectedNextScreenTarget = DebugUnprojectedNextScreenTarget;
}

void FDollyFramingCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText(
			TEXT("dolly position (%0.1f ; %0.1f)  correction (%0.1f ; %0.1f)"), 
			DollyPosition.X, DollyPosition.Y,
			DollyCorrection.X, DollyCorrection.Y);

	Renderer.DrawLine(WorldTarget, UnprojectedNextScreenTarget, FLinearColor(FColorList::NavyBlue), 1.f);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

UDollyFramingCameraNode::UDollyFramingCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	CanMoveLaterally.Value = true;
	CanMoveVertically.Value = true;
}

FCameraNodeEvaluatorPtr UDollyFramingCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FDollyFramingCameraNodeEvaluator>();
}

