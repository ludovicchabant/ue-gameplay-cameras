// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCamerasPlayerCameraManager.h"

#include "Camera/CameraComponent.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraEvaluationService.h"
#include "Core/CameraRigTransition.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNodeCameraRigEvent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/ActorCameraEvaluationContext.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/GameplayCameraRigComponent.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "GameFramework/PlayerController.h"
#include "Services/CameraModifierService.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

class FViewTargetTransitionParamsBlendCameraNodeEvaluator : public FSimpleBlendCameraNodeEvaluator
{
	UE_DECLARE_BLEND_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FViewTargetTransitionParamsBlendCameraNodeEvaluator, FSimpleBlendCameraNodeEvaluator)

protected:

	// FSimpleBlendCameraNodeEvaluator interface.
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnComputeBlendFactor(const FCameraNodeEvaluationParams& Params, FSimpleBlendCameraNodeEvaluationResult& OutResult) override;

private:

	float CurrentTime = 0.f;
};

void FViewTargetTransitionParamsBlendCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UViewTargetTransitionParamsBlendCameraNode* TransitionParamsNode = GetCameraNodeAs<UViewTargetTransitionParamsBlendCameraNode>();
	CurrentTime += Params.DeltaTime;
	if (CurrentTime >= TransitionParamsNode->TransitionParams.BlendTime)
	{
		CurrentTime = TransitionParamsNode->TransitionParams.BlendTime;
		SetBlendFinished();
	}

	FSimpleBlendCameraNodeEvaluator::OnRun(Params, OutResult);
}

void FViewTargetTransitionParamsBlendCameraNodeEvaluator::OnComputeBlendFactor(const FCameraNodeEvaluationParams& Params, FSimpleBlendCameraNodeEvaluationResult& OutResult)
{
	const UViewTargetTransitionParamsBlendCameraNode* TransitionParamsNode = GetCameraNodeAs<UViewTargetTransitionParamsBlendCameraNode>();
	float TimeFactor = 1.f;
	if (TransitionParamsNode->TransitionParams.BlendTime > 0.f)
	{
		TimeFactor = CurrentTime / TransitionParamsNode->TransitionParams.BlendTime;
	}
	OutResult.BlendFactor = TransitionParamsNode->TransitionParams.GetBlendAlpha(TimeFactor);
}

UE_DEFINE_BLEND_CAMERA_NODE_EVALUATOR(FViewTargetTransitionParamsBlendCameraNodeEvaluator)

class FViewTargetContextReferencerService : public FCameraEvaluationService
{
public:

	FViewTargetContextReferencerService()
	{
		SetEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsRootCameraNodeEvents);
	}

	void AddViewTargetContext(AActor* InViewTarget, TSharedRef<FCameraEvaluationContext> InContext)
	{
		Entries.Add(FEntry{ InViewTarget, InContext });
	}

protected:

	virtual void OnInitialize(const FCameraEvaluationServiceInitializeParams& Params) override
	{
		Evaluator = Params.Evaluator;
	}

	virtual void OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent) override
	{
		if (InEvent.EventType == ERootCameraNodeCameraRigEventType::Deactivated)
		{
			TSharedPtr<const FCameraEvaluationContext> Context = InEvent.CameraRigInfo.EvaluationContext;
			if (Context)
			{
				for (auto It = Entries.CreateIterator(); It; ++It)
				{
					if (It->Context == Context)
					{
						It.RemoveCurrent();
					}
				}
			}
		}
	}

private:

	struct FEntry
	{
		TWeakObjectPtr<AActor> WeakViewTarget;
		TSharedPtr<FCameraEvaluationContext> Context;
	};

	FCameraSystemEvaluator* Evaluator = nullptr;
	TArray<FEntry> Entries;
};

}  // namespace UE::Cameras

AGameplayCamerasPlayerCameraManager::AGameplayCamerasPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AGameplayCamerasPlayerCameraManager::StealPlayerController(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (!ensure(PlayerController->PlayerCameraManager != this))
	{
		return;
	}

	OriginalCameraManager = PlayerController->PlayerCameraManager;
	AActor* OriginalViewTarget = OriginalCameraManager->GetViewTarget();

	PlayerController->PlayerCameraManager = this;
	InitializeFor(PlayerController);

	if (WeakOriginalAutoCameraSystemActor != OriginalViewTarget)
	{
		// We had some arbitrary view target before. Let's keep it.
		SetViewTarget(OriginalViewTarget);
	}
	else
	{
		// The original view target was our camera system, so nothing changes. We will restore 
		// that actor as the view target on the original camera manager when we're done.
		//
		// Let's set the view target to whatever owns the active context, or fall back to the
		// player controller if the active context owner isn't an actor. We need to avoid using
		// SetViewTarget otherwise that would create another evaluation context. Instead we
		// call AssignViewTarget directly.
		TSharedPtr<FCameraEvaluationContext> ActiveContext = CameraSystemEvaluator->GetEvaluationContextStack().GetActiveContext();
		if (ActiveContext && ActiveContext->GetOwner())
		{
			AActor* ActiveViewTarget = ActiveContext->GetOwner()->GetTypedOuter<AActor>();
			AssignViewTarget(ActiveViewTarget, ViewTarget);
		}
		ViewTarget.CheckViewTarget(PlayerController);
	}
}

