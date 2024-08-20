// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Core/CameraAssetBuilder.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraSystemEvaluator.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameDelegates.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameplayCameras.h"
#include "GameplayCamerasSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/MessageLog.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraComponent"

UGameplayCameraComponent::UGameplayCameraComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITORONLY_DATA
	if (GIsEditor && !IsRunningCommandlet())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> EditorCameraMesh(
				TEXT("/Engine/EditorMeshes/Camera/SM_CineCam.SM_CineCam"));
		PreviewMesh = EditorCameraMesh.Object;
	}
#endif  // WITH_EDITORONLY_DATA
}

TSharedPtr<UE::Cameras::FCameraEvaluationContext> UGameplayCameraComponent::GetEvaluationContext()
{
	return EvaluationContext;
}

void UGameplayCameraComponent::ActivateCameraForPlayerIndex(int32 PlayerIndex)
{
	ActivateCameraEvaluationContext(PlayerIndex);
}

void UGameplayCameraComponent::ActivateCameraForPlayerController(APlayerController* PlayerController)
{
	ActivateCameraEvaluationContext(PlayerController);
}

void UGameplayCameraComponent::DeactivateCamera()
{
	DeactivateCameraEvaluationContext();
}

void UGameplayCameraComponent::ActivateCameraEvaluationContext(int32 PlayerIndex)
{
	if (WeakPlayerController.IsValid())
	{
		DeactivateCameraEvaluationContext();
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (!PlayerController)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera: no player controller found!"));
		return;
	}

	ActivateCameraEvaluationContext(PlayerController);
}

void UGameplayCameraComponent::DeactivateCameraEvaluationContext()
{
	using namespace UE::Cameras;

	APlayerController* PlayerController = WeakPlayerController.Get();
	if (!PlayerController)
	{
		return;
	}

	if (!ensure(PlayerController->PlayerCameraManager))
	{
		return;
	}
	
	AGameplayCameraSystemActor* CameraSystem = Cast<AGameplayCameraSystemActor>(PlayerController->PlayerCameraManager->GetViewTarget());
	if (!ensure(CameraSystem))
	{
		return;
	}

	if (EvaluationContext.IsValid())
	{
		TSharedPtr<FCameraSystemEvaluator> Evaluator = CameraSystem->GetCameraSystemComponent()->GetCameraSystemEvaluator();
		Evaluator->RemoveEvaluationContext(EvaluationContext.ToSharedRef());
	}

	WeakPlayerController.Reset();
}

void UGameplayCameraComponent::ActivateCameraEvaluationContext(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (!ensureMsgf(
				PlayerController && PlayerController->PlayerCameraManager,
				TEXT("Can't activate gameplay camera component: invalid player controller!")))
	{
		return;
	}

	if (!Camera)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera component: no camera asset was set!"));
		return;
	}
	
	AGameplayCameraSystemActor* CameraSystem = Cast<AGameplayCameraSystemActor>(PlayerController->PlayerCameraManager->GetViewTarget());
	if (!CameraSystem)
	{
		const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
		if (Settings->bAutoSpawnCameraSystemActor)
		{
			FActorSpawnParameters SpawnParams;
			CameraSystem = GetWorld()->SpawnActor<AGameplayCameraSystemActor>(SpawnParams);
			CameraSystem->GetCameraSystemComponent()->ActivateCameraSystemForPlayerController(PlayerController);
		}
	}
	if (!CameraSystem)
	{
		FGameDelegates::Get().GetViewTargetChangedDelegate().AddUObject(
				this, &UGameplayCameraComponent::DelayedActivateCameraEvaluationContext);

		GetWorld()->GetTimerManager().SetTimerForNextTick(
				this, &UGameplayCameraComponent::AbortDelayedActivateCameraEvaluationContext);

		UE_LOG(LogCameraSystem, Verbose, 
				TEXT("Can't activate gameplay camera component: no camera system found on the view target. "
					"Waiting until next tick to see if it is added after us."));
		return;
	}

	DoActivateCameraEvaluationContext(PlayerController, CameraSystem->GetCameraSystemComponent()->GetCameraSystemEvaluator());

	WeakPlayerController = PlayerController;
}

