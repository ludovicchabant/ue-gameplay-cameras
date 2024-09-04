// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphNode.h"

#include "Core/CameraParameters.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraNodeGraphSchema.h"
#include "ToolMenus.h"

UCameraNodeGraphNode::UCameraNodeGraphNode(const FObjectInitializer& ObjInit)
	: UObjectTreeGraphNode(ObjInit)
{
}

void UCameraNodeGraphNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Add extra input pins for any camera parameter.
	UClass* CameraNodeClass = GetObject()->GetClass();
	for (TFieldIterator<FProperty> PropertyIt(CameraNodeClass); PropertyIt; ++PropertyIt)
	{
		FStructProperty* StructProperty = CastField<FStructProperty>(*PropertyIt);
		if (!StructProperty)
		{
			continue;
		}

		const FName PropertyName = PropertyIt->GetFName();
		
		FEdGraphPinType PinType;
		PinType.PinCategory = UCameraNodeGraphSchema::PC_CameraParameter;

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		if (StructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
		{\
			UEdGraphPin* ParameterPin = CreatePin(EGPD_Input, PinType, PropertyName);\
			ParameterPin->PinFriendlyName = FText::FromName(PropertyName);\
			continue;\
		}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}
}

UEdGraphPin* UCameraNodeGraphNode::GetPinForCameraParameterProperty(const FName& InPropertyName) const
{
	UEdGraphPin* const* FoundItem = Pins.FindByPredicate(
			[InPropertyName](UEdGraphPin* Item)
			{ 
				return Item->PinType.PinCategory == UCameraNodeGraphSchema::PC_CameraParameter &&
					Item->GetFName() == InPropertyName;
			});
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

FName UCameraNodeGraphNode::GetCameraParameterPropertyForPin(const UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UCameraNodeGraphSchema::PC_CameraParameter)
	{
		return InPin->GetFName();
	}
	return NAME_None;
}

