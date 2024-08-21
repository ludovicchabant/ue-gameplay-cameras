// Copyright Epic Games, Inc. All Rights Reserved.

#include "Builders/BlueprintCameraDirectorEditorBuilder.h"

#include "Core/CameraAsset.h"
#include "Core/CameraBuildLog.h"
#include "Directors/BlueprintCameraDirector.h"
#include "EdGraph/EdGraph.h"
#include "Engine/Blueprint.h"
#include "Helpers/CameraAssetReferenceGatherer.h"
#include "K2Node_CallFunction.h"

#define LOCTEXT_NAMESPACE "BlueprintCameraDirectorEditorBuilder"

namespace UE::Cameras
{

void FBlueprintCameraDirectorEditorBuilder::OnBuildCameraAsset(UCameraAsset* CameraAsset, UE::Cameras::FCameraBuildLog& BuildLog)
{
	UBlueprintCameraDirector* BlueprintCameraDirector = Cast<UBlueprintCameraDirector>(CameraAsset->GetCameraDirector());
	if (!BlueprintCameraDirector)
	{
		// Skip other camera directors.
		return;
	}

	TSubclassOf<UBlueprintCameraDirectorEvaluator> CameraDirectorEvaluatorClass = BlueprintCameraDirector->CameraDirectorEvaluatorClass;
	if (!CameraDirectorEvaluatorClass)
	{
		// This case is already checked by the Blueprint camera director itself.
		return;
	}

	// If the evaluator Blueprint is shared with other camera assets, check that it only uses
	// camera rig proxies for activation.
	TArray<UCameraAsset*> ReferencingCameraAssets;
	UBlueprint* Blueprint = CastChecked<UBlueprint>(CameraDirectorEvaluatorClass->ClassGeneratedBy);
	FCameraAssetReferenceGatherer::GetReferencingCameraAssets(Blueprint, ReferencingCameraAssets);

	UCameraAsset* ThisCameraAsset = BlueprintCameraDirector->GetTypedOuter<UCameraAsset>();
	ensure(ThisCameraAsset);
	ReferencingCameraAssets.Remove(ThisCameraAsset);

	if (ReferencingCameraAssets.Num() > 0)
	{
		const bool bUsesDirectRigActivation = UsesDirectCameraRigActivation(Blueprint);
		if (bUsesDirectRigActivation)
		{
			BuildLog.AddMessage(
					EMessageSeverity::Error, 
					BlueprintCameraDirector,
					FText::Format(LOCTEXT("SharedDirectorMustUseProxies",
						"Blueprint camera director evaluator '{0}' is shared with {1} other camera assets, but uses "
						"ActivateCameraRig nodes. Shared Blueprints must use ActivateCameraRigViaProxy to work between "
						"multiple camera assets."),
						{
							FText::FromString(CameraDirectorEvaluatorClass->GetOutermost()->GetName()),
							ReferencingCameraAssets.Num() }));
		}
	}
}

bool FBlueprintCameraDirectorEditorBuilder::UsesDirectCameraRigActivation(UBlueprint* Blueprint)
{
	TArray<UEdGraph*> BlueprintGraphs;
	Blueprint->GetAllGraphs(BlueprintGraphs);
	for (const UEdGraph* BlueprintGraph : BlueprintGraphs)
	{
		TArray<UK2Node_CallFunction*> CallFunctionNodes;
		BlueprintGraph->GetNodesOfClass(CallFunctionNodes);
		for (UK2Node_CallFunction* CallFunctionNode : CallFunctionNodes)
		{
			// Look for calls of `this->ActivateCameraRig()`.
			const FName FunctionName = CallFunctionNode->FunctionReference.GetMemberName();
			if (CallFunctionNode->FunctionReference.IsSelfContext()
					&& FunctionName == GET_FUNCTION_NAME_CHECKED(UBlueprintCameraDirectorEvaluator, ActivateCameraRig))
			{
				return true;
			}
		}
	}
	return false;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

