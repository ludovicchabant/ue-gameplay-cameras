// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraComponentBase.h"

#include "Build/CameraAssetBuilder.h"
#include "Build/CameraBuildLog.h"
#include "CineCameraComponent.h"
#include "Core/CameraAsset.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "Debug/CameraDebugRenderer.h"
#include "Engine/Canvas.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameplayCamerasSettings.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AssertionMacros.h"
#include "Misc/EngineVersionComparison.h"
#include "SceneView.h"
#include "ShowFlags.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,6,0)
#include "PrimitiveDrawInterface.h"
#else
#include "SceneManagement.h"
#endif

#if WITH_EDITOR
#include "Editor.h"
#include "LevelEditorViewport.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraComponentBase)

#define LOCTEXT_NAMESPACE "GameplayCameraComponentBase"

UGameplayCameraComponentBase::UGameplayCameraComponentBase(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	bAutoActivate = true;
	bTickInEditor = true;
	bWantsOnUpdateTransform = true;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	OutputCameraComponent = ObjectInit.CreateDefaultSubobject<UCineCameraComponent>(this, TEXT("OutputCameraComponent"), true);
	OutputCameraComponent->SetupAttachment(this);
}

void UGameplayCameraComponentBase::BeginDestroy()
{
	DestroyCameraSystem();

	EvaluationContext = nullptr;

	Super::BeginDestroy();
}

void UGameplayCameraComponentBase::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(InThis, Collector);

	UGameplayCameraComponentBase* This = CastChecked<UGameplayCameraComponentBase>(InThis);
	This->OnAddReferencedObjects(Collector);
	if (This->EvaluationContext.IsValid())
	{
		This->EvaluationContext->AddReferencedObjects(Collector);
	}
}

TSharedPtr<const UE::Cameras::FCameraEvaluationContext> UGameplayCameraComponentBase::GetEvaluationContext() const
{
	return EvaluationContext;
}

TSharedPtr<UE::Cameras::FCameraEvaluationContext> UGameplayCameraComponentBase::GetEvaluationContext()
{
	return EvaluationContext;
}

void UGameplayCameraComponentBase::ActivateCameraForPlayerIndex(
		int32 PlayerIndex, bool bSetAsViewTarget, EGameplayCameraComponentActivationMode ActivationMode)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	ActivateCameraForPlayerController(PlayerController, bSetAsViewTarget, ActivationMode);
}

void UGameplayCameraComponentBase::ActivateCameraForPlayerController(
		APlayerController* PlayerController, bool bSetAsViewTarget, EGameplayCameraComponentActivationMode ActivationMode)
{
	using namespace UE::Cameras;

	// Make sure we are activated, since we need to tick and udpate our evaluation context and, possibly,
	// our private camera system.
	Super::Activate(false);

	// Deactivate any existing evaluation context immediately first, since we might be re-activating with
	// a different player controller, a different insertion point in the director tree, etc.
	DeactivateCameraEvaluationContext(true);

	// See if we are going to run the camera system on our own, or if we can activate our camera inside
	// the player camera manager.
	IGameplayCameraSystemHost* PlayerControllerHost = nullptr;
	if (bSetAsViewTarget && PlayerController)
	{
		PlayerControllerHost = Cast<IGameplayCameraSystemHost>(PlayerController->PlayerCameraManager);
	}
	if (!PlayerControllerHost && ActivationMode != EGameplayCameraComponentActivationMode::Push)
	{
		UE_LOG(LogCameraSystem, Warning,
				TEXT("Gameplay camera component '%s' cannot activate with mode '%s' because no camera system "
					 "was found on the given player controller, or not player controller was specified. "),
				*GetNameSafe(this),
				*UEnum::GetValueAsString(ActivationMode));
	}

	if (PlayerControllerHost)
	{
		TeardownCameraSystemHost();
		ActivateCameraEvaluationContext(PlayerController, PlayerControllerHost, ActivationMode);
	}
	else
	{
		EnsureCameraSystemHost();
		ActivateCameraEvaluationContext(PlayerController, this, EGameplayCameraComponentActivationMode::Push);

		if (bSetAsViewTarget && PlayerController)
		{
			AActor* OwnerActor = GetOwner();
			PlayerController->SetViewTarget(OwnerActor);
		}
	}
}

