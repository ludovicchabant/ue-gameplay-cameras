// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/ActivateCameraRigFunctions.h"

#include "Camera/PlayerCameraManager.h"
#include "Core/CameraRigAsset.h"
#include "Core/RootCameraNode.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCameras.h"
#include "Templates/SharedPointer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActivateCameraRigFunctions)

void UActivateCameraRigFunctions::ActivateBaseCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRig(WorldContextObject, PlayerController, CameraRig, ECameraRigLayer::Base);
}

void UActivateCameraRigFunctions::ActivateGlobalCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRig(WorldContextObject, PlayerController, CameraRig, ECameraRigLayer::Global);
}

void UActivateCameraRigFunctions::ActivateVisualCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRig(WorldContextObject, PlayerController, CameraRig, ECameraRigLayer::Visual);
}

void UActivateCameraRigFunctions::ActivateCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig, ECameraRigLayer EvaluationLayer)
{
	using namespace UE::Cameras;

	if (!CameraRig)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No camera rig was given to activate!"));
		return;
	}

	// Register our evaluation component on the given player controller, if it's not there already.
	UControllerGameplayCameraEvaluationComponent* CameraEvaluationComponent = PlayerController->FindComponentByClass<UControllerGameplayCameraEvaluationComponent>();
	if (!CameraEvaluationComponent)
	{
		CameraEvaluationComponent = NewObject<UControllerGameplayCameraEvaluationComponent>(
				PlayerController, TEXT("ControllerGameplayCameraEvaluationComponent"), RF_Transient);
		CameraEvaluationComponent->RegisterComponent();
	}

	// Activate the camera rig.
	CameraEvaluationComponent->ActivateCameraRig(CameraRig, EvaluationLayer);
}

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

	APlayerController* PlayerController = GetOwner<APlayerController>();
	FCameraSystemEvaluator* SystemEvaluator = FindCameraSystemEvaluator(PlayerController);
	if (!SystemEvaluator)
	{
		return;
	}
	
	EnsureEvaluationContext();
	if (!EvaluationContext)
	{
		return;
	}

	FRootCameraNodeEvaluator* RootNodeEvaluator = SystemEvaluator->GetRootNodeEvaluator();

	for (FCameraRigInfo& CameraRigInfo : CameraRigInfos)
	{
		if (!CameraRigInfo.bActivated)
		{
			FActivateCameraRigParams Params;
			Params.CameraRig = CameraRigInfo.CameraRig;
			Params.EvaluationContext = EvaluationContext;
			Params.Evaluator = SystemEvaluator;
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

UE::Cameras::FCameraSystemEvaluator* UControllerGameplayCameraEvaluationComponent::FindCameraSystemEvaluator(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		AActor* ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget();
		if (AGameplayCameraSystemActor* SystemActor = Cast<AGameplayCameraSystemActor>(ViewTarget))
		{
			return SystemActor->GetCameraSystemComponent()->GetCameraSystemEvaluator().Get();
		}
	}
	return nullptr;
}

