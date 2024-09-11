// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Core/CameraSystemEvaluator.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraSystemComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraSystemComponent"

UGameplayCameraSystemComponent::UGameplayCameraSystemComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
#if WITH_EDITORONLY_DATA
	if (GIsEditor && !IsRunningCommandlet())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> EditorCameraMesh(
				TEXT("/Engine/EditorMeshes/Camera/SM_CineCam.SM_CineCam"));
		PreviewMesh = EditorCameraMesh.Object;
	}
#endif  // WITH_EDITORONLY_DATA
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

#if WITH_EDITORONLY_DATA
	if (PreviewMesh && !PreviewMeshComponent)
	{
		PreviewMeshComponent = NewObject<UStaticMeshComponent>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		PreviewMeshComponent->SetupAttachment(this);
		PreviewMeshComponent->SetIsVisualizationComponent(true);
		PreviewMeshComponent->SetStaticMesh(PreviewMesh);
		PreviewMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		PreviewMeshComponent->bHiddenInGame = true;
		PreviewMeshComponent->CastShadow = false;
		PreviewMeshComponent->CreationMethod = CreationMethod;
		PreviewMeshComponent->RegisterComponentWithWorld(GetWorld());
	}
#endif	// WITH_EDITORONLY_DATA
}

void UGameplayCameraSystemComponent::ActivateCameraSystemForPlayerIndex(int32 PlayerIndex)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (!PlayerController)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera system: no player controller found!"));
		return;
	}

	ActivateCameraSystemForPlayerController(PlayerController);
}

void UGameplayCameraSystemComponent::ActivateCameraSystemForPlayerController(APlayerController* PlayerController)
{
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
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera system: no owning actor found!"));
		return;
	}

	if (!CameraSystemHost)
	{
		CameraSystemHost = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
		if (!CameraSystemHost)
		{
			UE_LOG(LogCameraSystem, Error, TEXT("can't create camera system host!"));
			return;
		}
	}

	PlayerController->SetViewTarget(OwningActor);
	WeakPlayerController = PlayerController;
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

#if WITH_EDITORONLY_DATA
	if (PreviewMeshComponent)
	{
		PreviewMeshComponent->DestroyComponent();
	}
#endif  // WITH_EDITORONLY_DATA

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

	TSharedPtr<FCameraSystemEvaluator> Evaluator = GetCameraSystemEvaluator();
	if (Evaluator.IsValid())
	{
		FCameraSystemDebugUpdateParams DebugUpdateParams;
		DebugUpdateParams.CanvasObject = Canvas;
		Evaluator->DebugUpdate(DebugUpdateParams);
	}
}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#undef LOCTEXT_NAMESPACE

