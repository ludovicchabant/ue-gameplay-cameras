// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraComponent.h"

#include "CineCameraComponent.h"
#include "Core/CameraAsset.h"
#include "Core/CameraAssetBuilder.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "Debug/CameraDebugRenderer.h"
#include "Engine/Canvas.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "GameplayCamerasDelegates.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AssertionMacros.h"
#include "PrimitiveDrawInterface.h"
#include "SceneView.h"
#include "ShowFlags.h"

#if WITH_EDITOR
#include "Editor.h"
#include "LevelEditorViewport.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraComponent)

#define LOCTEXT_NAMESPACE "GameplayCameraComponent"

UGameplayCameraComponent::UGameplayCameraComponent(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	bTickInEditor = true;
	bWantsOnUpdateTransform = true;

	PrimaryComponentTick.bCanEverTick = true;

	OutputCameraComponent = ObjectInit.CreateDefaultSubobject<UCineCameraComponent>(this, TEXT("OutputCameraComponent"), true);
	OutputCameraComponent->SetupAttachment(this);
}

void UGameplayCameraComponent::PostLoad()
{
	Super::PostLoad();

	if (Camera_DEPRECATED)
	{
		CameraReference.SetCameraAsset(Camera_DEPRECATED);
		Camera_DEPRECATED = nullptr;
	}
}

void UGameplayCameraComponent::BeginDestroy()
{
	Super::BeginDestroy();

#if WITH_EDITOR

	if (EditorPreviewEvaluator)
	{
		EditorPreviewEvaluator.Reset();
	}

	if (EvaluationContext)
	{
		EvaluationContext.Reset();
	}

#endif  // WITH_EDITOR
}

void UGameplayCameraComponent::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	UGameplayCameraComponent* This = CastChecked<UGameplayCameraComponent>(InThis);
	if (This->EvaluationContext.IsValid())
	{
		This->EvaluationContext->AddReferencedObjects(Collector);
	}

#if WITH_EDITOR

	if (This->EditorPreviewEvaluator)
	{
		This->EditorPreviewEvaluator->AddReferencedObjects(Collector);
	}

#endif  // WITH_EDITOR
}

TSharedPtr<const UE::Cameras::FCameraEvaluationContext> UGameplayCameraComponent::GetEvaluationContext() const
{
	return EvaluationContext;
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

	if (OutputCameraComponent)
	{
		OutputCameraComponent->SetRelativeTransform(FTransform());
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

	if (!CameraReference.IsValid())
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

	EnsureCameraEvaluationContextCreated(PlayerController);

	TSharedPtr<FCameraSystemEvaluator> CameraSystemEvaluator = CameraSystemHost->GetCameraSystemEvaluator();
	CameraSystemEvaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());

	// Make sure the component is active so it receives tick updates to maintain the evaluation context.
	Activate();
}

void UGameplayCameraComponent::EnsureCameraEvaluationContextCreated(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (!EvaluationContext.IsValid())
	{
		EvaluationContext = MakeShared<FGameplayCameraComponentEvaluationContext>();

		FCameraEvaluationContextInitializeParams InitParams;
		InitParams.Owner = this;
		InitParams.CameraAsset = CameraReference.GetCameraAsset();
		InitParams.PlayerController = PlayerController;
		EvaluationContext->Initialize(InitParams);

		UpdateCameraEvaluationContext(true);
	}
}

