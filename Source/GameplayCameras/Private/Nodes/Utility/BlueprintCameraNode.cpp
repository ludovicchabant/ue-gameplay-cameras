// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Utility/BlueprintCameraNode.h"

#include "Components/ActorComponent.h"
#include "Core/CameraBuildLog.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraRigBuildContext.h"
#include "Core/CameraVariableTable.h"
#include "GameFramework/Actor.h"
#include "GameplayCameras.h"
#include "Templates/UnrealTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlueprintCameraNode)

#define LOCTEXT_NAMESPACE "BlueprintCameraNode"

namespace UE::Cameras
{

class FBlueprintCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FBlueprintCameraNodeEvaluator)

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;

private:

	TObjectPtr<UBlueprintCameraNodeEvaluator> EvaluatorBlueprint;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlueprintCameraNodeEvaluator)

void FBlueprintCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UBlueprintCameraNode* BlueprintNode = GetCameraNodeAs<UBlueprintCameraNode>();
	if (!ensure(BlueprintNode))
	{
		return;
	}

	if (BlueprintNode->CameraNodeEvaluatorClass)
	{
		UObject* Outer = Params.EvaluationContext->GetOwner();
		EvaluatorBlueprint = NewObject<UBlueprintCameraNodeEvaluator>(Outer, BlueprintNode->CameraNodeEvaluatorClass);
	}
	else
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No Blueprint class set on camera node '%s'."), *GetNameSafe(BlueprintNode));
	}
}

void FBlueprintCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	if (EvaluatorBlueprint)
	{
		EvaluatorBlueprint->NativeRunCameraNode(Params, OutResult);
	}
}

void FBlueprintCameraNodeEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EvaluatorBlueprint);
}

}  // namespace UE::Cameras

void UBlueprintCameraNodeEvaluator::NativeRunCameraNode(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	using namespace UE::Cameras;

	bIsFirstFrame = Params.bIsFirstFrame;
	EvaluationContextOwner = Params.EvaluationContext->GetOwner();
	CameraPose = FBlueprintCameraPose::FromCameraPose(OutResult.CameraPose);

	ensure(!CurrentContext.IsValid());
	TGuardValue<TSharedPtr<const FCameraEvaluationContext>> CurrentContextGuard(CurrentContext, Params.EvaluationContext);

	ensure(CurrentResult == nullptr);
	TGuardValue<FCameraNodeEvaluationResult*> CurrentResultGuard(CurrentResult, &OutResult);

	TickCameraNode(Params.DeltaTime);

	CameraPose.ApplyTo(OutResult.CameraPose);
}

AActor* UBlueprintCameraNodeEvaluator::FindEvaluationContextOwnerActor(TSubclassOf<AActor> ActorClass) const
{
	if (CurrentContext)
	{
		if (UActorComponent* ContextOwnerAsComponent = Cast<UActorComponent>(CurrentContext->GetOwner()))
		{
			return ContextOwnerAsComponent->GetOwner();
		}
		else if (AActor* ContextOwnerAsActor = Cast<AActor>(CurrentContext->GetOwner()))
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

void UBlueprintCameraNode::OnBuild(FCameraRigBuildContext& BuildContext)
{
	if (!CameraNodeEvaluatorClass)
	{
		BuildContext.BuildLog.AddMessage(
				EMessageSeverity::Error, this,
				LOCTEXT("MissingBlueprintClass", "No evaluator Blueprint class is set."));
	}
}

FCameraNodeEvaluatorPtr UBlueprintCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FBlueprintCameraNodeEvaluator>();
}

#undef LOCTEXT_NAMESPACE

