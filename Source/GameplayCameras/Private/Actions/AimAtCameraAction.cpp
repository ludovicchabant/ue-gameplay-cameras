// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/AimAtCameraAction.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraNodeEvaluatorHierarchy.h"
#include "Core/CameraOperation.h"
#include "Core/CameraRigAsset.h"  // IWYU pragma: keep
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Math/CameraAimingMath.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Services/CameraActionEvaluator.h"
#include "Services/CameraActionScope.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AimAtCameraAction)

namespace UE::Cameras
{

class FAimAtCameraActionEvaluator : public FCameraActionEvaluator
{
	UE_DECLARE_CAMERA_ACTION_EVALUATOR(, FAimAtCameraActionEvaluator)

protected:

	// FCameraActionEvaluator
	virtual void OnInitialize(const FCameraActionEvaluatorInitializeParams& Params, FCameraActionEvaluationResult& OutResult) override;
	virtual void OnPreScopeRun(const FCameraActionEvaluationParams& Params, FCameraActionEvaluationResult& OutResult) override;
	virtual void OnSerialize(const FCameraActionEvaluatorSerializeParams& Params, FArchive& Ar) override;
#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	void LockUserInputThisFrame(const FCameraActionEvaluationParams& Params);
	bool RunPreviewEvaluation(const FCameraActionEvaluationParams& Params, const FCameraNodeEvaluationResult& Result, FRotator3d& OutCorrection);
	bool ExecuteYawPitchCorrection(const FCameraActionEvaluationParams& Params, const FRotator3d& Correction);

	bool ContinueAimAction(const FCameraActionEvaluationParams& Params, FCameraActionEvaluationResult& OutResult);
	bool KeepLockOn(const FCameraActionEvaluationParams& Params, FCameraActionEvaluationResult& OutResult);

private:

	TUniquePtr<TCameraValueInterpolator<FVector2d>> Interpolator;

	FCameraNodeEvaluatorHierarchy ChildHierarchy;

	FCameraNodeEvaluationResult ScratchResult;
	TArray<uint8> EvaluatorSnapshot;

	FVector3d TargetLocation;

	FCameraPose LastCameraPose;
	FVector3d LastContextLocation;
	FVector3d LastPivotLocation;