#define UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT(ErrorMsg, ErrorResult)\
	using namespace UE::Cameras;\
	if (!EvaluationContext)\
	{\
		FFrame::KismetExecutionMessage(\
				*FString::Format(\
					TEXT(#ErrorResult " on Gameplay Camera component '{0}': it isn't active."),\
					{ *GetNameSafe(this) }),\
				ELogVerbosity::Error);\
		return ErrorResult;\
	}

FBlueprintCameraNodeEvaluationResult UGameplayCameraComponent::GetInitialResult() const
{
	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't get shared camera data", FBlueprintCameraNodeEvaluationResult());

	return FBlueprintCameraNodeEvaluationResult(&EvaluationContext->GetInitialResult());
}

FBlueprintCameraNodeEvaluationResult UGameplayCameraComponent::GetConditionalResult(ECameraEvaluationDataCondition Condition) const
{
	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't get conditional camera data", FBlueprintCameraNodeEvaluationResult());

	return FBlueprintCameraNodeEvaluationResult(&EvaluationContext->GetOrAddConditionalResult(Condition));
}

FBlueprintCameraPose UGameplayCameraComponent::GetInitialPose() const
{
	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't get initial camera pose", FBlueprintCameraPose());

	return FBlueprintCameraPose::FromCameraPose(EvaluationContext->GetInitialResult().CameraPose);
}

bool UGameplayCameraComponent::SetInitialPose(const FBlueprintCameraPose& CameraPose)
{
	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't set initial camera pose", false);

	FCameraPose InitialPose = EvaluationContext->GetInitialResult().CameraPose;
	CameraPose.ApplyTo(InitialPose);
	return true;
}

FBlueprintCameraVariableTable UGameplayCameraComponent::GetInitialVariableTable() const
{
	using namespace UE::Cameras;

	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't get initial camera variable table", FBlueprintCameraVariableTable());

	FCameraVariableTable& VariableTable = EvaluationContext->GetInitialResult().VariableTable;
	return FBlueprintCameraVariableTable(&VariableTable);
}

#undef UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT

void UGameplayCameraComponent::OnRegister()
{
	using namespace UE::Cameras;

	Super::OnRegister();

#if WITH_EDITOR

	UWorld* World = GetWorld();
	bIsEditorWorld = (World && (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview));

	FGameplayCamerasDelegates::OnCameraAssetBuilt().AddUObject(this, &UGameplayCameraComponent::OnCameraAssetBuilt);

	const TCHAR* ShowFlagName = TEXT("GameplayCameras");
	CustomShowFlag = FEngineShowFlags::FindIndexByName(ShowFlagName);

#endif  // WITH_EDITOR
}

void UGameplayCameraComponent::BeginPlay()
{
	using namespace UE::Cameras;

	Super::BeginPlay();

#if WITH_EDITOR

	if (CameraReference.IsValid())
	{
		UWorld* World = GetWorld();
		if (World && World->WorldType == EWorldType::PIE)
		{
			// Auto-build the camera asset on begin play to make sure we've got the latest user edits.
			FCameraBuildLog BuildLog;
			FCameraAssetBuilder Builder(BuildLog);
			Builder.BuildCamera(CameraReference.GetCameraAsset());
		}

		CameraReference.RebuildParametersIfNeeded();
	}

#endif  // WITH_EDITOR

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

void UGameplayCameraComponent::OnUnregister()
{
	using namespace UE::Cameras;

#if WITH_EDITOR

	FGameplayCamerasDelegates::OnCameraAssetBuilt().RemoveAll(this);

#endif  // WITH_EDITOR

	Super::OnUnregister();
}

void UGameplayCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	using namespace UE::Cameras;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR

	// Make sure things are setup (or not) if we want to run the camera logic in editor (or not).
	AutoManageEditorPreviewEvaluator();

#endif  // WITH_EDITOR

	if (EvaluationContext)
	{
		UpdateCameraEvaluationContext(false);

#if WITH_EDITOR
		
		UpdateEditorPreviewEvaluator(DeltaTime);

#endif  // WITH_EDITOR

		UpdateOutputCameraComponent();
	}
}

void UGameplayCameraComponent::UpdateCameraEvaluationContext(bool bForceApplyParameterOverrides)
{
	using namespace UE::Cameras;

	FCameraNodeEvaluationResult& InitialResult = EvaluationContext->GetInitialResult();

	const FTransform& OwnerTransform = GetComponentTransform();
	InitialResult.CameraPose.SetTransform(OwnerTransform, true);
	InitialResult.bIsCameraCut = false;
	InitialResult.bIsValid = true;

	if (bIsCameraCutNextFrame)
	{
		InitialResult.bIsCameraCut = true;
		bIsCameraCutNextFrame = false;
	}

	const bool bApplyDrivenParametersOnly = !bForceApplyParameterOverrides;
	CameraReference.ApplyParameterOverrides(InitialResult, bApplyDrivenParametersOnly);

#if WITH_EDITOR
	EvaluationContext->UpdateForEditorPreview();
#endif  // WITH_EDITOR
}

void UGameplayCameraComponent::UpdateOutputCameraComponent()
{
	using namespace UE::Cameras;

	if (!OutputCameraComponent)
	{
		return;
	}

	TSharedPtr<FCameraSystemEvaluator> CameraSystemEvaluator;

	if (CameraSystemHost)
	{
		CameraSystemEvaluator = CameraSystemHost->GetCameraSystemEvaluator();
	}
#if WITH_EDITOR
	else if (EditorPreviewEvaluator)
	{
		CameraSystemEvaluator = EditorPreviewEvaluator;
	}
#endif  // WITH_EDITOR

	bool bGotValidTransform = false;
	if (CameraSystemEvaluator)
	{
		FRootCameraNodeEvaluator* RootNodeEvaluator = CameraSystemEvaluator->GetRootNodeEvaluator();
		if (RootNodeEvaluator && RootNodeEvaluator->HasAnyActiveCameraRig())
		{
			const FCameraSystemEvaluationResult& Result = CameraSystemEvaluator->GetEvaluatedResult();

			OutputCameraComponent->SetWorldTransform(Result.CameraPose.GetTransform());
			OutputCameraComponent->SetFieldOfView(Result.CameraPose.GetEffectiveFieldOfView());
			OutputCameraComponent->CurrentAperture = Result.CameraPose.GetAperture();
			OutputCameraComponent->Filmback.SensorWidth = Result.CameraPose.GetSensorWidth();
			OutputCameraComponent->Filmback.SensorHeight = Result.CameraPose.GetSensorHeight();
			OutputCameraComponent->bConstrainAspectRatio = Result.CameraPose.GetConstrainAspectRatio();
			OutputCameraComponent->bOverrideAspectRatioAxisConstraint = Result.CameraPose.GetOverrideAspectRatioAxisConstraint();
			OutputCameraComponent->AspectRatioAxisConstraint = Result.CameraPose.GetAspectRatioAxisConstraint();

			OutputCameraComponent->FocusSettings.ManualFocusDistance = Result.CameraPose.GetFocusDistance();
			OutputCameraComponent->FocusSettings.FocusMethod = (Result.CameraPose.GetEnablePhysicalCamera() ? ECameraFocusMethod::Manual : ECameraFocusMethod::Disable);

			bGotValidTransform = true;
		}
	}
	
	if (!bGotValidTransform)
	{
		OutputCameraComponent->SetRelativeTransform(FTransform());
	}
}

void UGameplayCameraComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UGameplayCameraComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	if (EvaluationContext && Teleport != ETeleportType::None)
	{
		bIsCameraCutNextFrame = true;
	}

#if WITH_EDITOR

	if (bIsEditorWorld && EvaluationContext)
	{
		UpdateCameraEvaluationContext(false);
	}

#endif  // WITH_EDITOR
}

