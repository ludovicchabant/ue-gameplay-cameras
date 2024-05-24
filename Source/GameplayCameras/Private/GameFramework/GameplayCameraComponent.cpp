// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Core/CameraSystemEvaluator.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CameraEvaluationResultInterop.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameplayCameras.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/MessageLog.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraComponent"

UGameplayCameraComponent::UGameplayCameraComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	PrimaryComponentTick.bCanEverTick = true;

	InitialResultInterop = ObjectInit.CreateDefaultSubobject<UCameraEvaluationResultInterop>(this, "InitialResultInterop");

#if WITH_EDITORONLY_DATA
	if (GIsEditor && !IsRunningCommandlet())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> EditorCameraMesh(
				TEXT("/Engine/EditorMeshes/Camera/SM_CineCam.SM_CineCam"));
		PreviewMesh = EditorCameraMesh.Object;
	}
#endif  // WITH_EDITORONLY_DATA
}

void UGameplayCameraComponent::ActivateCamera(int32 PlayerIndex)
{
	if (ActivatedForPlayerIndex == PlayerIndex)
	{
		return;
	}

	if (ActivatedForPlayerIndex >= 0)
	{
		DeactivateCamera();
	}

	Activate();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, PlayerIndex))
	{
		ActivateCamera(PC);
		ActivatedForPlayerIndex = PlayerIndex;
	}
}

void UGameplayCameraComponent::DeactivateCamera()
{
	if (ActivatedForPlayerIndex < 0)
	{
		return;
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, ActivatedForPlayerIndex))
	{
		DeactivateCamera(PC);
	}

	ActivatedForPlayerIndex = INDEX_NONE;

	Deactivate();
}

void UGameplayCameraComponent::ActivateCamera(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (!ensureMsgf(
				PlayerController && PlayerController->PlayerCameraManager,
				TEXT("Can't activate gameplay camera component: invalid player controller!")))
	{
		return;
	}
	
	AGameplayCameraSystemActor* CameraSystem = Cast<AGameplayCameraSystemActor>(PlayerController->PlayerCameraManager->GetViewTarget());
	if (!CameraSystem)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera component: no camera system found on the view target!"));
		return;
	}

	if (!Camera)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate gameplay camera component: no camera asset was set!"));
		return;
	}

	if (!EvaluationContext.IsValid())
	{
		EvaluationContext = MakeShared<FGameplayCameraComponentEvaluationContext>();

		FCameraEvaluationContextInitializeParams InitParams;
		InitParams.Owner = this;
		InitParams.CameraAsset = Camera;
		InitParams.PlayerController = PlayerController;
		EvaluationContext->Initialize(InitParams);

		InitialResultInterop->Setup(&EvaluationContext->GetInitialResult());
	}

	TSharedPtr<FCameraSystemEvaluator> Evaluator = CameraSystem->GetCameraSystemComponent()->GetCameraSystemEvaluator();
	Evaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());

	Activate();
}

void UGameplayCameraComponent::DeactivateCamera(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (!ensure(PlayerController && PlayerController->PlayerCameraManager))
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

	Deactivate();
}

UCameraEvaluationResultInterop* UGameplayCameraComponent::GetInitialResult() const
{
	return InitialResultInterop;
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

void UGameplayCameraComponent::Deactivate()
{
	DeactivateCamera();

	Super::Deactivate();
}

void UGameplayCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AutoActivateForPlayer != EAutoReceiveInput::Disabled && GetNetMode() != NM_DedicatedServer)
	{
		const int32 PlayerIndex = AutoActivateForPlayer.GetIntValue() - 1;
		ActivateCamera(PlayerIndex);
	}
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

