// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/MessageLog.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraSystemComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraSystemComponent"

UGameplayCameraSystemComponent::UGameplayCameraSystemComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
#if WITH_EDITORONLY_DATA
	bTickInEditor = true;
	PrimaryComponentTick.bCanEverTick = true;

	if (GIsEditor && !IsRunningCommandlet())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> EditorCameraMesh(
				TEXT("/Engine/EditorMeshes/Camera/SM_CineCam.SM_CineCam"));
		PreviewMesh = EditorCameraMesh.Object;
	}
#endif  // WITH_EDITORONLY_DATA
}

void UGameplayCameraSystemComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	using namespace UE::Cameras;

	if (Evaluator.IsValid())
	{
		FCameraSystemEvaluationParams UpdateParams;
		UpdateParams.DeltaTime = DeltaTime;
		Evaluator->Update(UpdateParams);

		if (bSetPlayerControllerRotation && ActivatedForPlayerIndex != INDEX_NONE)
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, ActivatedForPlayerIndex);
			if (PC)
			{
				PC->SetControlRotation(Evaluator->GetEvaluatedResult().CameraPose.GetRotation());
			}
		}

		Evaluator->GetEvaluatedCameraView(DesiredView);
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

	if (!Evaluator.IsValid())
	{
		Evaluator = MakeShared<FCameraSystemEvaluator>();
		Evaluator->Initialize(this);
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

void UGameplayCameraSystemComponent::Deactivate()
{
	DeactivateCameraSystem();

	Super::Deactivate();
}

void UGameplayCameraSystemComponent::ActivateCameraSystem(int32 PlayerIndex)
{
	if (ActivatedForPlayerIndex == PlayerIndex)
	{
		return;
	}

	if (ActivatedForPlayerIndex >= 0)
	{
		DeactivateCameraSystem();
	}

	AActor* OwningActor = GetOwner();
	if (!OwningActor)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera system: no owning actor found!"));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (!PC)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera system: no player controller found!"));
		return;
	}

	Activate();

	PC->SetViewTarget(OwningActor);
	ActivatedForPlayerIndex = PlayerIndex;
}

void UGameplayCameraSystemComponent::DeactivateCameraSystem(AActor* NextViewTarget)
{
	if (ActivatedForPlayerIndex < 0)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, ActivatedForPlayerIndex);
	if (PC)
	{
		PC->SetViewTarget(NextViewTarget);
	}

	ActivatedForPlayerIndex = INDEX_NONE;

	Deactivate();
}

void UGameplayCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AutoActivateForPlayer != EAutoReceiveInput::Disabled && GetNetMode() != NM_DedicatedServer)
	{
		const int32 PlayerIndex = AutoActivateForPlayer.GetIntValue() - 1;
		ActivateCameraSystem(PlayerIndex);
	}
}

void UGameplayCameraSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

	if (Evaluator.IsValid())
	{
		Evaluator.Reset();
	}
}

#if WITH_EDITOR

bool UGameplayCameraSystemComponent::GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut)
{
	const bool bIsCameraSystemActive = IsActive();
	if (bIsCameraSystemActive)
	{
		GetCameraView(DeltaTime, ViewOut);
	}
	return bIsCameraSystemActive;
}

#endif  // WITH_EDITOR

void UGameplayCameraSystemComponent::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UGameplayCameraSystemComponent* TypedThis = CastChecked<UGameplayCameraSystemComponent>(InThis);
	if (TypedThis->Evaluator.IsValid())
	{
		TypedThis->Evaluator->AddReferencedObjects(Collector);
	}
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

	if (Evaluator.IsValid())
	{
		FCameraSystemDebugUpdateParams DebugUpdateParams;
		DebugUpdateParams.Canvas = Canvas->Canvas;
		Evaluator->DebugUpdate(DebugUpdateParams);
	}
}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#undef LOCTEXT_NAMESPACE

