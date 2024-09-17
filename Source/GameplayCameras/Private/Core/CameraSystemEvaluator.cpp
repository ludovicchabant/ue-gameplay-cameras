// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraSystemEvaluator.h"

#include "Algo/Transform.h"
#include "Camera/CameraTypes.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationService.h"
#include "Core/CameraRigCombinationRegistry.h"
#include "Core/DefaultRootCameraNode.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraSystemTrace.h"
#include "Debug/RootCameraDebugBlock.h"
#include "GameplayCamerasSettings.h"
#include "Services/AutoResetCameraVariableService.h"
#include "Services/OrientationInitializationService.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

DECLARE_CYCLE_STAT(TEXT("Camera System Eval"), CameraSystemEval_Total, STATGROUP_CameraSystem);

namespace UE::Cameras
{

extern bool GGameplayCamerasDebugEnable;

void FCameraSystemEvaluationResult::Reset()
{
	CameraPose.ClearAllChangedFlags();
	VariableTable.ClearAllWrittenThisFrameFlags();
	bIsCameraCut = false;
	bIsValid = false;
}

void FCameraSystemEvaluationResult::Reset(const FCameraNodeEvaluationResult& NodeResult)
{
	Reset();

	// Make the camera poses actually equal, so that we get the exact same changed-flags.
	CameraPose = NodeResult.CameraPose;

	VariableTable.OverrideAll(NodeResult.VariableTable);

	bIsCameraCut = NodeResult.bIsCameraCut;
	bIsValid = true;
}

FCameraSystemEvaluator::FCameraSystemEvaluator()
{
}

void FCameraSystemEvaluator::Initialize(TObjectPtr<UObject> InOwner)
{
	FCameraSystemEvaluatorCreateParams Params;
	Params.Owner = InOwner;
	Initialize(Params);
}

void FCameraSystemEvaluator::Initialize(const FCameraSystemEvaluatorCreateParams& Params)
{
	UObject* Owner = Params.Owner;
	if (!Owner)
	{
		Owner = GetTransientPackage();
	}
	WeakOwner = Owner;

	if (Params.RootNodeFactory)
	{
		RootNode = Params.RootNodeFactory();
	}
	else
	{
		RootNode = NewObject<UDefaultRootCameraNode>(Owner, TEXT("RootNode"));
	}

	ContextStack.Initialize(*this);

	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = RootNode;
	RootEvaluator = static_cast<FRootCameraNodeEvaluator*>(RootEvaluatorStorage.BuildEvaluatorTree(BuildParams));

	if (ensure(RootEvaluator))
	{
		FCameraNodeEvaluatorInitializeParams InitParams;
		InitParams.Evaluator = this;
		RootEvaluator->Initialize(InitParams, RootNodeResult);
	}

	VariableAutoResetService = MakeShared<FAutoResetCameraVariableService>();
	RegisterEvaluationService(VariableAutoResetService.ToSharedRef());
	RegisterEvaluationService(MakeShared<FOrientationInitializationService>());

	CameraRigCombinationRegistry = MakeShared<FCameraRigCombinationRegistry>();
}

FCameraSystemEvaluator::~FCameraSystemEvaluator()
{
	ContextStack.Reset();

	{
		FCameraEvaluationServiceTeardownParams TeardownParams;
		TeardownParams.Evaluator = this;
		for (TSharedPtr<FCameraEvaluationService> EvaluationService : EvaluationServices)
		{
			EvaluationService->Teardown(TeardownParams);
		}
		EvaluationServices.Reset();
	}
}

void FCameraSystemEvaluator::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(RootNode);
	ContextStack.AddReferencedObjects(Collector);
	if (RootEvaluator)
	{
		RootEvaluator->AddReferencedObjects(Collector);
	}
	for (TSharedPtr<FCameraEvaluationService> EvaluationService : EvaluationServices)
	{
		EvaluationService->AddReferencedObjects(Collector);
	}
	if (CameraRigCombinationRegistry)
	{
		CameraRigCombinationRegistry->AddReferencedObjects(Collector);
	}
}

void FCameraSystemEvaluator::PushEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext)
{
	ContextStack.PushContext(EvaluationContext);
}

void FCameraSystemEvaluator::RemoveEvaluationContext(TSharedRef<FCameraEvaluationContext> EvaluationContext)
{
	ContextStack.RemoveContext(EvaluationContext);
}

void FCameraSystemEvaluator::PopEvaluationContext()
{
	ContextStack.PopContext();
}

void FCameraSystemEvaluator::RegisterEvaluationService(TSharedRef<FCameraEvaluationService> EvaluationService)
{
	EvaluationServices.Add(EvaluationService);
	{
		FCameraEvaluationServiceInitializeParams InitParams;
		InitParams.Evaluator = this;
		EvaluationService->Initialize(InitParams);
	}
}

void FCameraSystemEvaluator::UnregisterEvaluationService(TSharedRef<FCameraEvaluationService> EvaluationService)
{
	{
		FCameraEvaluationServiceTeardownParams TeardownParams;
		TeardownParams.Evaluator = this;
		EvaluationService->Teardown(TeardownParams);
	}
	EvaluationServices.Remove(EvaluationService);
}