bool UGameplayCameraComponentBase::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator)
	{
		CameraSystemEvaluator->GetEvaluatedCameraView(OutResult);
		return true;
	}
	return false;
}

void UGameplayCameraComponentBase::DeactivateCamera(bool bImmediately)
{
	DeactivateCameraEvaluationContext(bImmediately);
}

void UGameplayCameraComponentBase::DeactivateCameraEvaluationContext(bool bImmediately)
{
	using namespace UE::Cameras;

	// Remove the evaluation context for the director tree.
	if (EvaluationContext && CameraSystemEvaluator)
	{
		UE_LOG(LogCameraSystem, Log, TEXT("Deactivating gameplay camera '%s'."), *GetNameSafe(this));

		if (TSharedPtr<FCameraEvaluationContext> ParentContext = EvaluationContext->GetParentContext())
		{
			ParentContext->RemoveChildContext(EvaluationContext.ToSharedRef());
		}
		else
		{
			CameraSystemEvaluator->RemoveEvaluationContext(EvaluationContext.ToSharedRef());
		}

		if (bImmediately)
		{
			// We are deactivating immediately (i.e. without letting our camera rigs blend out), so make 
			// sure everything is frozen or disabled before we delete our evaluation context.
			FRootCameraNodeEvaluator* RootNodeEvaluator = CameraSystemEvaluator->GetRootNodeEvaluator();
			RootNodeEvaluator->DeactivateAllCameraRigs(EvaluationContext, true);
		}
	}

	if (EvaluationContext && CameraSystemEvaluator && !bImmediately)
	{
		// Don't deactivate the component right away: we still need to update our evaluation context 
		// while any running camera rigs blend out.
		bIsDeactivating = true;
	}
	else
	{
		if (OutputCameraComponent)
		{
			OutputCameraComponent->SetRelativeTransform(FTransform());
		}

		EvaluationContext = nullptr;
	}
}

bool UGameplayCameraComponentBase::CanRunCameraSystem() const
{
	using namespace UE::Cameras;

#if WITH_EDITOR
	IGameplayCamerasModule& GameplayCamerasModule = IGameplayCamerasModule::Get();
	if (TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager())
	{
		const bool bCanRunInEditor = bRunInEditor && GameplayCamerasModule.GetLiveEditManager()->CanRunInEditor();
		return (!bIsEditorWorld || bCanRunInEditor);
	}
	return !bIsEditorWorld;
#else
	return true;
#endif  // WITH_EDITOR
}

void UGameplayCameraComponentBase::EnsureCameraSystemHost()
{
	using namespace UE::Cameras;

	if (!HasCameraSystem() && CanRunCameraSystem())
	{
		AActor* OwnerActor = GetOwner();
		UE_LOG(LogCameraSystem, Log, 
				TEXT("Creating camera system host for gameplay camera '%s'."),
				*GetNameSafe(OwnerActor));

		FCameraSystemEvaluatorCreateParams Params;
		Params.Owner = this;
#if WITH_EDITOR
		if (bIsEditorWorld)
		{
			Params.Role = ECameraSystemEvaluatorRole::EditorPreview;
		}
#endif  // WITH_EDITOR
		InitializeCameraSystem(Params);
	}
}

void UGameplayCameraComponentBase::TeardownCameraSystemHost()
{
	if (HasCameraSystem())
	{
		DestroyCameraSystem();
	}
}

