// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "K2Node_CameraRigBase.h"

#include "K2Node_SetCameraRigParameter.generated.h"

class FKismetCompilerContext;
class UCameraRigAsset;

UENUM()
enum class EK2Node_CameraParameterType : uint8
{
	Unknown,
	Blendable,
	Data
};

/**
 * Blueprint node that, given a camera rig, lets the user set the value of one single
 * exposed parameter.
 */
UCLASS(MinimalAPI)
class UK2Node_SetCameraRigParameter : public UK2Node_CameraRigBase
{
	GENERATED_BODY()

public:

	UK2Node_SetCameraRigParameter(const FObjectInitializer& ObjectInit);

	void Initialize(const FAssetData& UnloadedCameraRig, const FString& InCameraParameterName);
	void Initialize(UCameraRigAsset* InCameraRig, const FString& InCameraParameterName, ECameraVariableType InCameraVariableType, const UScriptStruct* InBlendableStructType);
	void Initialize(UCameraRigAsset* InCameraRig, const FString& InCameraParameterName, ECameraContextDataType InCameraContextDataType, ECameraContextDataContainerType InCameraContextDataContainerType, const UObject* InCameraContextDataTypeObject);

public:

	// UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

protected:

	// UK2Node_CameraRigBase interface.
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar, const FAssetData& CameraRigAssetData) const;

private:

	UPROPERTY()
	FString CameraParameterName;

	UPROPERTY()
	EK2Node_CameraParameterType CameraParameterType = EK2Node_CameraParameterType::Unknown;

	UPROPERTY()
	ECameraVariableType BlendableCameraParameterType;

	UPROPERTY()
	TObjectPtr<const UScriptStruct> BlendableStructType;

	UPROPERTY()
	ECameraContextDataType DataCameraParameterType;

	UPROPERTY()
	ECameraContextDataContainerType DataCameraParameterContainerType;

	UPROPERTY()
	TObjectPtr<UObject> DataCameraParameterTypeObject;
};

