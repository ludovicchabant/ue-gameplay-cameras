// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/GameplayCamerasGraphPanelPinFactory.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableAssets.h"
#include "EdGraphSchema_K2.h"
#include "Editors/SBlueprintCameraDirectorRigNameGraphPin.h"
#include "Editors/SCameraRigNameGraphPin.h"
#include "Editors/SCameraVariableNameGraphPin.h"
#include "K2Node_CallFunction.h"

namespace UE::Cameras
{

TSharedPtr<SGraphPin> FGameplayCamerasGraphPanelPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (!Pin)
	{
		return nullptr;
	}

	if (UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(Pin->GetOwningNode()))
	{
		if (TSharedPtr<SGraphPin> PinWidget = CreateFunctionParameterPin(Pin, CallFunctionNode))
		{
			return PinWidget;
		}
	}

	const FEdGraphPinType& PinType = Pin->PinType;
	const UClass* PinPropertyClass = Cast<const UClass>(PinType.PinSubCategoryObject);
	if (PinType.PinCategory == UEdGraphSchema_K2::PC_Object && PinPropertyClass)
	{
		if (PinPropertyClass == UCameraRigAsset::StaticClass())
		{
			return CreateCameraRigPickerPin(Pin);
		}
		if (PinPropertyClass->IsChildOf<UCameraVariableAsset>())
		{
			return CreateCameraVariablePickerPin(Pin);
		}
	}

	return nullptr;
}

TSharedPtr<SGraphPin> FGameplayCamerasGraphPanelPinFactory::CreateFunctionParameterPin(UEdGraphPin* Pin, UK2Node_CallFunction* CallFunctionNode) const
{
	UClass* BlueprintClass = CallFunctionNode->GetBlueprintClassFromNode();
	UFunction* ReferencedFunction = CallFunctionNode->FunctionReference.ResolveMember<UFunction>(BlueprintClass);
	if (!ReferencedFunction)
	{
		return nullptr;
	}

	FProperty* ParameterProperty = ReferencedFunction->FindPropertyByName(Pin->PinName);
	if (!ParameterProperty)
	{
		return nullptr;
	}

	if (ParameterProperty->HasMetaData(TEXT("UseBlueprintCameraDirectorRigPicker")))
	{
		return SNew(SBlueprintCameraDirectorRigNameGraphPin, Pin);
	}

	if (ParameterProperty->HasMetaData(TEXT("UseCameraRigPicker")))
	{
		return SNew(SCameraRigNameGraphPin, Pin);
	}

	return nullptr;
}

TSharedPtr<SGraphPin> FGameplayCamerasGraphPanelPinFactory::CreateCameraRigPickerPin(UEdGraphPin* Pin) const
{
	UEdGraphNode* OwningNode = Pin->GetOwningNode();
	if (!OwningNode)
	{
		return nullptr;
	}
	const UClass* OwningNodeClass = OwningNode->GetClass();
	const FString& UseCameraRigPickerForPinsMetaData = OwningNodeClass->GetMetaData(TEXT("UseCameraRigPickerForPins"));
	if (UseCameraRigPickerForPinsMetaData.IsEmpty())
	{
		return nullptr;
	}

	TArray<FString> CameraRigPickerPinNames;
	UseCameraRigPickerForPinsMetaData.ParseIntoArray(CameraRigPickerPinNames, TEXT(","));
	if (CameraRigPickerPinNames.Contains(Pin->GetName()))
	{
		return SNew(SCameraRigNameGraphPin, Pin);
	}
	return nullptr;
}

TSharedPtr<SGraphPin> FGameplayCamerasGraphPanelPinFactory::CreateCameraVariablePickerPin(UEdGraphPin* Pin) const
{
	return SNew(SCameraVariableNameGraphPin, Pin);
}

}  // namespace UE::Cameras

