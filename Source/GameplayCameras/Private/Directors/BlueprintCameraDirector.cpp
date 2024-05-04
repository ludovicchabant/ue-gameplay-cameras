// Copyright Epic Games, Inc. All Rights Reserved.

#include "Directors/BlueprintCameraDirector.h"

#include "Algo/StableSort.h"
#include "Core/CameraEvaluationContext.h"

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

	TObjectPtr<UBlueprintCameraDirectorEvaluator> EvaluatorBlueprint;
};

UE_DEFINE_CAMERA_DIRECTOR_EVALUATOR(FBlueprintCameraDirectorEvaluator)

void FBlueprintCameraDirectorEvaluator::OnInitialize(const FCameraDirectorInitializeParams& Params)
{
	const UBlueprintCameraDirector* Blueprint = GetCameraDirectorAs<UBlueprintCameraDirector>();
	if (!ensure(Blueprint && Blueprint->CameraDirectorEvaluatorClass))
	{
		return;
	}

	UObject* Outer = Params.OwnerContext->GetOwner();
	EvaluatorBlueprint = NewObject<UBlueprintCameraDirectorEvaluator>(Outer, Blueprint->CameraDirectorEvaluatorClass);
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

		for (UCameraRigAsset* ActiveCameraRig : BlueprintResult.ActiveCameraRigs)
		{
			OutResult.Add(Params.OwnerContext, ActiveCameraRig);
		}
	}
}

void FBlueprintCameraDirectorEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EvaluatorBlueprint);
}

}  // namespace UE::Cameras

void UBlueprintCameraDirectorEvaluator::NativeRunCameraDirector(const FBlueprintCameraDirectorEvaluationParams& Params, FBlueprintCameraDirectorEvaluationResult& OutResult)
{
	RunCameraDirector(Params, OutResult.ActiveCameraRigs);
}

FCameraDirectorEvaluatorPtr UBlueprintCameraDirector::OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;

	return Builder.BuildEvaluator<FBlueprintCameraDirectorEvaluator>();
}