void UGameplayCameraComponentBase::ActivateCameraEvaluationContext(APlayerController* PlayerController, IGameplayCameraSystemHost* Host, EGameplayCameraComponentActivationMode ActivationMode)
{
	using namespace UE::Cameras;

	if (!ensure(Host))
	{
		return;
	}

	if (!CanRunCameraSystem())
	{
		return;
	}

	if (!OnValidateCameraEvaluationContextActivation())
	{
		return;
	}

	// Make sure the evaluation context has been created. However, this can fail, such as when
	// we don't have a valid camera asset specified.
	TryCreateCameraEvaluationContext(PlayerController);

	if (!EvaluationContext.IsValid())
	{
		return;
	}

	if (EvaluationContext->IsActive())
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't activate gameplay camera component: it is already active!"),
				ELogVerbosity::Error);
		return;
	}

	UE_LOG(LogCameraSystem, Log, 
			TEXT("Activating gameplay camera '%s' with mode '%s'."),
			*GetNameSafe(this), 
			*UEnum::GetValueAsString(ActivationMode));

	TSharedPtr<FCameraSystemEvaluator> HostEvaluator = Host->GetCameraSystemEvaluator();
	FCameraEvaluationContextStack& ContextStack = HostEvaluator->GetEvaluationContextStack();

	switch (ActivationMode)
	{
		case EGameplayCameraComponentActivationMode::Push:
			ContextStack.PushContext(EvaluationContext.ToSharedRef());
			break;
		case EGameplayCameraComponentActivationMode::PushAndInsert:
			{
				TSharedPtr<FCameraEvaluationContext> PreviousActiveContext = ContextStack.GetActiveContext();
				ContextStack.PushContext(EvaluationContext.ToSharedRef());
				if (PreviousActiveContext)
				{
					ContextStack.RemoveContext(PreviousActiveContext.ToSharedRef());
					EvaluationContext->AddChildContext(PreviousActiveContext.ToSharedRef());
				}
			}
			break;
		case EGameplayCameraComponentActivationMode::InsertOrPush:
			{
				if (TSharedPtr<FCameraEvaluationContext> ActiveContext = ContextStack.GetActiveContext())
				{
					ActiveContext->AddChildContext(EvaluationContext.ToSharedRef());
				}
				else
				{
					ContextStack.PushContext(EvaluationContext.ToSharedRef());
				}
			}
			break;
		default:
			ensure(false);
			break;
	}

	// Cancel any ongoing deactivation.
	bIsDeactivating = false;
}

void UGameplayCameraComponentBase::TryCreateCameraEvaluationContext(APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	ensure(CanRunCameraSystem());

	if (!EvaluationContext.IsValid())
	{
		if (UCameraAsset* CameraAsset = GetCameraAsset())
		{
#if WITH_EDITOR
			if (bIsFirstActivation)
			{
				UWorld* World = GetWorld();
				const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
				if (Settings->bAutoBuildInPIE && World && World->WorldType == EWorldType::PIE)
				{
					// Auto-build the camera asset on begin play to make sure we've got the latest user edits.
					FCameraBuildLog BuildLog;
					FCameraAssetBuilder Builder(BuildLog);
					Builder.BuildCamera(CameraAsset);
				}

				bIsFirstActivation = false;
			}
#endif

			EvaluationContext = MakeShared<FGameplayCameraComponentEvaluationContext>();

			FCameraEvaluationContextInitializeParams InitParams;
			InitParams.Owner = this;
			InitParams.CameraAsset = CameraAsset;
			InitParams.PlayerController = PlayerController;
			EvaluationContext->Initialize(InitParams);

			UpdateCameraEvaluationContext(true);
		}
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

FBlueprintCameraEvaluationDataRef UGameplayCameraComponentBase::GetInitialResult() const
{
	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't get shared camera data", FBlueprintCameraEvaluationDataRef());

	return FBlueprintCameraEvaluationDataRef::MakeExternalRef(&EvaluationContext->GetInitialResult());
}

FBlueprintCameraEvaluationDataRef UGameplayCameraComponentBase::GetConditionalResult(ECameraEvaluationDataCondition Condition) const
{
	UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT("Can't get conditional camera data", FBlueprintCameraEvaluationDataRef());

	return FBlueprintCameraEvaluationDataRef::MakeExternalRef(&EvaluationContext->GetOrAddConditionalResult(Condition));
}

#undef UE_PRIVATE_GAMEPLAY_CAMERA_COMPONENT_VALIDATE_EVALUATION_CONTEXT

void UGameplayCameraComponentBase::OnRegister()
{
	using namespace UE::Cameras;

	Super::OnRegister();

#if WITH_EDITOR

	UWorld* World = GetWorld();
	bIsEditorWorld = (World && (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview));

	const TCHAR* ShowFlagName = TEXT("GameplayCameras");
	CustomShowFlag = FEngineShowFlags::FindIndexByName(ShowFlagName);

#endif  // WITH_EDITOR
}

void UGameplayCameraComponentBase::BeginPlay()
{
	using namespace UE::Cameras;

	Super::BeginPlay();

	// If we have been activated in OnRegister() (which happens when bAutoActivate is true), our code 
	// inside Activate() has postponed setting up the camera system evaluation until now, so let's
	// do it.
	// However, it can happen that some BP construction script already called ActivateCameraForXyz()
	// before we got to start play (e.g. from a parent actor) and so in this case, let's skip
	// re-activating for nothing.
	if (IsActive() && !EvaluationContext)
	{
		if (AutoActivateForPlayer != EAutoReceiveInput::Disabled && GetNetMode() != NM_DedicatedServer)
		{
			const int32 PlayerIndex = AutoActivateForPlayer.GetIntValue() - 1;
			ActivateCameraForPlayerIndex(PlayerIndex);
		}
		else
		{
			ActivateCameraForPlayerController(nullptr, false, EGameplayCameraComponentActivationMode::Push);
		}
	}
}

void UGameplayCameraComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateCameraEvaluationContext(true);

	Super::EndPlay(EndPlayReason);
}

void UGameplayCameraComponentBase::Activate(bool bReset)
{
	// When auto-activing, this method gets called during OnRegister, before we have started playing.
	// In this case, we don't activate the camera right away, we wait until BeginPlay.
	const bool bDoActivate = (bReset || ShouldActivate()) && HasBegunPlay();

	Super::Activate(bReset);

	if (bDoActivate)
	{
		DeactivateCameraEvaluationContext(true);

		EnsureCameraSystemHost();
		ActivateCameraEvaluationContext(nullptr, this, EGameplayCameraComponentActivationMode::Push);
	}
}

void UGameplayCameraComponentBase::Deactivate()
{
	DeactivateCameraEvaluationContext(true);

	Super::Deactivate();
}

void UGameplayCameraComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
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
	}

