// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/BlueprintCameraDirector.h"

#include "Components/ActorComponent.h"
#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraDirectorEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigProxyAsset.h"
#include "Core/CameraRigProxyTable.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ControllerGameplayCameraEvaluationComponent.h"
#include "GameplayCameras.h"
#include "Services/AutoResetCameraVariableService.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraDirector)

#define LOCTEXT_NAMESPACE "BlueprintCameraDirector"

namespace UE::Cameras
{

void FBlueprintCameraDirectorEvaluationResult::Reset()
{
	ActiveCameraRigProxies.Reset();
	ActiveCameraRigs.Reset();
	ActivePersistentCameraRigs.Reset();
	InactivePersistentCameraRigs.Reset();
}

class FBlueprintCameraDirectorEvaluator : public FCameraDirectorEvaluator
{
	UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(GAMEPLAYCAMERAS_API, FBlueprintCameraDirectorEvaluator)

protected:

	virtual void OnInitialize(const FCameraDirectorInitializeParams& Params) override;
	virtual void OnActivate(const FCameraDirectorActivateParams& Params) override;
	virtual void OnDeactivate(const FCameraDirectorDeactivateParams& Params) override;
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;

private:

	void ActivateTransientCameraRigs(
			const FCameraDirectorEvaluationParams& Params, 
			const FBlueprintCameraDirectorEvaluationResult& BlueprintResult, 
			FCameraDirectorEvaluationResult& OutResult);
	void ActivateDeactivePersistentCameraRigs(
			TSharedPtr<FCameraEvaluationContext> EvaluationContext,
			const FBlueprintCameraDirectorEvaluationResult& BlueprintResult);

	const UCameraRigAsset* FindCameraRigByProxy(const UCameraRigProxyAsset* InProxy);

private:

	FCameraSystemEvaluator* OwningEvaluator = nullptr;

	TObjectPtr<UBlueprintCameraDirectorEvaluator> EvaluatorBlueprint;
};

UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(FBlueprintCameraDirectorEvaluator)

void FBlueprintCameraDirectorEvaluator::OnInitialize(const FCameraDirectorInitializeParams& Params)
{
	const UBlueprintCameraDirector* Blueprint = GetCameraDirectorAs<UBlueprintCameraDirector>();
	if (!ensure(Blueprint))
	{
		return;
	}

	const UCameraAsset* CameraAsset = Params.OwnerContext->GetCameraAsset();
	if (!ensure(CameraAsset))
	{
		return;
	}

	if (Blueprint->CameraDirectorEvaluatorClass)
	{
		UObject* Outer = Params.OwnerContext->GetOwner();
		EvaluatorBlueprint = NewObject<UBlueprintCameraDirectorEvaluator>(Outer, Blueprint->CameraDirectorEvaluatorClass);
	}
	else
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No Blueprint class set on camera director for '%s'."), *CameraAsset->GetPathName());
	}
}

void FBlueprintCameraDirectorEvaluator::OnActivate(const FCameraDirectorActivateParams& Params)
{
	OwningEvaluator = Params.Evaluator;

	if (EvaluatorBlueprint)
	{
		EvaluatorBlueprint->NativeActivateCameraDirector(Params);

		const FBlueprintCameraDirectorEvaluationResult& BlueprintResult = EvaluatorBlueprint->GetEvaluationResult();
		ActivateDeactivePersistentCameraRigs(Params.OwnerContext, BlueprintResult);
	}
	else
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't activate Blueprint camera director, no Blueprint class was set!"));
	}
}

void FBlueprintCameraDirectorEvaluator::OnDeactivate(const FCameraDirectorDeactivateParams& Params)
{
	if (EvaluatorBlueprint)
	{
		EvaluatorBlueprint->NativeDeactivateCameraDirector(Params);

		const FBlueprintCameraDirectorEvaluationResult& BlueprintResult = EvaluatorBlueprint->GetEvaluationResult();
		ActivateDeactivePersistentCameraRigs(Params.OwnerContext, BlueprintResult);
	}

	OwningEvaluator = nullptr;
}