void AGameplayCamerasPlayerCameraManager::ReleasePlayerController()
{
	if (!ensure(PCOwner && PCOwner->PlayerCameraManager == this))
	{
		return;
	}

	PCOwner->PlayerCameraManager = OriginalCameraManager;
	if (AGameplayCameraSystemActor* OriginalAutoCameraSystemActor = WeakOriginalAutoCameraSystemActor.Get())
	{
		OriginalCameraManager->SetViewTarget(OriginalAutoCameraSystemActor);
	}

	ViewTarget.Target = nullptr;

	OriginalCameraManager = nullptr;
	WeakOriginalAutoCameraSystemActor = nullptr;

	ReleaseCameraSystemHost();

	PCOwner = nullptr;
}

FCameraRigInstanceID AGameplayCamerasPlayerCameraManager::StartGlobalCameraModifierRig(const UCameraRigAsset* CameraRig, int32 OrderKey)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator)
	{
		TSharedPtr<FCameraModifierService> CameraModifierService = CameraSystemEvaluator->FindEvaluationService<FCameraModifierService>();
		return CameraModifierService->StartCameraModifierRig(CameraRig, ECameraRigLayer::Global, OrderKey);
	}

	return FCameraRigInstanceID();
}

FCameraRigInstanceID AGameplayCamerasPlayerCameraManager::StartVisualCameraModifierRig(const UCameraRigAsset* CameraRig, int32 OrderKey)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator)
	{
		TSharedPtr<FCameraModifierService> CameraModifierService = CameraSystemEvaluator->FindEvaluationService<FCameraModifierService>();
		return CameraModifierService->StartCameraModifierRig(CameraRig, ECameraRigLayer::Visual, OrderKey);
	}

	return FCameraRigInstanceID();
}

void AGameplayCamerasPlayerCameraManager::StopCameraModifierRig(FCameraRigInstanceID InstanceID, bool bImmediately)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator)
	{
		TSharedPtr<FCameraModifierService> CameraModifierService = CameraSystemEvaluator->FindEvaluationService<FCameraModifierService>();
		return CameraModifierService->StopCameraModifierRig(InstanceID, bImmediately);
	}
}

void AGameplayCamerasPlayerCameraManager::InitializeFor(APlayerController* PlayerController)
{
	AcquireCameraSystemHost(PlayerController);
	Super::InitializeFor(PlayerController);
}