#if WITH_EDITOR

	if (bIsEditorWorld)
	{
		UpdateCameraSystemForEditorPreview(DeltaTime);
	}
	else
	{
		UpdateCameraSystem(DeltaTime);
		UpdateControlRotationIfNeeded();
	}

#else

	UpdateCameraSystem(DeltaTime);
	UpdateControlRotationIfNeeded();

#endif  // WITH_EDITOR

	UpdateOutputCameraComponent();

	CheckPendingDeactivation();
}

void UGameplayCameraComponentBase::CheckPendingDeactivation()
{
	using namespace UE::Cameras;

	if (!bIsDeactivating)
	{
		return;
	}

	// If we were deactivating, we wait until we don't have any running camera rig anymore,
	// at which point we can tear down all our evaluation apparatus.
	bool bDoneDeactivating = true;

	if (CameraSystemEvaluator && EvaluationContext)
	{
		FRootCameraNodeEvaluator* RootNodeEvaluator = CameraSystemEvaluator->GetRootNodeEvaluator();
		bDoneDeactivating = (RootNodeEvaluator->HasAnyRunningCameraRig(EvaluationContext) == false);
	}

	if (bDoneDeactivating)
	{
		TeardownCameraSystemHost();
		EvaluationContext = nullptr;

		// Only call the base class method here: we just want to finish deactivating ourselves
		// by stopping ticking.
		Super::Deactivate();

		bIsDeactivating = false;
	}
}

void UGameplayCameraComponentBase::UpdateCameraEvaluationContext(bool bForceApplyParameterOverrides)
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

	OnUpdateCameraEvaluationContext(bForceApplyParameterOverrides);

#if WITH_EDITOR

	EvaluationContext->UpdateForEditorPreview();

#endif  // WITH_EDITOR
}

void UGameplayCameraComponentBase::UpdateControlRotationIfNeeded()
{
	using namespace UE::Cameras;

	if (!HasCameraSystem() || !HasCameraEvaluationContext() || !bSetControlRotationWhenViewTarget)
	{
		return;
	}

	APlayerController* PlayerController = EvaluationContext->GetPlayerController();
	if (!PlayerController)
	{
		return;
	}

	// If the player camera manager is hosting a camera system, it probably already handles control
	// rotation in its own way.
	if (IGameplayCameraSystemHost* CameraManagerHost = Cast<IGameplayCameraSystemHost>(PlayerController->PlayerCameraManager))
	{
		return;
	}

	// Set control rotation if we are the view target.
	AActor* OwnerActor = GetOwner();
	if (OwnerActor && PlayerController->GetViewTarget() == OwnerActor)
	{
		const FCameraSystemEvaluationResult& Result = CameraSystemEvaluator->GetPreVisualLayerEvaluatedResult();
		const FRotator3d& ControlRotation = Result.CameraPose.GetRotation();
		PlayerController->SetControlRotation(ControlRotation);
	}
}

