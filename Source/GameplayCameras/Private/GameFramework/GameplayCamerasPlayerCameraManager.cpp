// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCamerasPlayerCameraManager.h"

#include "Camera/CameraComponent.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraEvaluationService.h"
#include "Core/CameraRigTransition.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "Core/RootCameraNodeCameraRigEvent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/ActorCameraEvaluationContext.h"
#include "GameFramework/GameplayCameraComponentBase.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCamerasSettings.h"
#include "Services/CameraModifierService.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

/**
 * A blend node evaluator for UViewTargetTransitionParamsBlendCameraNode, which emulates the basic engine
 * blend curves for view targets.
 */
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

}  // namespace UE::Cameras

AGameplayCamerasPlayerCameraManager::AGameplayCamerasPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AGameplayCamerasPlayerCameraManager::BeginDestroy()
{
	DestroyCameraSystem();

	Super::BeginDestroy();
}

void AGameplayCamerasPlayerCameraManager::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	AGameplayCamerasPlayerCameraManager* This = CastChecked<AGameplayCamerasPlayerCameraManager>(InThis);
	This->IGameplayCameraSystemHost::OnAddReferencedObjects(Collector);
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

	SetViewTarget(OriginalViewTarget);
}

void AGameplayCamerasPlayerCameraManager::ReleasePlayerController()
{
	if (!ensure(PCOwner && PCOwner->PlayerCameraManager == this))
	{
		return;
	}

	PCOwner->PlayerCameraManager = OriginalCameraManager;

	ViewTarget.Target = nullptr;

	OriginalCameraManager = nullptr;

	DestroyCameraSystem();

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
	using namespace UE::Cameras;

	if (!bOverrideViewRotationMode)
	{
		const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
		ViewRotationMode = Settings->DefaultViewRotationMode;
	}

	EnsureCameraSystemInitialized();
	if (ensure(CameraSystemEvaluator))
	{
		FCameraEvaluationContextStack& ContextStack = CameraSystemEvaluator->GetEvaluationContextStack();
		ContextStack.OnStackChanged().AddUObject(this, &AGameplayCamerasPlayerCameraManager::OnContextStackChanged);
	}

	Super::InitializeFor(PlayerController);
}

void AGameplayCamerasPlayerCameraManager::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	using namespace UE::Cameras;

	// We want to keep our view target in sync with whatever is the active context owner in the camera system.
	// If that context owner isn't an actor, and isn't inside an actor (like a component), we use the player
	// controller as the view target.

	ensure(bIsSettingNewViewTarget == false);
	TGuardValue<bool> ReentrancyGuard(bIsSettingNewViewTarget, true);
	FCameraEvaluationContextStack& ContextStack = CameraSystemEvaluator->GetEvaluationContextStack();

	// If the view target is null, this is sort of a shortcut for "we're done with the current view target",
	// so pop the context stack and reactivate the previous context.
	if (NewViewTarget == nullptr)
	{
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

	// See if we can find the view target in the context stack. If so, reactivate it instead of potentially
	// making a new context for the same thing.
	bool bFoundContext = false;
	TArray<TSharedPtr<FCameraEvaluationContext>> CurrentContexts;
	ContextStack.GetAllContexts(CurrentContexts);
	for (TSharedPtr<FCameraEvaluationContext> CurrentContext : CurrentContexts)
	{
		UObject* CurrentContextOwner = CurrentContext ? CurrentContext->GetOwner() : nullptr;
		if (CurrentContextOwner && 
				(CurrentContext->GetOwner() == NewViewTarget || 
				 CurrentContextOwner->GetTypedOuter<AActor>() == NewViewTarget))
		{
			// This will move the context to the top if it's already in the stack (which it is, we
			// found it there).
			ContextStack.PushContext(CurrentContext.ToSharedRef());
			bFoundContext = true;
		}
	}

	if (!bFoundContext)
	{
		if (UGameplayCameraComponentBase* GameplayCameraComponent = NewViewTarget->FindComponentByClass<UGameplayCameraComponentBase>())
		{
			GameplayCameraComponent->ActivateCameraForPlayerController(PCOwner);
		}
		else if (UCameraComponent* CameraComponent = NewViewTarget->FindComponentByClass<UCameraComponent>())
		{
			TSharedRef<FActorCameraEvaluationContext> NewContext = MakeShared<FActorCameraEvaluationContext>(CameraComponent);
			CameraSystemEvaluator->PushEvaluationContext(NewContext);
			ViewTargetContexts.Add(NewContext);
		}
		else
		{
			TSharedRef<FActorCameraEvaluationContext> NewContext = MakeShared<FActorCameraEvaluationContext>(NewViewTarget);
			CameraSystemEvaluator->PushEvaluationContext(NewContext);
			ViewTargetContexts.Add(NewContext);
		}
	}

	// If transition parameters were given, override the next activation for the new evaluation context.
	TSharedPtr<FCameraEvaluationContext> NextContext = CameraSystemEvaluator->GetEvaluationContextStack().GetActiveContext();
	if (NextContext && TransitionParams.BlendTime > 0.f)
	{
		UViewTargetTransitionParamsBlendCameraNode* BlendNode = NewObject<UViewTargetTransitionParamsBlendCameraNode>(GetTransientPackage());
		BlendNode->TransitionParams = TransitionParams;

		UCameraRigTransition* Transition = NewObject<UCameraRigTransition>(GetTransientPackage());
		Transition->Blend = BlendNode;

		FCameraDirectorEvaluator* DirectorEvaluator = NextContext->GetDirectorEvaluator();
		DirectorEvaluator->OverrideNextActivationTransition(Transition);
	}
}

void AGameplayCamerasPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	switch (ViewRotationMode)
	{
		case EGameplayCamerasViewRotationMode::PreviewUpdate:
			RunViewRotationPreviewUpdate(DeltaTime, OutViewRotation, OutDeltaRot);
			break;
	}

	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}

void AGameplayCamerasPlayerCameraManager::RunViewRotationPreviewUpdate(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	using namespace UE::Cameras;

	if (HasCameraSystem())
	{
		FCameraSystemEvaluationParams Params;
		Params.DeltaTime = DeltaTime;

		FCameraSystemViewRotationEvaluationResult Result;
		Result.ViewRotation = OutViewRotation;
		Result.DeltaRotation = OutDeltaRot;

		CameraSystemEvaluator->ViewRotationPreviewUpdate(Params, Result);

		OutViewRotation = Result.ViewRotation;
		OutDeltaRot = Result.DeltaRotation;
	}
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

		CleanUpViewTargetContexts();
	}
}

void AGameplayCamerasPlayerCameraManager::OnContextStackChanged()
{
	using namespace UE::Cameras;

	// When the context stack changes, such as when a gameplay camera component activates directly
	// against our camera system host, we want to update the view target so that it's always in sync
	// with whichever owns the active evaluation context.
	//
	// This is as opposed to going through SetViewTarget or some other APlayerCameraManager method.

	if (ensure(CameraSystemEvaluator) && !bIsSettingNewViewTarget)
	{
		TGuardValue<bool> ReentrancyGuard(bIsSettingNewViewTarget, true);

		FCameraEvaluationContextStack& ContextStack = CameraSystemEvaluator->GetEvaluationContextStack();
		TSharedPtr<FCameraEvaluationContext> ActiveContext = ContextStack.GetActiveContext();
		UObject* ActiveContextOwner = ActiveContext->GetOwner();
		if (ActiveContextOwner)
		{
			ViewTarget.SetNewTarget(ActiveContextOwner->GetTypedOuter<AActor>());
		}
		else
		{
			ViewTarget.SetNewTarget(nullptr);
		}

		ViewTarget.CheckViewTarget(PCOwner);
		BlendParams = FViewTargetTransitionParams();
	}
}

void AGameplayCamerasPlayerCameraManager::CleanUpViewTargetContexts()
{
	using namespace UE::Cameras;

	FRootCameraNodeEvaluator* RootEvaluator = CameraSystemEvaluator->GetRootNodeEvaluator();

	for (auto It = ViewTargetContexts.CreateIterator(); It; ++It)
	{
		TSharedRef<FCameraEvaluationContext> Context(*It);
		if (!RootEvaluator->HasAnyRunningCameraRig(Context))
		{
			It.RemoveCurrent();
		}
	}
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

