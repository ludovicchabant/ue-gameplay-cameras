// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/StateTreeCameraDirector.h"

#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigProxyAsset.h"
#include "Core/CameraRigProxyTable.h"
#include "Directors/CameraDirectorStateTreeSchema.h"
#include "GameplayCameras.h"
#include "Logging/TokenizedMessage.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeInstanceData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StateTreeCameraDirector)

#define LOCTEXT_NAMESPACE "StateTreeCameraDirector"

namespace UE::Cameras
{

class FStateTreeCameraDirectorEvaluator : public FCameraDirectorEvaluator
{
	UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(GAMEPLAYCAMERAS_API, FStateTreeCameraDirectorEvaluator)

public:

protected:

	// FCameraDirectorEvaluator interface.
	virtual void OnActivate(const FCameraDirectorActivateParams& Params) override;
	virtual void OnDeactivate(const FCameraDirectorDeactivateParams& Params) override;
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;

private:

	bool SetContextRequirements(TSharedPtr<const FCameraEvaluationContext> OwnerContext, FStateTreeExecutionContext& StateTreeContext);
	const UCameraRigAsset* FindCameraRigByProxy(const UCameraRigProxyAsset* InProxy);

private:

	FStateTreeInstanceData StateTreeInstanceData;

	FCameraDirectorStateTreeEvaluationData EvaluationData;
};

UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(FStateTreeCameraDirectorEvaluator)

void FStateTreeCameraDirectorEvaluator::OnActivate(const FCameraDirectorActivateParams& Params)
{
	const UStateTreeCameraDirector* StateTreeDirector = GetCameraDirectorAs<UStateTreeCameraDirector>();
	const FStateTreeReference& StateTreeReference = StateTreeDirector->StateTreeReference;
	const UStateTree* StateTree = StateTreeReference.GetStateTree();

	if (!StateTree)
	{
		UE_LOG(LogCameraSystem, Error,
			TEXT("Can't activate camera director '%s': it doesn't have a valid StateTree asset specified."),
			*GetNameSafe(StateTreeDirector));
		return;
	}

	UObject* ContextOwner = Params.OwnerContext->GetOwner();
	if (!ContextOwner)
	{
		UE_LOG(LogCameraSystem, Error,
			TEXT("Can't activate camera director '%s': the evaluation context doesn't have a valid owner."),
			*GetNameSafe(StateTreeDirector));
		return;
	}

	FStateTreeExecutionContext StateTreeContext(*ContextOwner, *StateTree, StateTreeInstanceData);

	if (!StateTreeContext.IsValid())
	{
		UE_LOG(LogCameraSystem, Error,
			TEXT("Can't activate camera director '%s': initialization of execution context for StateTree asset '%s' "
				"and context owner '%s' failed."),
			*GetNameSafe(StateTreeDirector), *GetNameSafe(StateTree), *GetNameSafe(ContextOwner));
		return;
	}

	// TODO: validate schema.
	
	if (!SetContextRequirements(Params.OwnerContext, StateTreeContext))
	{
		UE_LOG(LogCameraSystem, Error,
			TEXT("Can't activate camera director '%s': failed to setup external data views for StateTree asset '%s'."),
			*GetNameSafe(StateTreeDirector), *GetNameSafe(StateTree));
		return;
	}

	StateTreeContext.Start(&StateTreeReference.GetParameters());
}

void FStateTreeCameraDirectorEvaluator::OnDeactivate(const FCameraDirectorDeactivateParams& Params)
{
	const UStateTreeCameraDirector* StateTreeDirector = GetCameraDirectorAs<UStateTreeCameraDirector>();
	const FStateTreeReference& StateTreeReference = StateTreeDirector->StateTreeReference;
	const UStateTree* StateTree = StateTreeReference.GetStateTree();

	UObject* ContextOwner = Params.OwnerContext->GetOwner();
	if (!ContextOwner)
	{
		UE_LOG(LogCameraSystem, Error,
			TEXT("Can't deactivate camera director '%s': the evaluation context doesn't have a valid owner."),
			*GetNameSafe(StateTreeDirector));
		return;
	}

	if (!StateTree)
	{
		UE_LOG(LogCameraSystem, Error,
			TEXT("Can't deactivate camera director '%s': it doesn't have a valid StateTree asset specified."),
			*GetNameSafe(StateTreeDirector));
		return;
	}
	
	FStateTreeExecutionContext StateTreeContext(*ContextOwner, *StateTree, StateTreeInstanceData);

	if (SetContextRequirements(Params.OwnerContext, StateTreeContext))
	{
		StateTreeContext.Stop();
	}
}

void FStateTreeCameraDirectorEvaluator::OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult)
{
	const UStateTreeCameraDirector* StateTreeDirector = GetCameraDirectorAs<UStateTreeCameraDirector>();
	const FStateTreeReference& StateTreeReference = StateTreeDirector->StateTreeReference;
	const UStateTree* StateTree = StateTreeReference.GetStateTree();

	UObject* ContextOwner = Params.OwnerContext->GetOwner();

	if (!StateTree || !ContextOwner)
	{
		// Fail silently... we already emitted errors during OnActivate.
		return;
	}
	
	FStateTreeExecutionContext StateTreeContext(*ContextOwner, *StateTree, StateTreeInstanceData);

	if (SetContextRequirements(Params.OwnerContext, StateTreeContext))
	{
		StateTreeContext.Tick(Params.DeltaTime);

		TArray<const UCameraRigAsset*, TInlineAllocator<2>> CameraRigs;
		const UCameraAsset* CameraAsset = Params.OwnerContext->GetCameraAsset();

		// Gather camera rigs.
		for (const UCameraRigAsset* ActiveCameraRig : EvaluationData.ActiveCameraRigs)
		{
			if (ActiveCameraRig)
			{
				CameraRigs.Add(ActiveCameraRig);
			}
			else
			{
				UE_LOG(LogCameraSystem, Error, TEXT("Null camera rig specified in camera director '%s'."), 
						*StateTree->GetPathName());
			}
		}

		// Resolve camera rig proxies.
		for (const UCameraRigProxyAsset* ActiveCameraRigProxy : EvaluationData.ActiveCameraRigProxies)
		{
			const UCameraRigAsset* ActiveCameraRig = FindCameraRigByProxy(ActiveCameraRigProxy);
			if (ActiveCameraRig)
			{
				CameraRigs.Add(ActiveCameraRig);
			}
			else
			{
				UE_LOG(LogCameraSystem, Error, TEXT("No camera rig found mapped to proxy '%s' in camera '%s'."),
						*ActiveCameraRigProxy->GetPathName(), *CameraAsset->GetPathName());
			}
		}

		// Set all collected camera rigs as our active rigs this frame.
		for (const UCameraRigAsset* CameraRig : CameraRigs)
		{
			OutResult.Add(Params.OwnerContext, CameraRig);
		}
	}
}