void FCameraSystemEvaluator::GetEvaluationServices(TArray<TSharedPtr<FCameraEvaluationService>>& OutEvaluationServices) const
{
	OutEvaluationServices = EvaluationServices;
}

TSharedPtr<FCameraEvaluationService> FCameraSystemEvaluator::FindEvaluationService(const FCameraObjectTypeID& TypeID) const
{
	for (TSharedPtr<FCameraEvaluationService> EvaluationService : EvaluationServices)
	{
		if (EvaluationService.Get()->IsKindOf(TypeID))
		{
			return EvaluationService;
		}
	}
	return nullptr;
}

void FCameraSystemEvaluator::NotifyRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent)
{
	for (TSharedPtr<FCameraEvaluationService> EvaluationService : EvaluationServices)
	{
		if (EvaluationService->HasAllEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsRootCameraNodeEvents))
		{
			EvaluationService->NotifyRootCameraNodeEvent(InEvent);
		}
	}
}

void FCameraSystemEvaluator::Update(const FCameraSystemEvaluationParams& Params)
{
	SCOPE_CYCLE_COUNTER(CameraSystemEval_Total);

	// Reset our result' flags.
	RootNodeResult.CameraPose.ClearAllChangedFlags();
	RootNodeResult.VariableTable.ClearAllWrittenThisFrameFlags();

	// Run the variable auto-reset service here, because the other (third party) services
	// should get the reset variable values.
	if (VariableAutoResetService)
	{
		VariableAutoResetService->PerformVariableResets(RootNodeResult.VariableTable, ContextStack);
	}

	// Pre-update all services.
	PreUpdateServices(Params.DeltaTime, ECameraEvaluationServiceFlags::None);

	// Get the active evaluation context.
	TSharedPtr<FCameraEvaluationContext> ActiveContext = ContextStack.GetActiveContext();
	if (UNLIKELY(!ActiveContext.IsValid()))
	{
		Result.bIsValid = false;
		return;
	}

	// Run the camera director, and activate any camera rig(s) it returns to us.
	FCameraDirectorEvaluator* ActiveDirectorEvaluator = ActiveContext->GetDirectorEvaluator();
	if (ActiveDirectorEvaluator)
	{
		FCameraDirectorEvaluationParams DirectorParams;
		DirectorParams.DeltaTime = Params.DeltaTime;
		DirectorParams.OwnerContext = ActiveContext;

		FCameraDirectorEvaluationResult DirectorResult;

		ActiveDirectorEvaluator->Run(DirectorParams, DirectorResult);

		if (DirectorResult.ActiveCameraRigs.Num() == 1)
		{
			// Only one camera rig to activate... let's do that.
			const FActiveCameraRigInfo& ActiveCameraRig = DirectorResult.ActiveCameraRigs[0];

			FActivateCameraRigParams CameraRigParams;
			CameraRigParams.EvaluationContext = ActiveCameraRig.EvaluationContext;
			CameraRigParams.CameraRig = ActiveCameraRig.CameraRig;
			RootEvaluator->ActivateCameraRig(CameraRigParams);
		}
		else if (DirectorResult.ActiveCameraRigs.Num() > 1)
		{
			// We have a combination of camera rigs to activate. Let's dynamically generate a new camera rig
			// asset that combines them.
#if WITH_EDITOR
			const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
			if (DirectorResult.ActiveCameraRigs.Num() > Settings->CombinedCameraRigNumThreshold)
			{
				UE_LOG(LogCameraSystem, Warning, 
						TEXT("Activating %d camera rigs combined! Is the camera director doing this on purpose? "
							"If so, raise the CombinedCameraRigNumThreshold setting to remove this warning."),
						DirectorResult.ActiveCameraRigs.Num());
			}
#endif

			// All combined camera rigs must belong to the same evaluation context.
			TArray<const UCameraRigAsset*> Combination;
			TSharedPtr<const FCameraEvaluationContext> CommonContext = DirectorResult.ActiveCameraRigs[0].EvaluationContext;
			for (const FActiveCameraRigInfo& ActiveCameraRig : DirectorResult.ActiveCameraRigs)
			{
				Combination.Add(ActiveCameraRig.CameraRig);
				ensureMsgf(ActiveCameraRig.EvaluationContext == CommonContext,
						TEXT("All combined camera rigs must be activated from the same evaluation context."));
			}
			const UCameraRigAsset* CombinedCameraRig = CameraRigCombinationRegistry->FindOrCreateCombination(Combination);

			FActivateCameraRigParams CameraRigParams;
			CameraRigParams.EvaluationContext = CommonContext;
			CameraRigParams.CameraRig = CombinedCameraRig;
			RootEvaluator->ActivateCameraRig(CameraRigParams);
		}
	}

	{
		// Setup the params/result for running the root camera node.
		FCameraNodeEvaluationParams NodeParams;
		NodeParams.Evaluator = this;
		NodeParams.DeltaTime = Params.DeltaTime;

		RootNodeResult.Reset();

		// Run the root camera node.
		RootEvaluator->Run(NodeParams, RootNodeResult);

		RootNodeResult.bIsValid = true;
	}

	// Post-update all services.
	PostUpdateServices(Params.DeltaTime, ECameraEvaluationServiceFlags::None);

	// Harvest the result.
	Result.Reset(RootNodeResult);

	// End of update things...
	ContextStack.OnEndCameraSystemUpdate();
}