#if WITH_EDITOR

void UGameplayCameraComponent::AutoManageEditorPreviewEvaluator()
{
	using namespace UE::Cameras;

	if (!bIsEditorWorld)
	{
		return;
	}
	
	if (bRunInEditor && !(EditorPreviewEvaluator && EvaluationContext))
	{
		// We want to run the camera logic in the editor but we haven't set things up for that.
		// Let's create the preview evaluator and the evaluation context.
		if (!EditorPreviewEvaluator)
		{
			EditorPreviewEvaluator = MakeShared<FCameraSystemEvaluator>();

			FCameraSystemEvaluatorCreateParams CreateParams;
			CreateParams.Owner = this;
			CreateParams.Role = ECameraSystemEvaluatorRole::EditorPreview;
			EditorPreviewEvaluator->Initialize(CreateParams);
		}
		if (!EvaluationContext)
		{
			EnsureCameraEvaluationContextCreated(nullptr);
			EditorPreviewEvaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());
			EvaluationContext->SetEditorPreviewCameraRigIndex(EditorPreviewCameraRigIndex);
		}
	}
	else if (!bRunInEditor && (EditorPreviewEvaluator || EvaluationContext))
	{
		// We don't want to run the camera logic in the editor anymore. Let's tear things down.
		EditorPreviewEvaluator = nullptr;
		EvaluationContext = nullptr;
	}
}

void UGameplayCameraComponent::OnCameraAssetReferenceChanged()
{
	if (!bIsEditorWorld)
	{
		return;
	}

	if (bRunInEditor && EditorPreviewEvaluator && EvaluationContext)
	{
		if (EvaluationContext->GetCameraAsset() != CameraReference.GetCameraAsset())
		{
			// The camera asset has changed! Recreate the context.
			EditorPreviewEvaluator->RemoveEvaluationContext(EvaluationContext.ToSharedRef());
			EvaluationContext = nullptr;

			EnsureCameraEvaluationContextCreated(nullptr);
			EditorPreviewEvaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());
		}
		else
		{
			// Otherwise, maybe one of the parameter overrides has changed. Re-apply them.
			UpdateCameraEvaluationContext(true);
		}
	}
}