void FBlueprintCameraDirectorEvaluator::OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult)
{
	if (EvaluatorBlueprint)
	{
		EvaluatorBlueprint->NativeRunCameraDirector(Params);

		const FBlueprintCameraDirectorEvaluationResult& BlueprintResult = EvaluatorBlueprint->GetEvaluationResult();
		ActivateTransientCameraRigs(Params, BlueprintResult, OutResult);
		ActivateDeactivePersistentCameraRigs(Params.OwnerContext, BlueprintResult);
	}
}

void FBlueprintCameraDirectorEvaluator::ActivateTransientCameraRigs(
		const FCameraDirectorEvaluationParams& Params, 
		const FBlueprintCameraDirectorEvaluationResult& BlueprintResult, 
		FCameraDirectorEvaluationResult& OutResult)
{
	TArray<const UCameraRigAsset*, TInlineAllocator<2>> CameraRigs;
	const UCameraAsset* CameraAsset = Params.OwnerContext->GetCameraAsset();

	// Gather camera rigs.
	for (const UCameraRigAsset* ActiveCameraRig : BlueprintResult.ActiveCameraRigs)
	{
		if (ActiveCameraRig)
		{
			CameraRigs.Add(ActiveCameraRig);
		}
		else
		{
			UE_LOG(
					LogCameraSystem, 
					Error, 
					TEXT("Null camera rig specified in camera director '%s'."),
					*EvaluatorBlueprint->GetClass()->GetPathName());
		}
	}

	// Resolve camera rig proxies.
	for (const UCameraRigProxyAsset* ActiveCameraRigProxy : BlueprintResult.ActiveCameraRigProxies)
	{
		const UCameraRigAsset* ActiveCameraRig = FindCameraRigByProxy(ActiveCameraRigProxy);
		if (ActiveCameraRig)
		{
			CameraRigs.Add(ActiveCameraRig);
		}
		else
		{
			UE_LOG(
					LogCameraSystem, 
					Error, 
					TEXT("No camera rig found mapped to proxy '%s' in camera '%s'."),
					*ActiveCameraRigProxy->GetPathName(), *CameraAsset->GetPathName());
		}
	}

	// The BP interface doesn't specify the evaluation context for the chosen camera rigs: we always automatically
	// make them run in our own owner context.
	for (const UCameraRigAsset* ActiveCameraRig : CameraRigs)
	{
		OutResult.Add(Params.OwnerContext, ActiveCameraRig);
	}
}

void FBlueprintCameraDirectorEvaluator::ActivateDeactivePersistentCameraRigs(
		TSharedPtr<FCameraEvaluationContext> EvaluationContext,
		const FBlueprintCameraDirectorEvaluationResult& BlueprintResult)
{
	if (BlueprintResult.InactivePersistentCameraRigs.IsEmpty() && BlueprintResult.ActivePersistentCameraRigs.IsEmpty())
	{
		return;
	}

	if (!ensure(OwningEvaluator))
	{
		return;
	}

	FRootCameraNodeEvaluator* RootNodeEvaluator = OwningEvaluator->GetRootNodeEvaluator();

	APlayerController* PlayerController = EvaluationContext->GetPlayerController();
	TSharedPtr<const FCameraEvaluationContext> ControllerEvaluationContext;
	if (ensure(PlayerController))
	{
		ControllerEvaluationContext = UControllerGameplayCameraEvaluationComponent::FindOrAddEvaluationContext(PlayerController);
	}

	for (const FBlueprintPersistentCameraRigInfo& CameraRigInfo : BlueprintResult.InactivePersistentCameraRigs)
	{
		FDeactivateCameraRigParams DeactivateParams;
		DeactivateParams.EvaluationContext = ControllerEvaluationContext;
		DeactivateParams.CameraRig = CameraRigInfo.CameraRig;
		DeactivateParams.Layer = CameraRigInfo.Layer;
		RootNodeEvaluator->DeactivateCameraRig(DeactivateParams);
	}

	for (const FBlueprintPersistentCameraRigInfo& CameraRigInfo : BlueprintResult.ActivePersistentCameraRigs)
	{
		FActivateCameraRigParams ActivateParams;
		ActivateParams.EvaluationContext = ControllerEvaluationContext;
		ActivateParams.CameraRig = CameraRigInfo.CameraRig;
		ActivateParams.Layer = CameraRigInfo.Layer;
		RootNodeEvaluator->ActivateCameraRig(ActivateParams);
	}
}

