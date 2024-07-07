// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/BlueprintCameraDirector.h"

#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigProxyAsset.h"
#include "Core/CameraRigProxyTable.h"
#include "Core/CameraEvaluationContext.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraDirector)

#define LOCTEXT_NAMESPACE "BlueprintCameraDirector"

namespace UE::Cameras
{

class FBlueprintCameraDirectorEvaluator : public FCameraDirectorEvaluator
{
	UE_DECLARE_CAMERA_DIRECTOR_EVALUATOR(GAMEPLAYCAMERAS_API, FBlueprintCameraDirectorEvaluator)

protected:

	virtual void OnInitialize(const FCameraDirectorInitializeParams& Params) override;
	virtual void OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;

private:

	const UCameraRigAsset* FindCameraRigByProxy(const UCameraRigProxyAsset* InProxy);

private:

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

void FBlueprintCameraDirectorEvaluator::OnRun(const FCameraDirectorEvaluationParams& Params, FCameraDirectorEvaluationResult& OutResult)
{
	if (EvaluatorBlueprint)
	{
		FBlueprintCameraDirectorEvaluationParams BlueprintParams;
		BlueprintParams.DeltaTime = Params.DeltaTime;
		if (Params.OwnerContext)
		{
			BlueprintParams.EvaluationContextOwner = Params.OwnerContext->GetOwner();
		}

		FBlueprintCameraDirectorEvaluationResult BlueprintResult;

		EvaluatorBlueprint->NativeRunCameraDirector(BlueprintParams, BlueprintResult);

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
	else
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't run Blueprint camera director, no Blueprint class was set!"));
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

void UBlueprintCameraDirectorEvaluator::ActivateCameraRig(UCameraRigAsset* CameraRig)
{
	CurrentResult.ActiveCameraRigs.Add(CameraRig);
}

void UBlueprintCameraDirectorEvaluator::ActivateCameraRigViaProxy(UCameraRigProxyAsset* CameraRigProxy)
{
	CurrentResult.ActiveCameraRigProxies.Add(CameraRigProxy);
}

void UBlueprintCameraDirectorEvaluator::NativeRunCameraDirector(const FBlueprintCameraDirectorEvaluationParams& Params, FBlueprintCameraDirectorEvaluationResult& OutResult)
{
	CurrentResult = OutResult;
	{
		// Run the Blueprint logic.
		RunCameraDirector(Params);
	}
	OutResult = CurrentResult;
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

