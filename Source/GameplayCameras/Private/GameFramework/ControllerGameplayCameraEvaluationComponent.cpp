// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/ControllerGameplayCameraEvaluationComponent.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ControllerGameplayCameraEvaluationComponent)

UControllerGameplayCameraEvaluationComponent::UControllerGameplayCameraEvaluationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
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

	EnsureCameraSystemHost();
	if (!CameraSystemHost)
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
		APlayerController* PlayerController = GetOwner<APlayerController>();

		FCameraEvaluationContextInitializeParams InitParams;
		InitParams.Owner = this;
		InitParams.PlayerController = PlayerController;
		EvaluationContext = MakeShared<FCameraEvaluationContext>(InitParams);
		EvaluationContext->GetInitialResult().bIsValid = true;	
	}
}

void UControllerGameplayCameraEvaluationComponent::EnsureCameraSystemHost()
{
	if (!CameraSystemHost)
	{
		APlayerController* PlayerController = GetOwner<APlayerController>();

		CameraSystemHost = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
	}
}

UControllerGameplayCameraEvaluationComponent* UControllerGameplayCameraEvaluationComponent::FindComponent(APlayerController* PlayerController)
{
	return PlayerController->FindComponentByClass<UControllerGameplayCameraEvaluationComponent>();
}

UControllerGameplayCameraEvaluationComponent* UControllerGameplayCameraEvaluationComponent::FindOrAddComponent(APlayerController* PlayerController)
{
	UControllerGameplayCameraEvaluationComponent* ControllerComponent = FindComponent(PlayerController);
	if (!ControllerComponent)
	{
		ControllerComponent = NewObject<UControllerGameplayCameraEvaluationComponent>(
				PlayerController, TEXT("ControllerGameplayCameraEvaluationComponent"), RF_Transient);
		ControllerComponent->RegisterComponent();
	}
	return ControllerComponent;
}

TSharedPtr<UE::Cameras::FCameraEvaluationContext> UControllerGameplayCameraEvaluationComponent::FindEvaluationContext(APlayerController* PlayerController)
{
	if (UControllerGameplayCameraEvaluationComponent* ControllerComponent = FindComponent(PlayerController))
	{
		ControllerComponent->EnsureEvaluationContext();
		return ControllerComponent->EvaluationContext;
	}
	return nullptr;
}

TSharedRef<UE::Cameras::FCameraEvaluationContext> UControllerGameplayCameraEvaluationComponent::FindOrAddEvaluationContext(APlayerController* PlayerController)
{
	UControllerGameplayCameraEvaluationComponent* ControllerComponent = FindOrAddComponent(PlayerController);
	check(ControllerComponent);
	ControllerComponent->EnsureEvaluationContext();
	return ControllerComponent->EvaluationContext.ToSharedRef();
}