bool FStateTreeCameraDirectorEvaluator::SetContextRequirements(TSharedPtr<const FCameraEvaluationContext> OwnerContext, FStateTreeExecutionContext& StateTreeContext)
{
	UObject* ContextOwner = OwnerContext->GetOwner();
	StateTreeContext.SetContextDataByName(
			FStateTreeContextDataNames::ContextOwner, 
			FStateTreeDataView(ContextOwner));

	EvaluationData.Reset();

	StateTreeContext.SetCollectExternalDataCallback(FOnCollectStateTreeExternalData::CreateLambda(
		[this](const FStateTreeExecutionContext& Context, const UStateTree* StateTree, 
			TArrayView<const FStateTreeExternalDataDesc> ExternalDescs, TArrayView<FStateTreeDataView> OutDataViews)
		{
			for (int32 Index = 0; Index < ExternalDescs.Num(); Index++)
			{
				const FStateTreeExternalDataDesc& ExternalDesc(ExternalDescs[Index]);
				if (ExternalDesc.Struct == FCameraDirectorStateTreeEvaluationData::StaticStruct())
				{
					OutDataViews[Index] = FStateTreeDataView(FStructView::Make(EvaluationData));
				}
			}
			return true;
		}));

	return true;
}

const UCameraRigAsset* FStateTreeCameraDirectorEvaluator::FindCameraRigByProxy(const UCameraRigProxyAsset* InProxy)
{
	const UStateTreeCameraDirector* Director = GetCameraDirectorAs<UStateTreeCameraDirector>();
	if (!ensure(Director))
	{
		return nullptr;
	}

	const UCameraRigProxyTable* ProxyTable = Director->CameraRigProxyTable;
	if (!ensureMsgf(ProxyTable, TEXT("No proxy table set on StateTree director '%s'."), *Director->GetPathName()))
	{
		return nullptr;
	}

	FCameraRigProxyTableResolveParams ResolveParams;
	ResolveParams.CameraRigProxy = InProxy;
	return Director->CameraRigProxyTable->ResolveProxy(ResolveParams);
}

void FStateTreeCameraDirectorEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	StateTreeInstanceData.AddStructReferencedObjects(Collector);
}

}  // namespace UE::Cameras

UStateTreeCameraDirector::UStateTreeCameraDirector(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

FCameraDirectorEvaluatorPtr UStateTreeCameraDirector::OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FStateTreeCameraDirectorEvaluator>();
}

void UStateTreeCameraDirector::OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog)
{
	using namespace UE::Cameras;

	// Check that a state tree was specified.
	if (!StateTreeReference.IsValid())
	{
		BuildLog.AddMessage(EMessageSeverity::Error, LOCTEXT("MissingStateTree", "No state tree reference is set."));
		return;
	}
}

#if WITH_EDITOR

void UStateTreeCameraDirector::OnFactoryCreateAsset(const FCameraDirectorFactoryCreateParams& InParams)
{
	if (!CameraRigProxyTable)
	{
		CameraRigProxyTable = NewObject<UCameraRigProxyTable>(this);
	}
}

#endif

#undef LOCTEXT_NAMESPACE

