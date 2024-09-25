// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Core/CameraAssetBuilder.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraSystemEvaluator.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "GameplayCameras.h"
#include "Kismet/GameplayStatics.h"
#include "Services/AutoResetCameraVariableService.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraComponent"

UGameplayCameraComponent::UGameplayCameraComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	bWantsOnUpdateTransform = true;
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

APlayerController* UGameplayCameraComponent::GetPlayerController() const
{
	if (CameraSystemHost)
	{
		return CameraSystemHost->GetPlayerController();
	}
	return nullptr;
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
	DeactivateCameraEvaluationContext();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (!PlayerController)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera: no player controller found!"),
				ELogVerbosity::Error);
		return;
	}

	ActivateCameraEvaluationContext(PlayerController);
}

void UGameplayCameraComponent::DeactivateCameraEvaluationContext()
{
	using namespace UE::Cameras;

	if (!CameraSystemHost)
	{
		return;
	}

	if (EvaluationContext.IsValid())
	{
		TSharedPtr<FCameraSystemEvaluator> Evaluator = CameraSystemHost->GetCameraSystemEvaluator();
		Evaluator->RemoveEvaluationContext(EvaluationContext.ToSharedRef());
	}

	// Don't deactivate the component: we still need to update our evaluation context while any
	// running camera rigs blend out.
}

void UGameplayCameraComponent::ActivateCameraEvaluationContext(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (!PlayerController)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera component: invalid player controller!"),
				ELogVerbosity::Error);
		return;
	}

	if (!Camera)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera component: no camera asset was set!"),
				ELogVerbosity::Error);
		return;
	}
	
	CameraSystemHost = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
	if (!CameraSystemHost)
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera component: no camera system host found!"),
				ELogVerbosity::Error);
		return;
	}

	AGameplayCameraSystemActor::AutoManageActiveViewTarget(PlayerController);

	if (!EvaluationContext.IsValid())
	{
		EvaluationContext = MakeShared<FGameplayCameraComponentEvaluationContext>();

		FCameraEvaluationContextInitializeParams InitParams;
		InitParams.Owner = this;
		InitParams.CameraAsset = Camera;
		InitParams.PlayerController = PlayerController;
		EvaluationContext->Initialize(InitParams);
	}

	TSharedPtr<FCameraSystemEvaluator> CameraSystemEvaluator = CameraSystemHost->GetCameraSystemEvaluator();
	CameraSystemEvaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());

	// Make sure the component is active so it receives tick updates to maintain the evaluation context.
	Activate();
}

FBlueprintCameraPose UGameplayCameraComponent::GetInitialPose() const
{
	if (EvaluationContext)
	{
		return FBlueprintCameraPose::FromCameraPose(EvaluationContext->GetInitialResult().CameraPose);
	}
	else
	{
		FFrame::KismetExecutionMessage(
				*FString::Format(
					TEXT("Can't get initial camera pose on Gameplay Camera component '{0}': it isn't active."),
					{ *GetNameSafe(this) }),
				ELogVerbosity::Error);
		return FBlueprintCameraPose();
	}
}

void UGameplayCameraComponent::SetInitialPose(const FBlueprintCameraPose& CameraPose)
{
	if (EvaluationContext)
	{
		FCameraPose InitialPose = EvaluationContext->GetInitialResult().CameraPose;
		CameraPose.ApplyTo(InitialPose);
	}
	else
	{
		FFrame::KismetExecutionMessage(
				*FString::Format(
					TEXT("Can't set initial camera pose on Gameplay Camera component '{0}': it isn't active."),
					{ GetNameSafe(this) }),
				ELogVerbosity::Error);
	}
}

FBlueprintCameraVariableTable UGameplayCameraComponent::GetInitialVariableTable() const
{
	using namespace UE::Cameras;

	if (EvaluationContext)
	{
		FCameraVariableTable& VariableTable = EvaluationContext->GetInitialResult().VariableTable;
		FCameraSystemEvaluator* CameraSystemEvaluator = EvaluationContext->GetCameraSystemEvaluator();
		TSharedPtr<FAutoResetCameraVariableService> VariableAutoResetService = 
			CameraSystemEvaluator->FindEvaluationService<FAutoResetCameraVariableService>();
		return FBlueprintCameraVariableTable(&VariableTable, VariableAutoResetService);
	}
	else
	{
		FFrame::KismetExecutionMessage(
				*FString::Format(
					TEXT("Can't get initial camera variable table on Gameplay Camera component '{0}': it isn't active."),
					{ GetNameSafe(this) }),
				ELogVerbosity::Error);
		return FBlueprintCameraVariableTable();
	}
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
		// Auto-build the camera asset on begin play to make sure we've got the latest user edits.
		using namespace UE::Cameras;
		FCameraBuildLog BuildLog;
		FCameraAssetBuilder Builder(BuildLog);
		Builder.BuildCamera(Camera);
	}
#endif

	if (IsActive() && AutoActivateForPlayer != EAutoReceiveInput::Disabled && GetNetMode() != NM_DedicatedServer)
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

		if (bIsCameraCutNextFrame)
		{
			EvaluationContext->GetInitialResult().bIsCameraCut = true;
			bIsCameraCutNextFrame = false;
		}
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

void UGameplayCameraComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	if (EvaluationContext && Teleport != ETeleportType::None)
	{
		bIsCameraCutNextFrame = true;
	}
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

#endif  // WITH_EDITORONLY_DATA

#if WITH_EDITOR

bool UGameplayCameraComponent::GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut)
{
	// TODO: in the future, run the camera asset in a private camera system evaluator, with a UI
	//		 to pick which camera rig to preview.
	const FTransform3d& ComponentTransform = GetComponentTransform();
	ViewOut.Location = ComponentTransform.GetLocation();
	ViewOut.Rotation = ComponentTransform.Rotator();
	return true;
}

#endif  // WITH_EDITOR

namespace UE::Cameras
{

UE_DEFINE_CAMERA_EVALUATION_CONTEXT(FGameplayCameraComponentEvaluationContext)

void FGameplayCameraComponentEvaluationContext::Update(UGameplayCameraComponent* Owner)
{
	const FTransform& OwnerTransform = Owner->GetComponentTransform();
	InitialResult.CameraPose.SetTransform(OwnerTransform);
	InitialResult.bIsCameraCut = false;
	InitialResult.bIsValid = true;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