void UGameplayCameraComponent::OnEditorPreviewCameraRigIndexChanged()
{
	if (!bIsEditorWorld)
	{
		return;
	}

	if (bRunInEditor && EditorPreviewEvaluator && EvaluationContext)
	{
		EvaluationContext->SetEditorPreviewCameraRigIndex(EditorPreviewCameraRigIndex);
	}
}

void UGameplayCameraComponent::OnCameraAssetBuilt(const UCameraAsset* InCameraAsset)
{
	using namespace UE::Cameras;

	if (InCameraAsset != CameraReference.GetCameraAsset())
	{
		return;
	}

	// If our camera asset was just built, it may have some new parameters. We need to rebuild
	// our variable table and context data table, and re-apply overrides.
	if (bRunInEditor && EditorPreviewEvaluator && EvaluationContext)
	{
		const FCameraAssetAllocationInfo& AllocationInfo = InCameraAsset->GetAllocationInfo();
		FCameraNodeEvaluationResult& InitialResult = EvaluationContext->GetInitialResult();
		InitialResult.VariableTable.Initialize(AllocationInfo.VariableTableInfo);
		InitialResult.ContextDataTable.Initialize(AllocationInfo.ContextDataTableInfo);

		CameraReference.RebuildParametersIfNeeded();

		UpdateCameraEvaluationContext(true);
	}
}

bool UGameplayCameraComponent::GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut)
{
	if (OutputCameraComponent)
	{
		OutputCameraComponent->GetEditorPreviewInfo(DeltaTime, ViewOut);
		return true;
	}
	return false;
}

void UGameplayCameraComponent::UpdateEditorPreviewEvaluator(float DeltaTime)
{
	using namespace UE::Cameras;

	if (EditorPreviewEvaluator)
	{
		FCameraSystemEvaluationParams Params;
		Params.DeltaTime = DeltaTime;
		EditorPreviewEvaluator->Update(Params);
	}
}

void UGameplayCameraComponent::OnDrawVisualizationHUD(const FViewport* Viewport, const FSceneView* SceneView, FCanvas* Canvas) const
{
	using namespace UE::Cameras;

	const bool bHasShowFlag = SceneView->Family->EngineShowFlags.GetSingleFlag(CustomShowFlag);
	if (bHasShowFlag && bRunInEditor && EditorPreviewEvaluator && EvaluationContext)
	{
		const AActor* OwnerActor = GetOwner();

		const AActor* ViewActor = SceneView->ViewActor.Get();
		const bool bIsLockedToCamera = (ViewActor == OwnerActor);

		FCameraSystemEditorPreviewParams Params;
		Params.Canvas = Canvas;
		Params.SceneView = SceneView;
		Params.bIsLockedToCamera = bIsLockedToCamera;
		Params.bDrawWorldDebug = false;

		EditorPreviewEvaluator->DrawEditorPreview(Params);
	}
}

void UGameplayCameraComponent::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent)
{
	using namespace UE::Cameras;

	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraComponent, CameraReference))
	{
		OnCameraAssetReferenceChanged();
	}
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraComponent, bRunInEditor))
	{
		AutoManageEditorPreviewEvaluator();
	}
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraComponent, EditorPreviewCameraRigIndex))
	{
		OnEditorPreviewCameraRigIndexChanged();
	}
}

#endif  // WITH_EDITOR

namespace UE::Cameras
{

UE_DEFINE_CAMERA_EVALUATION_CONTEXT(FGameplayCameraComponentEvaluationContext)

#if WITH_EDITOR

void FGameplayCameraComponentEvaluationContext::UpdateForEditorPreview()
{
	FCameraSystemEvaluator* ActiveEvaluator = GetCameraSystemEvaluator();
	if (ActiveEvaluator && ActiveEvaluator->GetRole() == ECameraSystemEvaluatorRole::EditorPreview)
	{
		if (GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->Viewport)
		{
			FIntPoint ViewportSize = GCurrentLevelEditingViewportClient->Viewport->GetSizeXY();
			OverrideViewportSize = ViewportSize;
		}
		else
		{
			OverrideViewportSize.Reset();
		}
	}
}

#endif  // WITH_EDITOR

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