const UCameraRigAsset* FBlueprintCameraDirectorEvaluator::FindCameraRigByProxy(const UCameraRigProxyAsset* InProxy)
{
	const UBlueprintCameraDirector* Blueprint = GetCameraDirectorAs<UBlueprintCameraDirector>();
	if (!ensure(Blueprint))
	{
		return nullptr;
	}

	UCameraRigProxyTable* ProxyTable = Blueprint->CameraRigProxyTable;
	if (!ensureMsgf(ProxyTable, TEXT("No proxy table set on Blueprint director '%s'."), *Blueprint->GetPathName()))
	{
		return nullptr;
	}

	FCameraRigProxyTableResolveParams ResolveParams;
	ResolveParams.CameraRigProxy = InProxy;
	return Blueprint->CameraRigProxyTable->ResolveProxy(ResolveParams);
}

void FBlueprintCameraDirectorEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EvaluatorBlueprint);
}

}  // namespace UE::Cameras

void UBlueprintCameraDirectorEvaluator::ActivatePersistentBaseCameraRig(UCameraRigAsset* CameraRigPrefab)
{
	EvaluationResult.ActivePersistentCameraRigs.Add({ CameraRigPrefab, ECameraRigLayer::Base });
}

void UBlueprintCameraDirectorEvaluator::ActivatePersistentGlobalCameraRig(UCameraRigAsset* CameraRigPrefab)
{
	EvaluationResult.ActivePersistentCameraRigs.Add({ CameraRigPrefab, ECameraRigLayer::Global });
}

void UBlueprintCameraDirectorEvaluator::ActivatePersistentVisualCameraRig(UCameraRigAsset* CameraRigPrefab)
{
	EvaluationResult.ActivePersistentCameraRigs.Add({ CameraRigPrefab, ECameraRigLayer::Visual });
}

void UBlueprintCameraDirectorEvaluator::DeactivatePersistentBaseCameraRig(UCameraRigAsset* CameraRigPrefab)
{
	EvaluationResult.InactivePersistentCameraRigs.Add({ CameraRigPrefab, ECameraRigLayer::Base });
}

void UBlueprintCameraDirectorEvaluator::DeactivatePersistentGlobalCameraRig(UCameraRigAsset* CameraRigPrefab)
{
	EvaluationResult.InactivePersistentCameraRigs.Add({ CameraRigPrefab, ECameraRigLayer::Global });
}

void UBlueprintCameraDirectorEvaluator::DeactivatePersistentVisualCameraRig(UCameraRigAsset* CameraRigPrefab)
{
	EvaluationResult.InactivePersistentCameraRigs.Add({ CameraRigPrefab, ECameraRigLayer::Visual });
}

void UBlueprintCameraDirectorEvaluator::ActivateCameraRig(UCameraRigAsset* CameraRig)
{
	EvaluationResult.ActiveCameraRigs.Add(CameraRig);
}

void UBlueprintCameraDirectorEvaluator::ActivateCameraRigViaProxy(UCameraRigProxyAsset* CameraRigProxy)
{
	EvaluationResult.ActiveCameraRigProxies.Add(CameraRigProxy);
}

void UBlueprintCameraDirectorEvaluator::ActivateCameraRigPrefab(UCameraRigAsset* CameraRig)
{
	EvaluationResult.ActiveCameraRigs.Add(CameraRig);
}

UCameraRigAsset* UBlueprintCameraDirectorEvaluator::GetCameraRig(UCameraRigAsset* CameraRig) const
{
	// This function is only here to provide an easy way to pick a camera rig from the referencing 
	// camera asset, using the custom rig picker. Then we just return it.
	return CameraRig;
}

AActor* UBlueprintCameraDirectorEvaluator::FindEvaluationContextOwnerActor(TSubclassOf<AActor> ActorClass) const
{
	if (EvaluationContext)
	{
		if (UActorComponent* ContextOwnerAsComponent = Cast<UActorComponent>(EvaluationContext->GetOwner()))
		{
			return ContextOwnerAsComponent->GetOwner();
		}
		else if (AActor* ContextOwnerAsActor = Cast<AActor>(EvaluationContext->GetOwner()))
		{
			return ContextOwnerAsActor;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't access evaluation context outside of RunCameraDirector"), 
				ELogVerbosity::Error);
		return nullptr;
	}
}

