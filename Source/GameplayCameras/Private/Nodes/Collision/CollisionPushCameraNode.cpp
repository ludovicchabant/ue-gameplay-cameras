// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Collision/CollisionPushCameraNode.h"

#include "CollisionQueryParams.h"
#include "Core/BuiltInCameraVariables.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraParameterReader.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/CameraValueInterpolator.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Engine/Engine.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCameras.h"
#include "Misc/AssertionMacros.h"
#include "WorldCollision.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CollisionPushCameraNode)

namespace UE::Cameras
{

class FCollisionPushCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FCollisionPushCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	TOptional<FVector3d> GetSafePosition(const FCameraNodeEvaluationParams& Params, const FCameraNodeEvaluationResult& OutResult);
	void RunCollisionTrace(UWorld* World, APlayerController* PlayerController, const FVector3d& SafePosition, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);
	void HandleAsyncCollisionTraceResult(UWorld* World, const FVector3d& SafePosition, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);
	void HandleCollisionTraceResult(UWorld* World, TArrayView<const FHitResult> HitResults, const FVector3d& SafePosition, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

private:

	TCameraParameterReader<float> CollisionSphereRadiusReader;
	TCameraParameterReader<FVector3d> SafePositionOffsetReader;

	TUniquePtr<FCameraDoubleValueInterpolator> PushInterpolator;
	TUniquePtr<FCameraDoubleValueInterpolator> PullInterpolator;

	FTraceHandle CollisionTraceHandle;

	float LastPushFactor = 0.f;
	float LastDampedPushFactor = 0.f;

	enum class ECameraCollisionDirection { Pushing, Pulling };
	ECameraCollisionDirection LastDirection = ECameraCollisionDirection::Pushing;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	bool bDebugFoundHit = false;
	FString DebugHitObjectName;
	FVector3d DebugSafePosition;
#endif
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FCollisionPushCameraNodeEvaluator)

UE_DECLARE_CAMERA_DEBUG_BLOCK_START(GAMEPLAYCAMERAS_API, FCollisionPushCameraDebugBlock)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(float, PushFactor);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(float, DampedPushFactor);
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(bool, bIsPulling)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(bool, bFoundHit)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FString, HitObjectName)
	UE_DECLARE_CAMERA_DEBUG_BLOCK_FIELD(FVector3d, SafePosition)
UE_DECLARE_CAMERA_DEBUG_BLOCK_END()

UE_DEFINE_CAMERA_DEBUG_BLOCK_WITH_FIELDS(FCollisionPushCameraDebugBlock)

void FCollisionPushCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UCollisionPushCameraNode* CollisionPushNode = GetCameraNodeAs<UCollisionPushCameraNode>();

	CollisionSphereRadiusReader.Initialize(CollisionPushNode->CollisionSphereRadius);
	SafePositionOffsetReader.Initialize(CollisionPushNode->SafePositionOffset);

	PushInterpolator = CollisionPushNode->PushInterpolator ?
		CollisionPushNode->PushInterpolator->BuildDoubleInterpolator() :
		MakeUnique<TPopValueInterpolator<double>>();
	PullInterpolator = CollisionPushNode->PullInterpolator ?
		CollisionPushNode->PullInterpolator->BuildDoubleInterpolator() :
		MakeUnique<TPopValueInterpolator<double>>();
}

void FCollisionPushCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (!ensure(Params.EvaluationContext))
	{
		return;
	}

	UWorld* World = Params.EvaluationContext->GetWorld();
	APlayerController* PlayerController = Params.EvaluationContext->GetPlayerController();
	if (!World || !PlayerController)
	{
		return;
	}

	TOptional<FVector3d> SafePosition = GetSafePosition(Params, OutResult);
	if (!SafePosition.IsSet())
	{
		const UCameraNode* ThisNode = GetCameraNode();
		UE_LOG(LogCameraSystem, Error, 
				TEXT("Can't find safe position for CollisionPushCameraNode '%s'."),
				*GetNameSafe(ThisNode));
		return;
	}

	if (Params.EvaluationType != ECameraNodeEvaluationType::Standard)
	{
		// Don't run collision traces during IK/stateless updates.
		// Push the camera by the same amount as last time we updated properly, if possible.
		if (SafePosition.IsSet())
		{
			const FVector3d CameraPoseLocation = OutResult.CameraPose.GetLocation();
			const FVector3d PushedLocation = CameraPoseLocation + (SafePosition.GetValue() - CameraPoseLocation) * LastDampedPushFactor;
			OutResult.CameraPose.SetLocation(PushedLocation);
		}
		return;
	}

	HandleAsyncCollisionTraceResult(World, SafePosition.GetValue(), Params, OutResult);
	RunCollisionTrace(World, PlayerController, SafePosition.GetValue(), Params, OutResult);
}

TOptional<FVector3d> FCollisionPushCameraNodeEvaluator::GetSafePosition(const FCameraNodeEvaluationParams& Params, const FCameraNodeEvaluationResult& OutResult)
{
	FTransform3d SafePositionTransform;
	bool bGotSafePosition = false;

	// See if we can use the camera rig pivot as the safe position.
	// Otherwise, use the context's initial position.
	const FBuiltInCameraVariables& BuiltInVariables = FBuiltInCameraVariables::Get();
	TArrayView<const FCameraRigJoint> Joints = OutResult.CameraRigJoints.GetJoints();
	const FCameraRigJoint* PivotJoint = Joints.FindByPredicate([&BuiltInVariables](const FCameraRigJoint& Joint)
			{
				return Joint.VariableID == BuiltInVariables.YawPitchDefinition;
			});
	if (PivotJoint)
	{
		SafePositionTransform = PivotJoint->Transform;
		bGotSafePosition = true;
	}
	else if (Params.EvaluationContext)
	{
		const FCameraNodeEvaluationResult& InitialResult = Params.EvaluationContext->GetInitialResult();
		if (InitialResult.bIsValid)
		{
			SafePositionTransform = InitialResult.CameraPose.GetTransform();
			bGotSafePosition = true;
		}
	}

	// Apply the offset in the specified space.
	if (bGotSafePosition)
	{
		const UCollisionPushCameraNode* ThisNode = GetCameraNodeAs<UCollisionPushCameraNode>();
		const FVector3d SafePositionOffset = SafePositionOffsetReader.Get(OutResult.VariableTable);

		FVector3d WorldSafePositionOffset = SafePositionOffset;
		switch (ThisNode->SafePositionOffsetSpace)
		{
			case ECollisionSafePositionOffsetSpace::ActiveContext:
				if (Params.Evaluator)
				{
					const FCameraEvaluationContextStack& ContextStack = Params.Evaluator->GetEvaluationContextStack();
					TSharedPtr<const FCameraEvaluationContext> ActiveContext = ContextStack.GetActiveContext();
					if (ActiveContext)
					{
						const FCameraNodeEvaluationResult& InitialResult = Params.EvaluationContext->GetInitialResult();
						ensure(InitialResult.bIsValid);
						WorldSafePositionOffset = InitialResult.CameraPose.GetRotation().RotateVector(SafePositionOffset);
					}
					else
					{
						UE_LOG(LogCameraSystem, Warning, 
								TEXT("Can't offset safe collision position in active context space because "
									"no active context is active on the camera system evaluator for node '%s'!"),
								*GetNameSafe(ThisNode));
					}
				}
				else
				{
					UE_LOG(LogCameraSystem, Warning, 
							TEXT("Can't offset safe collision position in active context space because "
								"no camera system evaluator was given for node '%s'!"),
							*GetNameSafe(ThisNode));
				}
			case ECollisionSafePositionOffsetSpace::OwningContext:
				if (Params.EvaluationContext)
				{
					const FCameraNodeEvaluationResult& InitialResult = Params.EvaluationContext->GetInitialResult();
					ensure(InitialResult.bIsValid);
					WorldSafePositionOffset = InitialResult.CameraPose.GetRotation().RotateVector(SafePositionOffset);
				}
				else
				{
					UE_LOG(LogCameraSystem, Warning, 
							TEXT("Can't offset safe collision position in owning context space because "
								"there is no owning context for node '%s'!"),
							*GetNameSafe(ThisNode));
				}
				break;
			case ECollisionSafePositionOffsetSpace::Pivot:
				if (PivotJoint)
				{
					WorldSafePositionOffset = PivotJoint->Transform.TransformVectorNoScale(SafePositionOffset);
				}
				break;
			case ECollisionSafePositionOffsetSpace::CameraPose:
				{
					WorldSafePositionOffset = OutResult.CameraPose.GetRotation().RotateVector(SafePositionOffset);
				}
				break;
		}

		return SafePositionTransform.GetLocation() + WorldSafePositionOffset;
	}
	return TOptional<FVector3d>();
}