void UGameplayCameraComponent::DelayedActivateCameraEvaluationContext(APlayerController* PlayerController, AActor* OldViewTarget, AActor* NewViewTarget)
{
	FGameDelegates::Get().GetViewTargetChangedDelegate().RemoveAll(this);

	AGameplayCameraSystemActor* CameraSystem = Cast<AGameplayCameraSystemActor>(NewViewTarget);
	if (!CameraSystem)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera component: no camera system found on the view target!"));
		return;
	}

	DoActivateCameraEvaluationContext(PlayerController, CameraSystem->GetCameraSystemComponent()->GetCameraSystemEvaluator());

	WeakPlayerController = PlayerController;
}

void UGameplayCameraComponent::AbortDelayedActivateCameraEvaluationContext()
{
	if (!WeakPlayerController.IsValid())
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera component: no camera system found after a full tick!"));
		FGameDelegates::Get().GetViewTargetChangedDelegate().RemoveAll(this);
	}
}

void UGameplayCameraComponent::DoActivateCameraEvaluationContext(APlayerController* PlayerController, TSharedPtr<UE::Cameras::FCameraSystemEvaluator> CameraSystemEvaluator)
{
	using namespace UE::Cameras;

	if (!EvaluationContext.IsValid())
	{
		EvaluationContext = MakeShared<FGameplayCameraComponentEvaluationContext>();

		FCameraEvaluationContextInitializeParams InitParams;
		InitParams.Owner = this;
		InitParams.CameraAsset = Camera;
		InitParams.PlayerController = PlayerController;
		EvaluationContext->Initialize(InitParams);
	}

	CameraSystemEvaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());
}

FBlueprintCameraPose UGameplayCameraComponent::GetInitialPose() const
{
	return FBlueprintCameraPose::FromCameraPose(EvaluationContext->GetInitialResult().CameraPose);
}

void UGameplayCameraComponent::SetInitialPose(const FBlueprintCameraPose& CameraPose)
{
	FCameraPose InitialPose = EvaluationContext->GetInitialResult().CameraPose;
	CameraPose.ApplyTo(InitialPose);
}

FBlueprintCameraVariableTable UGameplayCameraComponent::GetInitialVariableTable() const
{
	return FBlueprintCameraVariableTable(&EvaluationContext->GetInitialResult().VariableTable);
}

void UGameplayCameraComponent::OnRegister()
{
	Super::OnRegister();

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

	UpdatePreviewMeshTransform();
#endif	// WITH_EDITORONLY_DATA
}

void UGameplayCameraComponent::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	if (Camera)
	{
		using namespace UE::Cameras;
		FCameraBuildLog BuildLog;
		FCameraAssetBuilder Builder(BuildLog);
		Builder.BuildCamera(Camera);
	}
#endif

	if (AutoActivateForPlayer != EAutoReceiveInput::Disabled && GetNetMode() != NM_DedicatedServer)
	{
		const int32 PlayerIndex = AutoActivateForPlayer.GetIntValue() - 1;
		ActivateCameraForPlayerIndex(PlayerIndex);
	}
}

void UGameplayCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateCameraEvaluationContext();

	Super::EndPlay(EndPlayReason);
}

void UGameplayCameraComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EvaluationContext)
	{
		EvaluationContext->Update(this);
	}
}

void UGameplayCameraComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

#if WITH_EDITORONLY_DATA
	if (PreviewMeshComponent)
	{
		PreviewMeshComponent->DestroyComponent();
	}
#endif  // WITH_EDITORONLY_DATA
}

#if WITH_EDITORONLY_DATA

void UGameplayCameraComponent::UpdatePreviewMeshTransform()
{
	if (PreviewMeshComponent)
	{
		// CineCam mesh is wrong, adjust like UCineCameraComponent
		PreviewMeshComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		PreviewMeshComponent->SetRelativeLocation(FVector(-46.f, 0, -24.f));
		PreviewMeshComponent->SetRelativeScale3D(FVector::OneVector);
	}
}

#endif

namespace UE::Cameras
{

UE_DEFINE_CAMERA_EVALUATION_CONTEXT(FGameplayCameraComponentEvaluationContext)

void FGameplayCameraComponentEvaluationContext::Update(UGameplayCameraComponent* Owner)
{
	const FTransform& OwnerTransform = Owner->GetComponentTransform();
	InitialResult.CameraPose.SetTransform(OwnerTransform);
	InitialResult.bIsValid = true;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

