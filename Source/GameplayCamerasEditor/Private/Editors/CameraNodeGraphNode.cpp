// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphNode.h"

#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraObjectGraphSchemaBase.h"
#include "Editors/SCameraNodeGraphNode.h"
#include "GameplayCamerasDelegates.h"
#include "ToolMenus.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNodeGraphNode)

UCameraNodeGraphNode::UCameraNodeGraphNode(const FObjectInitializer& ObjInit)
	: UObjectTreeGraphNode(ObjInit)
{
}

void UCameraNodeGraphNode::OnInitialize()
{
	using namespace UE::Cameras;

	const bool bIsCustomParameterProvider = GetObject()->Implements<UCustomCameraNodeParameterProvider>();
	if (bIsCustomParameterProvider)
	{
		FGameplayCamerasDelegates::OnCustomCameraNodeParametersChanged().AddUObject(
				this, &UCameraNodeGraphNode::OnCustomCameraNodeParametersChanged);
	}
}

void UCameraNodeGraphNode::BeginDestroy()
{
	using namespace UE::Cameras;

	FGameplayCamerasDelegates::OnCustomCameraNodeParametersChanged().RemoveAll(this);

	Super::BeginDestroy();
}

void UCameraNodeGraphNode::OnCustomCameraNodeParametersChanged(const UCameraNode* CameraNode)
{
	if (CameraNode == GetObject())
	{
		ReconstructNode();
	}
}

void UCameraNodeGraphNode::AllocateDefaultPins()
{
	using namespace UE::Cameras;

	Super::AllocateDefaultPins();

	FCameraNodeParameterInfos ParameterInfos;
	ParameterInfos.BuildFrom(CastObject<UCameraNode>());

	const UEnum* VariableTypeEnum = StaticEnum<ECameraVariableType>();
	const UEnum* DataTypeEnum = StaticEnum<ECameraContextDataType>();

	for (const FCameraNodeBlendableParameterInfo& BlendableParameter : ParameterInfos.GetBlendableParameters())
	{
		FEdGraphPinType PinType;
		PinType.PinCategory = UCameraObjectGraphSchemaBase::PC_CameraParameter;
		PinType.PinSubCategory = VariableTypeEnum->GetNameByValue((int64)BlendableParameter.VariableType);
		PinType.PinSubCategoryObject = const_cast<UScriptStruct*>(BlendableParameter.BlendableStructType);

		UEdGraphPin* ParameterPin = CreatePin(EGPD_Input, PinType, BlendableParameter.ParameterName);
		ParameterPin->PinFriendlyName = FText::FromName(BlendableParameter.ParameterName);

		FString PinToolTip = VariableTypeEnum->GetNameStringByValue((int64)BlendableParameter.VariableType);
		if (BlendableParameter.BlendableStructType)
		{
			PinToolTip = BlendableParameter.BlendableStructType->GetDisplayNameText().ToString();
		}
		ParameterPin->PinToolTip = PinToolTip;
	}

	for (const FCameraNodeDataParameterInfo& DataParameter : ParameterInfos.GetDataParameters())
	{
		FEdGraphPinType PinType;
		PinType.PinCategory = UCameraObjectGraphSchemaBase::PC_CameraContextData;
		PinType.PinSubCategory = DataTypeEnum->GetNameByValue((int64)DataParameter.DataType);
		PinType.PinSubCategoryObject = const_cast<UObject*>(DataParameter.DataTypeObject);

		if (DataParameter.DataContainerType == ECameraContextDataContainerType::Array)
		{
			PinType.ContainerType = EPinContainerType::Array;
		}

		UEdGraphPin* ContextDataPin = CreatePin(EGPD_Input, PinType, DataParameter.ParameterName);
		ContextDataPin->PinFriendlyName = FText::FromName(DataParameter.ParameterName);

		FString PinToolTip = DataTypeEnum->GetNameStringByValue((int64)DataParameter.DataType);
		if (DataParameter.DataTypeObject)
		{
			PinToolTip = DataParameter.DataTypeObject->GetName();
		}
		ContextDataPin->PinToolTip = PinToolTip;
	}
}

TSharedPtr<SGraphNode> UCameraNodeGraphNode::CreateVisualWidget()
{
	return SNew(SCameraNodeGraphNode).GraphNode(this);
}

