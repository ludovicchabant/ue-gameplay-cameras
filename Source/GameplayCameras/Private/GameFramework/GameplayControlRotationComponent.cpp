// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayControlRotationComponent.h"

#include "Core/CameraEvaluationContext.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/Pawn.h"
#include "GameplayCameras.h"
#include "Kismet/GameplayStatics.h"
#include "Math/ColorList.h"
#include "Services/PlayerControlRotationService.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayControlRotationComponent)

#define LOCTEXT_NAMESPACE "GameplayControlRotationComponent"

UGameplayControlRotationComponent::UGameplayControlRotationComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UGameplayControlRotationComponent::BeginPlay()
{
	Super::BeginPlay();

	using namespace UE::Cameras;

	// We need to grab our owner actor, and then find the gameplay camera component and the enhanced
	// input component that we will be coordinating.
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogCameraSystem, Error, 
				TEXT("GameplayControlRotationComponent '%s' doesn't belong to any actor"),
				*GetNameSafe(this));
		return;
	}

	UGameplayCameraComponent* GameplayCameraComponent = OwnerActor->FindComponentByClass<UGameplayCameraComponent>();
	if (!GameplayCameraComponent)
	{
		UE_LOG(LogCameraSystem, Error,
				TEXT("GameplayControlRotationComponent '%s' couldn't find a GameplayCameraComponent on owner '%s'"),
				*GetNameSafe(this), *GetNameSafe(OwnerActor));
		return;
	}

	// Find where the camera system is running from.
	PlayerIndex = GameplayCameraComponent->GetPlayerIndex();
	if (!ensure(PlayerIndex != INDEX_NONE))
	{
		UE_LOG(LogCameraSystem, Error,
				TEXT("GameplayCameraComponent '%s' hasn't activated for any player yet"),
				*GetNameSafe(GameplayCameraComponent));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (!PlayerController)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't find player controller for player index %d"), PlayerIndex);
		return;
	}

	AActor* ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget();
	UGameplayCameraSystemComponent* CameraSystemComponent = ViewTarget->FindComponentByClass<UGameplayCameraSystemComponent>();
	if (!CameraSystemComponent)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't find camera system on the player camera manager."));
		return;
	}

	TSharedPtr<FCameraSystemEvaluator> CameraSystem = CameraSystemComponent->GetCameraSystemEvaluator();
	if (!CameraSystem)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No camera system has been initialized"));
		return;
	}

	// Create the evaluation service, with a copy of our parameters.
	FPlayerControlRotationParams ServiceParams;
	ServiceParams.AxisActionAngularSpeedThreshold = AxisActionAngularSpeedThreshold;
	ServiceParams.AxisActionMagnitudeThreshold = AxisActionMagnitudeThreshold;
	ServiceParams.AxisActions = AxisActions;
	// We will set the control rotation ourselves.
	ServiceParams.bApplyControlRotation = false;

	ControlRotationService = MakeShared<FPlayerControlRotationEvaluationService>(ServiceParams);
	CameraSystem->RegisterEvaluationService(ControlRotationService.ToSharedRef());

	WeakCameraSystem = CameraSystem;
}

void UGameplayControlRotationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	using namespace UE::Cameras;

	if (TSharedPtr<FCameraSystemEvaluator> CameraSystem = WeakCameraSystem.Pin())
	{
		CameraSystem->UnregisterEvaluationService(ControlRotationService.ToSharedRef());
	}

	ControlRotationService.Reset();

	Super::EndPlay(EndPlayReason);
}

void UGameplayControlRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APlayerController* PlayerController = GetPlayerController();
	if (PlayerController)
	{
		// This may be technically one frame late (i.e. we set the control rotation computed 
		// late last tick) unless the camera system is setup to process player input into camera
		// rotation early in the frame.
		PlayerController->SetControlRotation(ControlRotationService->GetCurrentControlRotation());
	}
}

APlayerController* UGameplayControlRotationComponent::GetPlayerController()
{
	if (PlayerIndex != INDEX_NONE)
	{
		return UGameplayStatics::GetPlayerController(this, PlayerIndex);
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE

