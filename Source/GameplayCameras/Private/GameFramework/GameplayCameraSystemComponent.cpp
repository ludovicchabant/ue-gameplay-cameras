// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemComponent.h"

#include "Components/BillboardComponent.h"
#include "Core/CameraSystemEvaluator.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/ICookInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraSystemComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraSystemComponent"

UGameplayCameraSystemComponent::UGameplayCameraSystemComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

TSharedPtr<UE::Cameras::FCameraSystemEvaluator> UGameplayCameraSystemComponent::GetCameraSystemEvaluator(bool bEnsureIfNull)
{
	UGameplayCameraSystemHost* HostPtr = CameraSystemHost.Get();
	ensureMsgf(HostPtr || !bEnsureIfNull, TEXT("Accessing camera system evaluator when we haven't found or created a host for one."));
	if (HostPtr)
	{
		return HostPtr->GetCameraSystemEvaluator();
	}
	return nullptr;
}

void UGameplayCameraSystemComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	using namespace UE::Cameras;

	TSharedPtr<FCameraSystemEvaluator> Evaluator = GetCameraSystemEvaluator();
	if (Evaluator.IsValid())
	{
		FCameraSystemEvaluationParams UpdateParams;
		UpdateParams.DeltaTime = DeltaTime;
		Evaluator->Update(UpdateParams);

		Evaluator->GetEvaluatedCameraView(DesiredView);

		if (bSetPlayerControllerRotation)
		{
			if (APlayerController* PlayerController = WeakPlayerController.Get())
			{
				PlayerController->SetControlRotation(Evaluator->GetEvaluatedResult().CameraPose.GetRotation());
			}
		}
	}
}

void UGameplayCameraSystemComponent::OnRegister()
{
	using namespace UE::Cameras;

	Super::OnRegister();

#if WITH_EDITOR
	CreateCameraSystemSpriteComponent();
#endif  // WITH_EDITOR

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || OwnerActor->HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

#if UE_GAMEPLAY_CAMERAS_DEBUG
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		DebugDrawDelegateHandle = UDebugDrawService::Register(
				TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UGameplayCameraSystemComponent::DebugDraw));
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

#if WITH_EDITOR

void UGameplayCameraSystemComponent::CreateCameraSystemSpriteComponent()
{
	UTexture2D* EditorSpriteTexture = nullptr;
	{
		FCookLoadScope EditorOnlyScope(ECookLoadType::EditorOnly);
		EditorSpriteTexture = LoadObject<UTexture2D>(
				nullptr,
				TEXT("/GameplayCameras/Textures/S_GameplayCameraSystem.S_GameplayCameraSystem"));
	}

	if (EditorSpriteTexture)
	{
		bVisualizeComponent = true;
		CreateSpriteComponent(EditorSpriteTexture);
	}

	if (SpriteComponent)
	{
		SpriteComponent->SpriteInfo.Category = TEXT("Cameras");
		SpriteComponent->SpriteInfo.DisplayName = NSLOCTEXT("SpriteCategory", "Cameras", "Cameras");
		SpriteComponent->SetRelativeScale3D(FVector3d(EditorSpriteTextureScale));
	}
}

#endif  // WITH_EDITOR


void UGameplayCameraSystemComponent::ActivateCameraSystemForPlayerIndex(int32 PlayerIndex)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (!PlayerController)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera system: no player controller found!"),
				ELogVerbosity::Error);
		return;
	}

	ActivateCameraSystemForPlayerController(PlayerController);
}

void UGameplayCameraSystemComponent::ActivateCameraSystemForPlayerController(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera system: invalid player controller given!"),
				ELogVerbosity::Error);
		return;
	}

	if (APlayerController* ActivePlayerController = WeakPlayerController.Get())
	{
		if (ActivePlayerController != PlayerController)
		{
			DeactivateCameraSystem();
		}
	}

	AActor* OwningActor = GetOwner();
	if (!OwningActor)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera system: no owning actor found!"),
				ELogVerbosity::Error);
		return;
	}

	if (!CameraSystemHost)
	{
		CameraSystemHost = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
		if (!CameraSystemHost)
		{
			FFrame::KismetExecutionMessage(TEXT("can't create camera system host!"), ELogVerbosity::Error);
			return;
		}
	}

	PlayerController->SetViewTarget(OwningActor);
	WeakPlayerController = PlayerController;

	// Make sure the component is active.
	Activate();
}

bool UGameplayCameraSystemComponent::IsCameraSystemActiveForPlayController(APlayerController* PlayerController) const
{
	APlayerController* ActivatedPlayerController = WeakPlayerController.Get();
	if (!ActivatedPlayerController || ActivatedPlayerController  != PlayerController)
	{
		return false;
	}

	AActor* OwningActor = GetOwner();
	if (!OwningActor)
	{
		return false;
	}
	
	if (!CameraSystemHost)
	{
		return false;
	}

	if (!ActivatedPlayerController->PlayerCameraManager)
	{
		return false;
	}

	return ActivatedPlayerController->PlayerCameraManager->GetViewTarget() == OwningActor;
}

void UGameplayCameraSystemComponent::DeactivateCameraSystem(AActor* NextViewTarget)
{
	APlayerController* PlayerController = WeakPlayerController.Get();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->SetViewTarget(NextViewTarget);
	WeakPlayerController.Reset();
}

void UGameplayCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsActive() && AutoActivateForPlayer != EAutoReceiveInput::Disabled && GetNetMode() != NM_DedicatedServer)
	{
		const int32 PlayerIndex = AutoActivateForPlayer.GetIntValue() - 1;
		ActivateCameraSystemForPlayerIndex(PlayerIndex);
	}
}

void UGameplayCameraSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateCameraSystem();

	Super::EndPlay(EndPlayReason);
}

void UGameplayCameraSystemComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	if (DebugDrawDelegateHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawDelegateHandle);
		DebugDrawDelegateHandle.Reset();
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

void UGameplayCameraSystemComponent::OnBecomeViewTarget()
{
}

void UGameplayCameraSystemComponent::OnEndViewTarget()
{
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void UGameplayCameraSystemComponent::DebugDraw(UCanvas* Canvas, APlayerController* PlayController)
{
	using namespace UE::Cameras;

	TSharedPtr<FCameraSystemEvaluator> Evaluator = GetCameraSystemEvaluator(false);
	if (Evaluator.IsValid())
	{
		FCameraSystemDebugUpdateParams DebugUpdateParams;
		DebugUpdateParams.CanvasObject = Canvas;
		Evaluator->DebugUpdate(DebugUpdateParams);
	}
}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#undef LOCTEXT_NAMESPACE

