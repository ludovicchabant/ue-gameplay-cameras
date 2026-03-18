// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Shakes/CameraAnimationSequenceShakeCameraNode.h"

#include "CameraAnimationSequence.h"
#include "CameraAnimationSequencePlayer.h"
//#include "CameraAnimationSequenceState.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraParameterReader.h"
#include "Core/CameraSystemEvaluator.h"
#include "Services/CameraAnimationSequenceService.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraAnimationSequenceShakeCameraNode)

struct FCameraAnimationSequenceState;

namespace UE::Cameras
{

class FCameraAnimationSequenceService;

class FCameraAnimationSequenceShakeCameraNodeEvaluator : public FShakeCameraNodeEvaluator
{
	UE_DECLARE_SHAKE_CAMERA_NODE_EVALUATOR(, FCameraAnimationSequenceShakeCameraNodeEvaluator)

protected:

	// FShakeCameraNodeEvaluator interface.
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnShakeResult(const FCameraNodeShakeParams& Params, FCameraNodeShakeResult& OutResult) override;
	virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) override;
	virtual void OnTeardown(const FCameraNodeEvaluatorTeardownParams& Params) override;

private:

	TSharedPtr<FCameraAnimationSequenceService> AnimationService;
	TSharedPtr<FCameraAnimationSequenceState> AnimationState;
	TObjectPtr<UCameraAnimationSequenceCameraStandIn> CameraStandIn;

	TCameraParameterReader<float> PlayRateReader;
	TCameraParameterReader<float> ScaleReader;
	TCameraParameterReader<float> BlendInTimeReader;
	TCameraParameterReader<float> BlendOutTimeReader;

	float AnimationStartTime = 0.f;
	float AnimationEndTime = 0.f;
	float CurrentWeight = 0.f;
	float CurrentTimeLeft = 0.f;

	FMinimalViewInfo OriginalStandInViewInfo;
};

UE_DEFINE_SHAKE_CAMERA_NODE_EVALUATOR(FCameraAnimationSequenceShakeCameraNodeEvaluator)

void FCameraAnimationSequenceShakeCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	SetNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::NeedsSerialize);

	const UCameraAnimationSequenceShakeCameraNode* AnimationNode = GetCameraNodeAs<UCameraAnimationSequenceShakeCameraNode>();

	PlayRateReader.Initialize(AnimationNode->PlayRate);
	ScaleReader.Initialize(AnimationNode->Scale);
	BlendInTimeReader.Initialize(AnimationNode->BlendInTime);
	BlendOutTimeReader.Initialize(AnimationNode->BlendOutTime);

	UCameraAnimationSequence* Animation = AnimationNode->Animation;
	/*if (Animation)
	{
		// Cache the animation service.
		AnimationService = Params.Evaluator->FindOrRegisterEvaluationService<FCameraAnimationSequenceService>();
		UObject* ContextOwner = Params.EvaluationContext->GetOwner();
		if (!ContextOwner)
		{
			ContextOwner = GetTransientPackage();
		}

		// Create the camera stand-in object and save its initial state.
		CameraStandIn = NewObject<UCameraAnimationSequenceCameraStandIn>(ContextOwner, NAME_None, RF_Transient);
		CameraStandIn->Initialize(Animation);

		OriginalStandInViewInfo.Location = CameraStandIn->GetTransform().GetLocation();
		OriginalStandInViewInfo.Rotation = CameraStandIn->GetTransform().GetRotation().Rotator();
		OriginalStandInViewInfo.FOV = CameraStandIn->FieldOfView;
		OriginalStandInViewInfo.AspectRatio = CameraStandIn->AspectRatio;
		OriginalStandInViewInfo.PostProcessSettings = CameraStandIn->PostProcessSettings;
		OriginalStandInViewInfo.PostProcessBlendWeight = CameraStandIn->PostProcessBlendWeight;

		// Initialize the animation player.
		AnimationState = MakeShared<FCameraAnimationSequenceState>();
		AnimationState->Initialize(AnimationService->GetLinker(), Animation, CameraStandIn, ContextOwner);
		AnimationState->SetPlayRate(PlayRateReader.Get(OutResult.VariableTable));
		AnimationState->Play();

		// Make sure the animation player is ticked every frame with the rest.
		AnimationService->AddAnimation(AnimationState.ToSharedRef());

		// Cache some useful information about the animation.
		const FMovieScenePlaybackManager& PlaybackManager = AnimationState->GetPlaybackManager();
		const FFrameRate DisplayRate = PlaybackManager.GetDisplayRate();
		AnimationStartTime = DisplayRate.AsSeconds(PlaybackManager.GetEffectiveStartTime());
		AnimationEndTime = DisplayRate.AsSeconds(PlaybackManager.GetEffectiveEndTime());
	}*/
}

void FCameraAnimationSequenceShakeCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	/*if (AnimationState)
	{
		// Compute the elapsed time of the animation.
		const FMovieScenePlaybackManager& PlaybackManager = AnimationState->GetPlaybackManager();
		const FFrameRate DisplayRate = PlaybackManager.GetDisplayRate();
		const float CurrentTime = DisplayRate.AsSeconds(PlaybackManager.GetCurrentTime());

		// Compute blend-in and blend-out weights.
		float BlendInWeight = 1.f;
		const float BlendInTime = BlendInTimeReader.Get(OutResult.VariableTable);
		if (BlendInTime > 0.f && (CurrentTime - AnimationStartTime) < BlendInTime)
		{
			BlendInWeight = (CurrentTime - AnimationStartTime) / BlendInTime;
		}

		float BlendOutWeight = 1.f;
		const float BlendOutTime = BlendOutTimeReader.Get(OutResult.VariableTable);
		if (BlendOutTime > 0.f && (AnimationEndTime - CurrentTime) < BlendOutTime)
		{
			BlendOutWeight = (AnimationEndTime - CurrentTime) / BlendOutTime;
		}

		// Cache the final computed blend weight.
		CurrentWeight = FMath::Min(BlendInWeight, BlendOutWeight);
		CurrentTimeLeft = AnimationEndTime - CurrentTime;
	}*/
}

void FCameraAnimationSequenceShakeCameraNodeEvaluator::OnShakeResult(const FCameraNodeShakeParams& Params, FCameraNodeShakeResult& OutResult)
{
	using namespace UE::MovieScene;

	if (!AnimationService || !CameraStandIn || !AnimationState)
	{
		return;
	}

	if (CurrentWeight > 0)
	{
		FCameraPose& ShakenCameraPose = OutResult.ShakenResult.CameraPose;
		const FCameraVariableTable& VariableTable = OutResult.ShakenResult.VariableTable;
		const float CurrentScale = ScaleReader.Get(VariableTable);

		// Make sure derived data like FOV is up to date.
		CameraStandIn->RecalcDerivedData();

		// Treat transform and FOV as additive.
		FCameraNodeShakeDelta ShakeDelta;
		const FTransform ShakenTransform = CameraStandIn->GetTransform();
		ShakeDelta.Location = ShakenTransform.GetLocation();
		ShakeDelta.Rotation = ShakenTransform.GetRotation().Rotator();

		const float DeltaFieldOfView = (CameraStandIn->FieldOfView - OriginalStandInViewInfo.FOV);
		ShakeDelta.FieldOfView = DeltaFieldOfView;

		// The other properties aren't treated as additive.
		if (CameraStandIn->PostProcessBlendWeight > 0)
		{
			OutResult.ShakenResult.PostProcessSettings.LerpAll(
					CameraStandIn->PostProcessSettings, CameraStandIn->PostProcessBlendWeight);
		}

		OutResult.ShakeDelta.Combine(ShakeDelta, CurrentScale);
	}

	OutResult.ShakeTimeLeft = CurrentTimeLeft;
}

void FCameraAnimationSequenceShakeCameraNodeEvaluator::OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar)
{
	Super::OnSerialize(Params, Ar);
}

void FCameraAnimationSequenceShakeCameraNodeEvaluator::OnTeardown(const FCameraNodeEvaluatorTeardownParams& Params)
{
	/*if (AnimationState && AnimationService)
	{
		AnimationService->RemoveAnimation(AnimationState.ToSharedRef());
	}
	if (AnimationState)
	{
		AnimationState->TearDown();
	}

	AnimationService.Reset();
	AnimationState.Reset();*/
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UCameraAnimationSequenceShakeCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCameraAnimationSequenceShakeCameraNodeEvaluator>();
}

