// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/BlueprintCameraDirector.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraEvaluationContext.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraDirector)

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

	const UCameraRigAsset* FindCameraRigByName(const UCameraAsset* InCameraAsset, const FString& InCameraRigName);

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
	const UCameraAsset* CameraAsset = Params.OwnerContext->GetCameraAsset();
	if (EvaluatorBlueprint && CameraAsset)
	{
		FBlueprintCameraDirectorEvaluationParams BlueprintParams;
		BlueprintParams.DeltaTime = Params.DeltaTime;
		if (Params.OwnerContext)
		{
			BlueprintParams.EvaluationContextOwner = Params.OwnerContext->GetOwner();
		}

		FBlueprintCameraDirectorEvaluationResult BlueprintResult;

		EvaluatorBlueprint->NativeRunCameraDirector(BlueprintParams, BlueprintResult);

		// The BP interface doesn't specify the evaluation context for the chosen camera rigs: we always automatically
		// make them run in our own owner context.
		for (const FString& ActiveCameraRigName : BlueprintResult.ActiveCameraRigs)
		{
			const UCameraRigAsset* ActiveCameraRig = FindCameraRigByName(CameraAsset, ActiveCameraRigName);
			if (ActiveCameraRig)
			{
				OutResult.Add(Params.OwnerContext, ActiveCameraRig);
			}
			else
			{
				UE_LOG(
						LogCameraSystem, 
						Error, 
						TEXT("Can't activate camera rig '%s' because no camera rig of that name was found on '%s'."),
						*ActiveCameraRigName, *CameraAsset->GetPathName());
			}
		}
	}
	else
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't run Blueprint camera director, no Blueprint set, or no owner camera set!"));
	}
}

const UCameraRigAsset* FBlueprintCameraDirectorEvaluator::FindCameraRigByName(const UCameraAsset* InCameraAsset, const FString& InCameraRigName)
{
	for (const UCameraRigAsset* CameraRig : InCameraAsset->CameraRigs)
	{
		if (CameraRig->GetName() == InCameraRigName || CameraRig->Interface.DisplayName == InCameraRigName)
		{
			return CameraRig;
		}
	}
	return nullptr;
}

void FBlueprintCameraDirectorEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EvaluatorBlueprint);
}

}  // namespace UE::Cameras

void UBlueprintCameraDirectorEvaluator::ActivateCameraRig(const FString& InCameraRigName)
{
	CurrentResult.ActiveCameraRigs.Add(InCameraRigName);
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

