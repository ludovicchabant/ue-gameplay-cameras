// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Framing/PanningFramingCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "Core/CameraVariableAssets.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "GameplayCameras.h"
#include "Math/CameraFramingMath.h"
#include "Math/CameraPoseMath.h"
#include "Math/InverseRotationMatrix.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PanningFramingCameraNode)

namespace UE::Cameras
{

class FPanningFramingCameraNodeEvaluator : public FBaseFramingCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FPanningFramingCameraNodeEvaluator, FBaseFramingCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	FTransform3d BuildPanningShotTransform(const FCameraPose& CameraPose) const;

private:

	TCameraParameterReader<bool> CanPanLaterallyReader;
	TCameraParameterReader<bool> CanPanVerticallyReader;

	FRotator3d PanningRotation;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FRotator3d DebugPanningCorrection;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FPanningFramingCameraNodeEvaluator)

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FPanningFramingCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FRotator3d, PanningRotation);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FRotator3d, PanningCorrection);
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FPanningFramingCameraDebugBlock)

void FPanningFramingCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	Super::OnInitialize(Params, OutResult);

	const UPanningFramingCameraNode* DollyNode = GetCameraNodeAs<UPanningFramingCameraNode>();
	CanPanLaterallyReader.Initialize(DollyNode->CanPanLaterally);
	CanPanVerticallyReader.Initialize(DollyNode->CanPanVertically);

	PanningRotation = FRotator3d::ZeroRotator;
}

void FPanningFramingCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	TOptional<FVector3d> OptTargetLocation = AcquireTargetLocation(Params, OutResult);
	if (!OptTargetLocation.IsSet())
	{
		return;
	}

	// Let the base class figure out all the screen-space framing stuff.
	const FVector3d TargetLocation = OptTargetLocation.GetValue();
	const FTransform3d LastShotTransform = BuildPanningShotTransform(OutResult.CameraPose);
	UpdateFramingState(Params, OutResult, TargetLocation, LastShotTransform);
	ComputeDesiredState(Params.DeltaTime);

	// If we need to reframe the target this tick, figure out how much we need to rotate the camera
	// to accomplish that.
	if (Desired.bHasCorrection)
	{
		FCameraPose LastShotPose(OutResult.CameraPose);
		LastShotPose.SetTransform(LastShotTransform);

		APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController();
		const float AspectRatio = FCameraPoseMath::GetEffectiveAspectRatio(LastShotPose, PlayerController);
		const FCameraFieldsOfView FOVs(FCameraPoseMath::GetEffectiveFieldsOfView(LastShotPose, AspectRatio));

		// Get yaw/pitch angles for where the target is currently at, and where we want it to be at.
		const FVector2d TargetAngles = FCameraFramingMath::GetTargetAngles(State.ScreenTarget, FOVs);
		const FVector2d DesiredAngles = FCameraFramingMath::GetTargetAngles(Desired.ScreenTarget, FOVs);

		// We need to reverse pitch because rotators treat positive pitch as up, whereas we were 
		// treating that as negative (due to the -1..1 normalized space of our math).
		FRotator3d PanningCorrection(
				(DesiredAngles.Y - TargetAngles.Y),	// Pitch
				(TargetAngles.X - DesiredAngles.X),	// Yaw
				0);									// Roll
		PanningRotation += PanningCorrection;

#if UE_GAMEPLAY_CAMERAS_DEBUG
		DebugPanningCorrection = PanningCorrection;
#endif  //UE_GAMEPLAY_CAMERAS_DEBUG
	}
	else
	{
#if UE_GAMEPLAY_CAMERAS_DEBUG
		DebugPanningCorrection = FRotator3d::ZeroRotator;
#endif  //UE_GAMEPLAY_CAMERAS_DEBUG
	}

	const FTransform3d NewShotTransform = BuildPanningShotTransform(OutResult.CameraPose);
	OutResult.CameraPose.SetTransform(NewShotTransform);
}

FTransform3d FPanningFramingCameraNodeEvaluator::BuildPanningShotTransform(const FCameraPose& CameraPose) const
{
	FTransform3d Transform = CameraPose.GetTransform();
	Transform = FTransform3d(PanningRotation) * Transform;
	return Transform;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FPanningFramingCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	Super::OnBuildDebugBlocks(Params, Builder);

	FPanningFramingCameraDebugBlock& DebugBlock = Builder.AttachDebugBlock<FPanningFramingCameraDebugBlock>();
	DebugBlock.PanningRotation = PanningRotation;
	DebugBlock.PanningCorrection = DebugPanningCorrection;
}

void FPanningFramingCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText(
			TEXT("pan yaw/pitch (%0.1f ; %0.1f)  correction (%0.1f ; %0.1f)"), 
			PanningRotation.Yaw, PanningRotation.Pitch,
			PanningCorrection.Yaw, PanningCorrection.Pitch);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

UPanningFramingCameraNode::UPanningFramingCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	CanPanLaterally.Value = true;
	CanPanVertically.Value = true;
}

FCameraNodeEvaluatorPtr UPanningFramingCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FPanningFramingCameraNodeEvaluator>();
}

