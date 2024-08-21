// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SBlueprintCameraDirectorRigNameGraphPin.h"

#include "Core/CameraAsset.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Helpers/CameraAssetReferenceGatherer.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "SBlueprintCameraDirectorRigNameGraphPin"

namespace UE::Cameras
{

void SBlueprintCameraDirectorRigNameGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SCameraRigNameGraphPin::Construct(
			SCameraRigNameGraphPin::FArguments(),
			InGraphPinObj);
}

void SBlueprintCameraDirectorRigNameGraphPin::OnCustomizeCameraRigPickerConfig(FCameraRigPickerConfig& CameraRigPickerConfig) const
{
	TSharedPtr<SGraphNode> OwnerNodeWidget = OwnerNodePtr.Pin();
	check(OwnerNodeWidget);
	
	UEdGraphNode* OwnerNode = OwnerNodeWidget->GetNodeObj();
	UBlueprint* OwnerBlueprint = FBlueprintEditorUtils::FindBlueprintForNode(OwnerNode);

	TArray<UCameraAsset*> ReferencingCameraAssets;
	FCameraAssetReferenceGatherer::GetReferencingCameraAssets(OwnerBlueprint, ReferencingCameraAssets);
	
	// Don't let the user choose which camera asset to activate rigs from. Automatically use the camera asset
	// that references this Blueprint camera director. Display warnings if we have zero referencers, or more
	// than one.
	CameraRigPickerConfig.bCanSelectCameraAsset = false;

	if (ReferencingCameraAssets.Num() == 0)
	{
		CameraRigPickerConfig.WarningMessage = LOCTEXT("NoReferencingCameraAssetWarning",
				"No camera asset references this Blueprint, so no camera rig list can be displayed. "
				"Make a camera asset use this Blueprint as its camera director evaluator, or use "
				"ActivateCameraRigViaProxy.");
	}
	else
	{
		CameraRigPickerConfig.InitialCameraAssetSelection = ReferencingCameraAssets[0];

		if (ReferencingCameraAssets.Num() > 1)
		{
			CameraRigPickerConfig.WarningMessage = LOCTEXT("ManyReferencingCameraAssetsWarning",
				"More than one camera asset references this Blueprint. Only camera rigs from the first "
				"one will be displayed. Even then, shared camera director Blueprints should use "
				"ActivateCameraRigViaProxy instead.");
		}
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