bool UGameplayCameraComponentBase::IsEditorWorld() const
{
#if WITH_EDITOR
	return bIsEditorWorld;
#else
	return false;
#endif  // WITH_EDITOR
}

#if WITH_EDITOR

void UGameplayCameraComponentBase::ReinitializeCameraEvaluationContext(
			const FCameraVariableTableAllocationInfo& VariableTableAllocationInfo,
			const FCameraContextDataTableAllocationInfo& ContextDataTableAllocationInfo)
{
	using namespace UE::Cameras;

	if (EvaluationContext)
	{
		FCameraNodeEvaluationResult& InitialResult = EvaluationContext->GetInitialResult();
		InitialResult.VariableTable.Initialize(VariableTableAllocationInfo);
		InitialResult.ContextDataTable.Initialize(ContextDataTableAllocationInfo);

		// Also freeze/remove any of our currently running camera rigs, because they might continue
		// accessing variables and data that don't exist anymore.
		if (CameraSystemEvaluator)
		{
			FRootCameraNodeEvaluator* RootEvaluator = CameraSystemEvaluator->GetRootNodeEvaluator();
			RootEvaluator->DeactivateAllCameraRigs(EvaluationContext, true);
		}
	}
}

void UGameplayCameraComponentBase::RecreateEditorWorldCameraEvaluationContext()
{
	using namespace UE::Cameras;

	if (!bIsEditorWorld)
	{
		return;
	}

	// We should only be calling this method to recreate the editor preview evaluator, so check that
	// this is indeed the case.
	if (EvaluationContext && CameraSystemEvaluator)
	{
		FCameraEvaluationContextStack& ContextStack = CameraSystemEvaluator->GetEvaluationContextStack();
		TArray<TSharedPtr<FCameraEvaluationContext>> AllContexts;
		ContextStack.GetAllContexts(AllContexts);
		ensure(AllContexts.Num() == 1 && AllContexts[0] == EvaluationContext);
	}

	// Teardown and rebuild the evaluation context.
	if (EvaluationContext)
	{
		if (CameraSystemEvaluator)
		{
			FRootCameraNodeEvaluator* RootEvaluator = CameraSystemEvaluator->GetRootNodeEvaluator();
			RootEvaluator->DeactivateAllCameraRigs(EvaluationContext.ToSharedRef(), true);
			CameraSystemEvaluator->RemoveEvaluationContext(EvaluationContext.ToSharedRef());
		}
		EvaluationContext = nullptr;

		TryCreateCameraEvaluationContext(nullptr);
		if (CameraSystemEvaluator)
		{
			CameraSystemEvaluator->PushEvaluationContext(EvaluationContext.ToSharedRef());
		}
	}
}

#endif  // WITH_EDITOR

void UGameplayCameraComponentBase::UpdateOutputCameraComponent()
{
	using namespace UE::Cameras;

	if (!OutputCameraComponent)
	{
		return;
	}

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
			OutputCameraComponent->Filmback.SensorHorizontalOffset = Result.CameraPose.GetSensorHorizontalOffset();
			OutputCameraComponent->Filmback.SensorVerticalOffset = Result.CameraPose.GetSensorVerticalOffset();

			OutputCameraComponent->Overscan = Result.CameraPose.GetOverscan();
			OutputCameraComponent->bConstrainAspectRatio = Result.CameraPose.GetConstrainAspectRatio();
			OutputCameraComponent->bOverrideAspectRatioAxisConstraint = Result.CameraPose.GetOverrideAspectRatioAxisConstraint();
			OutputCameraComponent->AspectRatioAxisConstraint = Result.CameraPose.GetAspectRatioAxisConstraint();

			OutputCameraComponent->FocusSettings.ManualFocusDistance = Result.CameraPose.GetFocusDistance();
			OutputCameraComponent->FocusSettings.FocusMethod = (Result.CameraPose.GetEnablePhysicalCamera() ? ECameraFocusMethod::Manual : ECameraFocusMethod::Disable);

			OutputCameraComponent->ProjectionMode = Result.CameraPose.GetProjectionMode();
			OutputCameraComponent->OrthoWidth = Result.CameraPose.GetOrthographicWidth();

			OutputCameraComponent->PostProcessSettings = Result.PostProcessSettings.Get();
			OutputCameraComponent->PostProcessBlendWeight = 1.f;

			bGotValidTransform = true;
		}
	}
	
	if (!bGotValidTransform)
	{
		OutputCameraComponent->SetRelativeTransform(FTransform());
	}
}

