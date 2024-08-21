// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/GameplayCamerasGraphPanelPinFactory.h"

#include "Core/CameraRigAsset.h"
#include "EdGraphSchema_K2.h"
#include "Editors/SBlueprintCameraDirectorRigNameGraphPin.h"
#include "Editors/SCameraRigNameGraphPin.h"
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
		return CreateFunctionParameterPin(Pin, CallFunctionNode);
	}

	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object && 
			Pin->PinType.PinSubCategoryObject == UCameraRigAsset::StaticClass())
	{
		return CreateCustomPin(Pin);
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

TSharedPtr<SGraphPin> FGameplayCamerasGraphPanelPinFactory::CreateCustomPin(UEdGraphPin* Pin) const
{
	return SNew(SCameraRigNameGraphPin, Pin);
}

}  // namespace UE::Cameras

