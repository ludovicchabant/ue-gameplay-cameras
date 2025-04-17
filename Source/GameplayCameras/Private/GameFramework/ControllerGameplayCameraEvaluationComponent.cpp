// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/ControllerGameplayCameraEvaluationComponent.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "GameFramework/IGameplayCameraSystemHost.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ControllerGameplayCameraEvaluationComponent)

UControllerGameplayCameraEvaluationComponent::UControllerGameplayCameraEvaluationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
}

void UControllerGameplayCameraEvaluationComponent::Initialize(TScriptInterface<IGameplayCameraSystemHost> InCameraSystemHost)
{
	ensure(InCameraSystemHost);
	CameraSystemHost = InCameraSystemHost;
}

void UControllerGameplayCameraEvaluationComponent::ActivateCameraRig(UCameraRigAsset* CameraRig, ECameraRigLayer EvaluationLayer)
{
	FCameraRigInfo NewCameraRigInfo;
	NewCameraRigInfo.CameraRig = CameraRig;
	NewCameraRigInfo.EvaluationLayer = EvaluationLayer;
	NewCameraRigInfo.bActivated = false;
	CameraRigInfos.Add(NewCameraRigInfo);

	if (IsActive())
	{
		ActivateCameraRigs();
	}
}

void UControllerGameplayCameraEvaluationComponent::BeginPlay()
{
	Super::BeginPlay();

	ActivateCameraRigs();
}

void UControllerGameplayCameraEvaluationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CameraRigInfos.Reset();
	EvaluationContext.Reset();

	Super::EndPlay(EndPlayReason);
}

void UControllerGameplayCameraEvaluationComponent::ActivateCameraRigs()
{
	using namespace UE::Cameras;

	if (!ensure(CameraSystemHost))
	{
		return;
	}
	
	EnsureEvaluationContext();
	if (!EvaluationContext)
	{
		return;
	}

	TSharedPtr<FCameraSystemEvaluator> SystemEvaluator = CameraSystemHost->GetCameraSystemEvaluator();
	FRootCameraNodeEvaluator* RootNodeEvaluator = SystemEvaluator->GetRootNodeEvaluator();

	for (FCameraRigInfo& CameraRigInfo : CameraRigInfos)
	{
		if (!CameraRigInfo.bActivated)
		{
			FActivateCameraRigParams Params;
			Params.CameraRig = CameraRigInfo.CameraRig;
			Params.EvaluationContext = EvaluationContext;
			Params.Layer = CameraRigInfo.EvaluationLayer;
			RootNodeEvaluator->ActivateCameraRig(Params);

			CameraRigInfo.bActivated = true;
		}
	}
}

void UControllerGameplayCameraEvaluationComponent::EnsureEvaluationContext()
{
	using namespace UE::Cameras;

	if (!EvaluationContext.IsValid())
	{
		// TODO: we won't find a player controller this way if we are under a view target.
		APlayerController* PlayerController = GetOwner<APlayerController>();

		FCameraEvaluationContextInitializeParams InitParams;
		InitParams.Owner = this;
		InitParams.PlayerController = PlayerController;
		EvaluationContext = MakeShared<FCameraEvaluationContext>(InitParams);
		EvaluationContext->GetInitialResult().bIsValid = true;	
	}
}

UControllerGameplayCameraEvaluationComponent* UControllerGameplayCameraEvaluationComponent::FindComponent(AActor* OwnerActor)
{
	return OwnerActor->FindComponentByClass<UControllerGameplayCameraEvaluationComponent>();
}

UControllerGameplayCameraEvaluationComponent* UControllerGameplayCameraEvaluationComponent::FindOrAddComponent(AActor* OwnerActor, bool* bOutCreated)
{
	UControllerGameplayCameraEvaluationComponent* ControllerComponent = FindComponent(OwnerActor);
	if (!ControllerComponent)
	{
		ControllerComponent = NewObject<UControllerGameplayCameraEvaluationComponent>(
				OwnerActor, TEXT("ControllerGameplayCameraEvaluationComponent"), RF_Transient);
		ControllerComponent->RegisterComponent();
		if (bOutCreated)
		{
			*bOutCreated = true;
		}
	}
	return ControllerComponent;
}