void FCameraSystemEvaluator::PreUpdateServices(float DeltaTime, ECameraEvaluationServiceFlags ExtraFlags)
{
	FCameraEvaluationServiceUpdateParams ServiceUpdateParams;
	ServiceUpdateParams.Evaluator = this;
	ServiceUpdateParams.DeltaTime = DeltaTime;

	FCameraEvaluationServiceUpdateResult ServiceUpdateResult(RootNodeResult);

	for (TSharedPtr<FCameraEvaluationService> EvaluationService : EvaluationServices)
	{
		if (EvaluationService->HasAllEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsPreUpdate | ExtraFlags))
		{
			EvaluationService->PreUpdate(ServiceUpdateParams, ServiceUpdateResult);
		}
	}
}

void FCameraSystemEvaluator::PostUpdateServices(float DeltaTime, ECameraEvaluationServiceFlags ExtraFlags)
{
	FCameraEvaluationServiceUpdateParams ServiceUpdateParams;
	ServiceUpdateParams.Evaluator = this;
	ServiceUpdateParams.DeltaTime = DeltaTime;

	FCameraEvaluationServiceUpdateResult ServiceUpdateResult(RootNodeResult);

	for (TSharedPtr<FCameraEvaluationService> EvaluationService : EvaluationServices)
	{
		if (EvaluationService->HasAllEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsPostUpdate | ExtraFlags))
		{
			EvaluationService->PostUpdate(ServiceUpdateParams, ServiceUpdateResult);
		}
	}
}

void FCameraSystemEvaluator::GetEvaluatedCameraView(FMinimalViewInfo& DesiredView)
{
	const FCameraPose& CameraPose = RootNodeResult.CameraPose;
	DesiredView.Location = CameraPose.GetLocation();
	DesiredView.Rotation = CameraPose.GetRotation();
	DesiredView.FOV = CameraPose.GetEffectiveFieldOfView();

	DesiredView.AspectRatio = CameraPose.GetSensorAspectRatio();
	DesiredView.bConstrainAspectRatio = CameraPose.GetConstrainAspectRatio();
	DesiredView.AspectRatioAxisConstraint = CameraPose.GetOverrideAspectRatioAxisConstraint() ?
		CameraPose.GetAspectRatioAxisConstraint() : TOptional<EAspectRatioAxisConstraint>();

	// TODO: add support for ortho cameras.
	DesiredView.PerspectiveNearClipPlane = CameraPose.GetNearClippingPlane();

	const FPostProcessSettingsCollection& PostProcessSettings = RootNodeResult.PostProcessSettings;
	DesiredView.PostProcessSettings = PostProcessSettings.Get();
	DesiredView.PostProcessBlendWeight = 1.f;
	// Create the physical camera settings if needed. Don't overwrite settings that were set by hand.
	CameraPose.ApplyPhysicalCameraSettings(DesiredView.PostProcessSettings, false);
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FCameraSystemEvaluator::DebugUpdate(const FCameraSystemDebugUpdateParams& Params)
{
#if UE_GAMEPLAY_CAMERAS_TRACE
	const bool bTraceEnabled = FCameraSystemTrace::IsTraceEnabled();
#else
	const bool bTraceEnabled = false;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
	if (!bTraceEnabled && !GGameplayCamerasDebugEnable)
	{
		return;
	}

#if UE_GAMEPLAY_CAMERAS_TRACE
	if (FCameraSystemTrace::IsTraceReplay())
	{
		return;
	}
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

	// Clear previous frame's debug info and make room for this frame's.
	DebugBlockStorage.DestroyDebugBlocks();

	// Create the root debug block and start building more.
	RootDebugBlock = DebugBlockStorage.BuildDebugBlock<FRootCameraDebugBlock>();

	FCameraDebugBlockBuildParams BuildParams;
	FCameraDebugBlockBuilder DebugBlockBuilder(DebugBlockStorage, *RootDebugBlock);
	RootDebugBlock->BuildDebugBlocks(*this, BuildParams, DebugBlockBuilder);

	UObject* Owner = WeakOwner.Get();
	UWorld* OwnerWorld = Owner ? Owner->GetWorld() : nullptr;

#if UE_GAMEPLAY_CAMERAS_TRACE
	if (bTraceEnabled)
	{
		FCameraSystemTrace::TraceEvaluation(OwnerWorld, Result, *RootDebugBlock);
	}
#endif
	
	FCameraDebugRenderer Renderer(OwnerWorld, Params.CanvasObject);
	RootDebugBlock->RootDebugDraw(Renderer);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