void FCollisionPushCameraNodeEvaluator::RunCollisionTrace(UWorld* World, APlayerController* PlayerController, const FVector3d& SafePosition, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	static FName CollisionTraceTag(TEXT("CameraCollision"));
	static FName CollisionTraceOwnerTag(TEXT("CollisionPushCameraNode"));

	const UCollisionPushCameraNode* CollisionPushNode = GetCameraNodeAs<UCollisionPushCameraNode>();
	ECollisionChannel CollisionChannel = CollisionPushNode->CollisionChannel;

	const float CollisionSphereRadius = CollisionSphereRadiusReader.Get(OutResult.VariableTable);
	const FVector3d SafePositionOffset = SafePositionOffsetReader.Get(OutResult.VariableTable);

	const FVector3d TraceStart(SafePosition);
	const FVector3d TraceEnd(OutResult.CameraPose.GetLocation());

	double TraceLength = FVector3d::Distance(TraceStart, TraceEnd);
	if (!ensure(TraceLength > 0))
	{
		return;
	}

	FCollisionShape SweepShape = FCollisionShape::MakeSphere(CollisionSphereRadius);
	// Ignore the player pawn by default.
	APawn* Pawn = PlayerController->GetPawn();
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(StartCollisionSweep), false, Pawn);
	QueryParams.TraceTag = CollisionTraceTag;
	QueryParams.OwnerTag = CollisionTraceOwnerTag;

	if (CollisionPushNode->bRunAsyncCollision)
	{
		CollisionTraceHandle = World->AsyncSweepByChannel(
			EAsyncTraceType::Single,
			TraceStart, TraceEnd, FQuat::Identity,
			CollisionChannel,
			SweepShape,
			QueryParams,
			FCollisionResponseParams::DefaultResponseParam);
	}
	else
	{
		TArray<FHitResult> HitResults;
		World->SweepMultiByChannel(
			HitResults,
			TraceStart, TraceEnd, FQuat::Identity,
			CollisionChannel,
			SweepShape,
			QueryParams,
			FCollisionResponseParams::DefaultResponseParam);
		HandleCollisionTraceResult(World, HitResults, SafePosition, Params, OutResult);
	}
}

void FCollisionPushCameraNodeEvaluator::HandleAsyncCollisionTraceResult(UWorld* World, const FVector3d& SafePosition, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UCollisionPushCameraNode* CollisionPushNode = GetCameraNodeAs<UCollisionPushCameraNode>();
	if (!CollisionPushNode->bRunAsyncCollision)
	{
		return;
	}

	if (!CollisionTraceHandle.IsValid())
	{
		return;
	}

	FTraceDatum TraceDatum;
	if (!World->QueryTraceData(CollisionTraceHandle, TraceDatum))
	{
		return;
	}

	HandleCollisionTraceResult(World, TraceDatum.OutHits, SafePosition, Params, OutResult);
}