	bool bIsLockedOn = false;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	float DebugElapsedTime = 0.f;
	FRotator3d DebugCorrectionLeft;
	FRotator3d DebugCurrentCorrection;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

UE_DEFINE_CAMERA_ACTION_EVALUATOR(FAimAtCameraActionEvaluator)

void FAimAtCameraActionEvaluator::OnInitialize(const FCameraActionEvaluatorInitializeParams& Params, FCameraActionEvaluationResult& OutResult)
{
	const UAimAtCameraAction* ActionData = GetCameraActionAs<UAimAtCameraAction>();

	if (ActionData->Interpolator)
	{
		Interpolator = ActionData->Interpolator->BuildVector2dInterpolator();
	}
	else
	{
		Interpolator = MakeUnique<TPopValueInterpolator<FVector2d>>();
	}

	const FCameraRigEvaluationInfo CameraRigEvaluationInfo = Params.Scope->GetCameraRigEvaluationInfo();
	ChildHierarchy.Build(CameraRigEvaluationInfo.RootEvaluator);

	TargetLocation = ActionData->TargetLocation;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	DebugElapsedTime = 0.f;
#endif
}

void FAimAtCameraActionEvaluator::OnPreScopeRun(const FCameraActionEvaluationParams& Params, FCameraActionEvaluationResult& OutResult)
{
	if (Params.EvaluationParams.IsStatelessEvaluation())
	{
		return;
	}

	bool bIsActionStillRunning = true;
	if (bIsLockedOn)
	{
		// We are locked on the target, and are supposed to keep it that way.
		bIsActionStillRunning = KeepLockOn(Params, OutResult);
	}
	else
	{
		// Aim the camera towards the target before the camera rig is run.
		bIsActionStillRunning = ContinueAimAction(Params, OutResult);
	}
	OutResult.bIsActionFinished = !bIsActionStillRunning;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	DebugElapsedTime += Params.EvaluationParams.DeltaTime;
#endif
}

void FAimAtCameraActionEvaluator::LockUserInputThisFrame(const FCameraActionEvaluationParams& Params)
{
	// Execute the operation to turn the camera.
	FCameraOperationParams OperationParams;
	OperationParams.Evaluator = Params.EvaluationParams.Evaluator;
	OperationParams.EvaluationContext = Params.EvaluationParams.EvaluationContext;

	FLockUserInputCameraOperation Operation;
	Operation.bLockThisFrame = true;

	ChildHierarchy.CallExecuteOperation(OperationParams, Operation);
}

bool FAimAtCameraActionEvaluator::RunPreviewEvaluation(const FCameraActionEvaluationParams& Params, const FCameraNodeEvaluationResult& Result, FRotator3d& OutCorrection)
{
	ScratchResult.OverrideAll(Result, true);

	// Save the initial state of the camera rig.
	{
		EvaluatorSnapshot.Reset();

		FCameraNodeEvaluatorSerializeParams SerializeParams;
		FMemoryWriter Writer(EvaluatorSnapshot);
		ChildHierarchy.CallSerialize(SerializeParams, Writer);
	}

	// Run the camera rig.
	{
		FCameraNodeEvaluationParams PreviewParams(Params.EvaluationParams);
		PreviewParams.EvaluationType = ECameraNodeEvaluationType::IK;

		const FCameraRigEvaluationInfo CameraRigEvaluationInfo = Params.Scope->GetCameraRigEvaluationInfo();
		FCameraNodeEvaluator* ChildEvaluator = CameraRigEvaluationInfo.RootEvaluator;
		ChildEvaluator->Run(PreviewParams, ScratchResult);
	}

	// Restore the state of the camera rig.
	{
		FCameraNodeEvaluatorSerializeParams SerializeParams;
		FMemoryReader Reader(EvaluatorSnapshot);
		ChildHierarchy.CallSerialize(SerializeParams, Reader);
	}

	// Find the pivot for this frame.
	const FCameraRigJoint* PreviewPivot = FCameraAimingMath::FindPivotJoint(ScratchResult.CameraRigJoints);
	if (!PreviewPivot)
	{
		// If there's no pivot in the camera, we can't aim it.
		UE_LOG(LogCameraSystem, Warning, 
				TEXT("Can't aim camera rig '%s': it has no pivot joint."),
				*GetNameSafe(Params.Scope->GetCameraRig()));
		return false;
	}

	// Compute the total correction need to aim at the target.
	FRotator3d TotalCorrection;
	const bool bGotCorrection = FCameraAimingMath::ComputeTwoBonesCorrection(
			ScratchResult.CameraPose, PreviewPivot->Transform.GetLocation(), TargetLocation, TotalCorrection);
	if (!bGotCorrection)
	{
		UE_LOG(LogCameraSystem, Warning, 
				TEXT("Can't aim camera rig '%s': we can't solve an IK correction for the desired target."),
				*GetNameSafe(Params.Scope->GetCameraRig()));
		return false;
	}

	OutCorrection = TotalCorrection.GetNormalized();
	return true;
}

bool FAimAtCameraActionEvaluator::ExecuteYawPitchCorrection(const FCameraActionEvaluationParams& Params, const FRotator3d& Correction)
{
	// Execute the operation to turn the camera.
	FCameraOperationParams OperationParams;
	OperationParams.Evaluator = Params.EvaluationParams.Evaluator;
	OperationParams.EvaluationContext = Params.EvaluationParams.EvaluationContext;

	FYawPitchCameraOperation Operation;
	Operation.Yaw = FConsumableDouble::Delta(Correction.Yaw);
	Operation.Pitch = FConsumableDouble::Delta(Correction.Pitch);

	ChildHierarchy.CallExecuteOperation(OperationParams, Operation);

	if (Operation.Yaw.HasValue() || Operation.Pitch.HasValue())
	{
		UE_LOG(LogCameraSystem, Warning, 
				TEXT("Aborting aiming of camera rig '%s': not all corrections were consumed by the camera nodes."),
				*GetNameSafe(Params.Scope->GetCameraRig()));
		return false;
	}

	return true;
}

bool FAimAtCameraActionEvaluator::ContinueAimAction(const FCameraActionEvaluationParams& Params, FCameraActionEvaluationResult& OutResult)
{
	const UAimAtCameraAction* ActionData = GetCameraActionAs<UAimAtCameraAction>();
	if (!ensure(ActionData))
	{
		return false;
	}

	TSharedPtr<const FCameraEvaluationContext> EvaluationContext = Params.EvaluationParams.EvaluationContext;
	if (!ensure(EvaluationContext))
	{
		return false;
	}

	// Prevent the user from turning the camera this frame, since we want to continue aiming.
	LockUserInputThisFrame(Params);

	// Run a preview evaluation of the camera rig to see exactly how much we still need to turn to get to the target.
	FRotator3d TotalCorrection;
	if (!RunPreviewEvaluation(Params, OutResult.Result, TotalCorrection))
	{
		return false;
	}
#if UE_GAMEPLAY_CAMERAS_DEBUG
	DebugCorrectionLeft = TotalCorrection;
#endif

	// See if we're close enough to the target given our tolerance margin.
	if (FMath::Abs(TotalCorrection.Yaw) < ActionData->LockOnAngleTolerance && FMath::Abs(TotalCorrection.Pitch) < ActionData->LockOnAngleTolerance)
	{
		bIsLockedOn = true;
		return (ActionData->LockOnPolicy == EAimAtCameraActionLockOnPolicy::KeepLock);
	}

	// Update the interpolator to know how much we need to turn _this frame_ to get to the target over time.
	{
		FCameraValueInterpolationParams InterpolatorParams;
		InterpolatorParams.DeltaTime = Params.EvaluationParams.DeltaTime;
		InterpolatorParams.bIsCameraCut = OutResult.Result.bIsCameraCut;

		FCameraValueInterpolationResult InterpolatorResult(OutResult.Result.VariableTable);

		Interpolator->Reset(FVector2d(TotalCorrection.Yaw, TotalCorrection.Pitch), FVector2d::ZeroVector);
		Interpolator->Run(InterpolatorParams, InterpolatorResult);
	}

	const FVector2d InterpValue = Interpolator->GetCurrentValue();
	const FRotator3d RemainingCorrection(InterpValue.Y, InterpValue.X, 0.0);
	const FRotator3d CurCorrection = (TotalCorrection - RemainingCorrection).GetNormalized();
#if UE_GAMEPLAY_CAMERAS_DEBUG
	DebugCurrentCorrection = CurCorrection;
#endif

	// Execute the operation to turn the camera.
	if (!ExecuteYawPitchCorrection(Params, CurCorrection))
	{
		return false;
	}

	// If the interpolator has finished, we're done too.
	if (Interpolator->IsFinished())
	{
		bIsLockedOn = true;
		return (ActionData->LockOnPolicy == EAimAtCameraActionLockOnPolicy::KeepLock);
	}

	return true;
}

bool FAimAtCameraActionEvaluator::KeepLockOn(const FCameraActionEvaluationParams& Params, FCameraActionEvaluationResult& OutResult)
{
	// Prevent the user from turning the camera this frame, since we want to keep a lock on the target.
	LockUserInputThisFrame(Params);

	// Run a preview of what the camera will look at this frame, and figure out the correction to keep it locked
	// on the target.
	FRotator3d Correction;
	if (!RunPreviewEvaluation(Params, OutResult.Result, Correction))
	{
		return false;
	}

	// Run the correction on the camera rig.
	if (!ExecuteYawPitchCorrection(Params, Correction))
	{
		return false;
	}

	return true;
}

void FAimAtCameraActionEvaluator::OnSerialize(const FCameraActionEvaluatorSerializeParams& Params, FArchive& Ar)
{
	if (Interpolator)
	{
		FCameraValueInterpolatorSerializeParams InterpolatorParams;
		Interpolator->Serialize(InterpolatorParams, Ar);
	}

	Ar << TargetLocation;

	LastCameraPose.SerializeWithFlags(Ar);
	Ar << LastContextLocation;
	Ar << LastPivotLocation;

	Ar << bIsLockedOn;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(, FAimAtCameraActionDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(float, ElapsedTime)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FRotator3d, CorrectionLeft)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FRotator3d, CurrentCorrection)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(bool, bIsLockedOn)
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

void FAimAtCameraActionEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FAimAtCameraActionDebugBlock& DebugBlock = Builder.AttachDebugBlock<FAimAtCameraActionDebugBlock>();
	DebugBlock.ElapsedTime = DebugElapsedTime;
	DebugBlock.CorrectionLeft = DebugCorrectionLeft;
	DebugBlock.CurrentCorrection = DebugCurrentCorrection;
	DebugBlock.bIsLockedOn = bIsLockedOn;
}

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FAimAtCameraActionDebugBlock)

void FAimAtCameraActionDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	if (bIsLockedOn)
	{
		Renderer.AddText(TEXT("locked on target"));
	}
	else
	{
		Renderer.AddText(
				TEXT("elapsed time %.1f ; correction yaw=%.1f/%.1f  pitch=%.1f/%.1f"),
				ElapsedTime,
				CurrentCorrection.Yaw, CorrectionLeft.Yaw,
				CurrentCorrection.Pitch, CorrectionLeft.Pitch);
	}
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

FCameraActionEvaluatorPtr UAimAtCameraAction::OnBuildEvaluator(FCameraActionEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FAimAtCameraActionEvaluator>();
}