FBlueprintCameraPose UBlueprintCameraDirectorEvaluator::GetInitialContextCameraPose() const
{
	if (EvaluationContext)
	{
		return FBlueprintCameraPose::FromCameraPose(EvaluationContext->GetInitialResult().CameraPose);
	}
	else
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't access evaluation context's initial result outside of RunCameraDirector"), 
				ELogVerbosity::Error);
		return FBlueprintCameraPose();
	}
}

FBlueprintCameraVariableTable UBlueprintCameraDirectorEvaluator::GetInitialContextVariableTable() const
{
	using namespace UE::Cameras;

	if (EvaluationContext)
	{
		FCameraVariableTable& VariableTable = EvaluationContext->GetInitialResult().VariableTable;
		return FBlueprintCameraVariableTable(&VariableTable, VariableAutoResetService);
	}
	else
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't access evaluation context's initial result outside of RunCameraDirector"), 
				ELogVerbosity::Error);
		return FBlueprintCameraVariableTable();
	}
}

void UBlueprintCameraDirectorEvaluator::SetInitialContextCameraPose(const FBlueprintCameraPose& InCameraPose)
{
	if (EvaluationContext)
	{
		InCameraPose.ApplyTo(EvaluationContext->GetInitialResult().CameraPose);
	}
	else
	{
		FFrame::KismetExecutionMessage(
				TEXT("Can't access evaluation context's initial result outside of RunCameraDirector"), 
				ELogVerbosity::Error);
	}
}

void UBlueprintCameraDirectorEvaluator::NativeActivateCameraDirector(const UE::Cameras::FCameraDirectorActivateParams& Params)
{
	using namespace UE::Cameras;

	EvaluationContext = Params.OwnerContext;
	VariableAutoResetService = Params.Evaluator->FindEvaluationService<FAutoResetCameraVariableService>();

	EvaluationResult.Reset();
	{
		FBlueprintCameraDirectorActivateParams BlueprintParams;
		if (Params.OwnerContext)
		{
			BlueprintParams.EvaluationContextOwner = Params.OwnerContext->GetOwner();
		}

		ActivateCameraDirector(BlueprintParams);
	}
}

void UBlueprintCameraDirectorEvaluator::NativeDeactivateCameraDirector(const UE::Cameras::FCameraDirectorDeactivateParams& Params)
{
	EvaluationResult.Reset();
	{
		FBlueprintCameraDirectorDeactivateParams BlueprintParams;
		if (Params.OwnerContext)
		{
			BlueprintParams.EvaluationContextOwner = Params.OwnerContext->GetOwner();
		}

		DeactivateCameraDirector(BlueprintParams);
	}

	VariableAutoResetService = nullptr;
	EvaluationContext = nullptr;
}

void UBlueprintCameraDirectorEvaluator::NativeRunCameraDirector(const UE::Cameras::FCameraDirectorEvaluationParams& Params)
{
	EvaluationResult.Reset();
	{
		FBlueprintCameraDirectorEvaluationParams BlueprintParams;
		BlueprintParams.DeltaTime = Params.DeltaTime;
		if (Params.OwnerContext)
		{
			BlueprintParams.EvaluationContextOwner = Params.OwnerContext->GetOwner();
		}

		RunCameraDirector(BlueprintParams);
	}
}

FCameraDirectorEvaluatorPtr UBlueprintCameraDirector::OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;

	return Builder.BuildEvaluator<FBlueprintCameraDirectorEvaluator>();
}

void UBlueprintCameraDirector::OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog)
{
	using namespace UE::Cameras;

	// Check that a camera director evaluator Blueprint was specified.
	if (!CameraDirectorEvaluatorClass)
	{
		BuildLog.AddMessage(EMessageSeverity::Error, LOCTEXT("MissingBlueprintClass", "No evaluator Blueprint class is set."));
		return;
	}
}

#if WITH_EDITOR

void UBlueprintCameraDirector::OnFactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams)
{
	if (!CameraRigProxyTable)
	{
		CameraRigProxyTable = NewObject<UCameraRigProxyTable>(this);
	}
}

#endif

#undef LOCTEXT_NAMESPACE

