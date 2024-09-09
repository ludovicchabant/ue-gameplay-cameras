// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "EdGraph/EdGraphNode.h"
#include "K2Node.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "K2Node_SetCameraRigParameters.generated.h"

class UCameraRigAsset;

/**
 * Dynamic Blueprint node that, given a camera rig, exposes input pins for any camera rig parameters
 * found on it. On compile, this node gets expanded into the appropriate number of individual setter
 * function calls for each parameter (see UCameraRigParameterInterop).
 */
UCLASS(MinimalAPI, meta=(UseCameraRigPickerForPins="CameraRig"))
class UK2Node_SetCameraRigParameters : public UK2Node
{
	GENERATED_BODY()

public:

	UK2Node_SetCameraRigParameters(const FObjectInitializer& ObjectInit);

public:

	// UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin);
	virtual void PostPlacedNewNode() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	// UK2Node interface.
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;

protected:

	static const FName CameraRigPinName;
	static const FName CameraVariableTablePinName;

	UEdGraphPin* GetCameraRigPin(TArrayView<UEdGraphPin* const>* InPinsToSearch = nullptr) const;
	UEdGraphPin* GetCameraEvaluationResultPin() const;
	void GetCameraRigParameterPins(TArray<UEdGraphPin*>& OutParameterPins) const;
	bool IsCameraRigParameterPin(UEdGraphPin* Pin) const;
	void CreatePinsForCameraRig(UCameraRigAsset* CameraRig, TArray<UEdGraphPin*>* CreatedPins = nullptr);

	UCameraRigAsset* GetCameraRig(TArrayView<UEdGraphPin* const>* InPinsToSearch = nullptr) const;
	void OnCameraRigChanged();
};

