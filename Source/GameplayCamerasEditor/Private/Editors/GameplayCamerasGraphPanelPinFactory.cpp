// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/GameplayCamerasGraphPanelPinFactory.h"

#include "Editors/SCameraRigNameGraphPin.h"
#include "K2Node_CallFunction.h"

namespace UE::Cameras
{

TSharedPtr<class SGraphPin> FGameplayCamerasGraphPanelPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (!Pin)
	{
		return nullptr;
	}

	UK2Node_CallFunction* OwningNode = Cast<UK2Node_CallFunction>(Pin->GetOwningNode());
	if (!OwningNode)
	{
		return nullptr;
	}

	UClass* BlueprintClass = OwningNode->GetBlueprintClassFromNode();
	UFunction* ReferencedFunction = OwningNode->FunctionReference.ResolveMember<UFunction>(BlueprintClass);
	if (!ReferencedFunction)
	{
		return nullptr;
	}

	FProperty* ParameterProperty = ReferencedFunction->FindPropertyByName(Pin->PinName);
	if (!ParameterProperty)
	{
		return nullptr;
	}

	if (ParameterProperty->HasMetaData(TEXT("UseCameraRigNamePicker")))
	{
		return SNew(SCameraRigNameGraphPin, Pin)
			.PinMode(ECameraRigNameGraphPinMode::NamePin);
	}

	if (ParameterProperty->HasMetaData(TEXT("UseCameraRigPicker")))
	{
		return SNew(SCameraRigNameGraphPin, Pin)
			.PinMode(ECameraRigNameGraphPinMode::ReferencePin);
	}

	return nullptr;
}

}  // namespace UE::Cameras