void FCollisionPushCameraNodeEvaluator::HandleCollisionTraceResult(UWorld* World, TArrayView<const FHitResult> HitResults, const FVector3d& SafePosition, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Find any relevant hit in the trace results.
	bool bFoundHit = false;
	float CurrentPushFactor = 0.f;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	DebugHitObjectName.Empty();
	DebugSafePosition = SafePosition;
#endif

	for (const FHitResult& Hit : HitResults)
	{
		if (!Hit.bBlockingHit)
		{
			continue;
		}

		double TraceLength = FVector3d::Distance(Hit.TraceStart, Hit.TraceEnd);
		double DistanceToHit = FVector3d::Distance(Hit.TraceEnd, Hit.Location);
		if (ensure(TraceLength > 0))
		{
			CurrentPushFactor = (float)(DistanceToHit / TraceLength);
			bFoundHit = true;
#if UE_GAMEPLAY_CAMERAS_DEBUG
			if (UObject* HitPhysicsObjectOwner = Hit.PhysicsObjectOwner.Get())
			{
				DebugHitObjectName = GetNameSafe(HitPhysicsObjectOwner);
			}
			else
			{
				DebugHitObjectName = TEXT("<no physics object owner>");
			}
#endif
			break;
		}
	}

#if UE_GAMEPLAY_CAMERAS_DEBUG
	bDebugFoundHit = bFoundHit;
#endif

	// If we weren't pushed last frame, and we have no reason to push this frame either, then
	// we can bail out early.
	if (!bFoundHit && LastDampedPushFactor < UE_SMALL_NUMBER)
	{
		LastDirection = ECameraCollisionDirection::Pushing;
		LastPushFactor = 0;
		LastDampedPushFactor = 0;
		return;
	}

	// Figure out if we're pulling or pushing towards the safe position.
	// If we had no hit, the current push factor is zero.
	ECameraCollisionDirection CurrentDirection = LastDirection;
	if (LastPushFactor < CurrentPushFactor)
	{
		CurrentDirection = ECameraCollisionDirection::Pushing;
	}
	else if (LastPushFactor > CurrentPushFactor)
	{
		CurrentDirection = ECameraCollisionDirection::Pulling;
	}
	// else, push factor hasn't changed, keep the same direction as before.

	// Interpolate the push factor to make camera movements smoother.
	float CurrentDampedPushFactor = LastDampedPushFactor;

	FCameraValueInterpolationParams InterpParams;
	InterpParams.bIsCameraCut = Params.bIsFirstFrame;
	InterpParams.DeltaTime = Params.DeltaTime;
	FCameraValueInterpolationResult InterpResult(OutResult.VariableTable);
	switch (CurrentDirection)
	{
		case ECameraCollisionDirection::Pushing:
			PushInterpolator->Reset(LastDampedPushFactor, CurrentPushFactor);
			CurrentDampedPushFactor = PushInterpolator->Run(InterpParams, InterpResult);
			break;
		case ECameraCollisionDirection::Pulling:
			PullInterpolator->Reset(LastDampedPushFactor, CurrentPushFactor);
			CurrentDampedPushFactor = PullInterpolator->Run(InterpParams, InterpResult);
			break;
	}

	// Push the camera!
	if (CurrentDampedPushFactor > 0)
	{
		const FVector3d CameraPoseLocation = OutResult.CameraPose.GetLocation();
		const FVector3d PushedLocation = CameraPoseLocation + (SafePosition - CameraPoseLocation) * CurrentDampedPushFactor;
		OutResult.CameraPose.SetLocation(PushedLocation);
	}

	LastPushFactor = CurrentPushFactor;
	LastDampedPushFactor = CurrentDampedPushFactor;
	LastDirection = CurrentDirection;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCollisionPushCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FCollisionPushCameraDebugBlock& DebugBlock = Builder.AttachDebugBlock<FCollisionPushCameraDebugBlock>();

	DebugBlock.bFoundHit = bDebugFoundHit;
	DebugBlock.HitObjectName = DebugHitObjectName;
	DebugBlock.SafePosition = DebugSafePosition;
	DebugBlock.bIsPulling = (LastDirection == ECameraCollisionDirection::Pulling);
	DebugBlock.PushFactor = LastPushFactor;
	DebugBlock.DampedPushFactor = LastDampedPushFactor;
}

void FCollisionPushCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	if (DampedPushFactor > 0)
	{
		Renderer.AddText(TEXT("need to push by %.2f%%, currently %.2f%% [%s]"),
				PushFactor, DampedPushFactor,
				bIsPulling ? TEXT("pulling") : TEXT("pushing"));
		if (bFoundHit)
		{
			Renderer.AddText(TEXT(" (colliding with '%s')"), *HitObjectName);
		}
	}
	else
	{
		Renderer.AddText(TEXT("not pushing"));
	}

	Renderer.DrawText(SafePosition, TEXT("Safe Position"), FLinearColor::Gray, GEngine->GetTinyFont());
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

UCollisionPushCameraNode::UCollisionPushCameraNode(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	CollisionSphereRadius.Value = 10.f;
}

FCameraNodeEvaluatorPtr UCollisionPushCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCollisionPushCameraNodeEvaluator>();
}