void AGameplayCamerasPlayerCameraManager::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	using namespace UE::Cameras;

	// We want to keep our view target in sync with whatever is the active context owner in the camera system.
	// If that context owner isn't an actor, and isn't inside an actor (like a component), we use the player
	// controller as the view target.

	if (NewViewTarget == nullptr)
	{
		FCameraEvaluationContextStack& ContextStack = CameraSystemEvaluator->GetEvaluationContextStack();
		ContextStack.PopContext();

		if (TSharedPtr<FCameraEvaluationContext> NewActiveContext = ContextStack.GetActiveContext())
		{
			if (UObject* NewActiveContextOwner = NewActiveContext->GetOwner())
			{
				NewViewTarget = Cast<AActor>(NewActiveContextOwner);
				if (!NewViewTarget)
				{
					NewViewTarget = NewActiveContextOwner->GetTypedOuter<AActor>();
				}
			}
		}
	}

	// We pass empty transition params here because we never want to use PendingViewTarget, just ViewTarget.
	Super::SetViewTarget(NewViewTarget, FViewTargetTransitionParams());

	if (!NewViewTarget)
	{
		return;
	}

	if (UGameplayCameraComponent* GameplayCameraComponent = NewViewTarget->FindComponentByClass<UGameplayCameraComponent>())
	{
		GameplayCameraComponent->ActivateCameraForPlayerController(PCOwner);
	}
	else if (UGameplayCameraRigComponent* GameplayCameraRigComponent = NewViewTarget->FindComponentByClass<UGameplayCameraRigComponent>())
	{
		GameplayCameraRigComponent->ActivateCameraForPlayerController(PCOwner);
	}
	else if (UCameraComponent* CameraComponent = NewViewTarget->FindComponentByClass<UCameraComponent>())
	{
		TSharedRef<FActorCameraEvaluationContext> NewContext = MakeShared<FActorCameraEvaluationContext>(CameraComponent);
		CameraSystemEvaluator->PushEvaluationContext(NewContext);
	}
	else
	{
		TSharedRef<FActorCameraEvaluationContext> NewContext = MakeShared<FActorCameraEvaluationContext>(NewViewTarget);
		CameraSystemEvaluator->PushEvaluationContext(NewContext);
	}

	// If transition parameters were given, override the next activation for the new evaluation context.
	TSharedPtr<FCameraEvaluationContext> NextContext = CameraSystemEvaluator->GetEvaluationContextStack().GetActiveContext();
	if (NextContext)
	{
		ViewTargetContextReferencerService->AddViewTargetContext(NewViewTarget, NextContext.ToSharedRef());

		if (TransitionParams.BlendTime > 0.f)
		{
			UViewTargetTransitionParamsBlendCameraNode* BlendNode = NewObject<UViewTargetTransitionParamsBlendCameraNode>(GetTransientPackage());
			BlendNode->TransitionParams = TransitionParams;

			UCameraRigTransition* Transition = NewObject<UCameraRigTransition>(GetTransientPackage());
			Transition->Blend = BlendNode;

			FCameraDirectorEvaluator* DirectorEvaluator = NextContext->GetDirectorEvaluator();
			DirectorEvaluator->OverrideNextActivationTransition(Transition);
		}
	}
}

void AGameplayCamerasPlayerCameraManager::AcquireCameraSystemHost(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	// Early-out if we already have acquired the host.
	if (CameraSystemHost)
	{
		return;
	}

	// Find or create a camera system host and grab its camera system.
	CameraSystemHost = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
	ensure(CameraSystemHost);
	CameraSystemEvaluator = CameraSystemHost->GetCameraSystemEvaluator();

	ViewTargetContextReferencerService = MakeShared<FViewTargetContextReferencerService>();
	CameraSystemEvaluator->RegisterEvaluationService(ViewTargetContextReferencerService.ToSharedRef());

	// See if there was an auto-spawned camera system actor. If so, remember to set it back.
	WeakOriginalAutoCameraSystemActor = AGameplayCameraSystemActor::GetAutoSpawnedCameraSystemActor(PlayerController, false);
}

void AGameplayCamerasPlayerCameraManager::ReleaseCameraSystemHost()
{
	if (ViewTargetContextReferencerService)
	{
		CameraSystemEvaluator->UnregisterEvaluationService(ViewTargetContextReferencerService.ToSharedRef());
	}

	CameraSystemHost = nullptr;
	CameraSystemEvaluator = nullptr;
}

void AGameplayCamerasPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}

void AGameplayCamerasPlayerCameraManager::DoUpdateCamera(float DeltaTime)
{
	using namespace UE::Cameras;

	Super::DoUpdateCamera(DeltaTime);

	if (CameraSystemEvaluator.IsValid())
	{
		FillCameraCache(LastFrameDesiredView);

		FCameraSystemEvaluationParams UpdateParams;
		UpdateParams.DeltaTime = DeltaTime;
		CameraSystemEvaluator->Update(UpdateParams);

		FMinimalViewInfo DesiredView;
		CameraSystemEvaluator->GetEvaluatedCameraView(DesiredView);

		FillCameraCache(DesiredView);

		LastFrameDesiredView = DesiredView;
	}
}

void AGameplayCamerasPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameplayCamerasPlayerCameraManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// TODO: destroy camera system actor?

	Super::EndPlay(EndPlayReason);
}

void AGameplayCamerasPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	int Indentation = 1;
	int LineNumber = FMath::CeilToInt(YPos / YL);

	UFont* DrawFont = GEngine->GetSmallFont();
	Canvas->SetDrawColor(FColor::Yellow);
	Canvas->DrawText(
			DrawFont, 
			FString::Printf(TEXT("Please use the Camera Debugger panel to inspect '%s'."), *GetNameSafe(this)),
			Indentation * YL, (LineNumber++) * YL);

	YPos = LineNumber * YL;

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
}

FCameraNodeEvaluatorPtr UViewTargetTransitionParamsBlendCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FViewTargetTransitionParamsBlendCameraNodeEvaluator>();
}