void UGameplayCameraComponentBase::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
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

void UGameplayCameraComponentBase::AutoManageEditorPreviewEvaluator()
{
	using namespace UE::Cameras;

	if (!bIsEditorWorld)
	{
		return;
	}
	
	const bool bCanRun = CanRunCameraSystem();
	if (bCanRun && !(CameraSystemEvaluator && EvaluationContext))
	{
		// We want to run the camera logic in the editor but we haven't set things up for that.
		// Let's create the preview evaluator and the evaluation context.
		EnsureCameraSystemHost();

		ActivateCameraEvaluationContext(nullptr, this, EGameplayCameraComponentActivationMode::Push);
		if (EvaluationContext)
		{
			EvaluationContext->SetEditorPreviewCameraRigIndex(EditorPreviewCameraRigIndex);
		}

		// OutputCameraComponent will be updated on the next tick.
	}
	else if (!bCanRun && (CameraSystemEvaluator || EvaluationContext))
	{
		// We don't want to run the camera logic in the editor anymore. Let's tear things down.
		DeactivateCameraEvaluationContext(true);
		TeardownCameraSystemHost();
		EvaluationContext = nullptr;

		if (OutputCameraComponent)
		{
			OutputCameraComponent->SetRelativeTransform(FTransform());
		}
	}
}

void UGameplayCameraComponentBase::OnEditorPreviewCameraRigIndexChanged()
{
	if (bIsEditorWorld)
	{
		const bool bCanRun = CanRunCameraSystem();
		if (bCanRun && CameraSystemEvaluator && EvaluationContext)
		{
			EvaluationContext->SetEditorPreviewCameraRigIndex(EditorPreviewCameraRigIndex);
		}
	}
}

bool UGameplayCameraComponentBase::GetEditorPreviewInfo(float DeltaTime, FMinimalViewInfo& ViewOut)
{
	if (OutputCameraComponent)
	{
		OutputCameraComponent->GetEditorPreviewInfo(DeltaTime, ViewOut);
		return true;
	}
	return false;
}

void UGameplayCameraComponentBase::OnDrawVisualizationHUD(const FViewport* Viewport, const FSceneView* SceneView, FCanvas* Canvas) const
{
	using namespace UE::Cameras;

	const bool bCanRun = CanRunCameraSystem();
	const bool bHasShowFlag = SceneView->Family->EngineShowFlags.GetSingleFlag(CustomShowFlag);
	if (bCanRun && bHasShowFlag && CameraSystemEvaluator && EvaluationContext)
	{
		const AActor* OwnerActor = GetOwner();

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,6,0)
		const AActor* ViewActor = SceneView->ViewActor.Get();
#else
		const AActor* ViewActor = SceneView->ViewActor;
#endif
		const bool bIsLockedToCamera = (ViewActor == OwnerActor);

		FCameraSystemEditorPreviewParams Params;
		Params.Canvas = Canvas;
		Params.SceneView = SceneView;
		Params.bIsLockedToCamera = bIsLockedToCamera;
		Params.bDrawWorldDebug = false;

		CameraSystemEvaluator->DrawEditorPreview(Params);
	}
}

void UGameplayCameraComponentBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	using namespace UE::Cameras;

	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraComponentBase, bRunInEditor))
	{
		AutoManageEditorPreviewEvaluator();
	}
	else if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGameplayCameraComponentBase, EditorPreviewCameraRigIndex))
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

