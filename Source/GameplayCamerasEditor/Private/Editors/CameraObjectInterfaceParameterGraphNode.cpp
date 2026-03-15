// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraObjectInterfaceParameterGraphNode.h"

#include "Commands/CameraObjectInterfaceParametersEditorCommands.h"
#include "Core/CameraRigAsset.h"
#include "Editors/CameraObjectGraphSchemaBase.h"
#include "Editors/SCameraObjectInterfaceParameterGraphNode.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "UObject/Object.h"

#define LOCTEXT_NAMESPACE "CameraObjectInterfaceParameterGraphNode"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraObjectInterfaceParameterGraphNode)

UCameraObjectInterfaceParameterGraphNode::UCameraObjectInterfaceParameterGraphNode(const FObjectInitializer& ObjInit)
	: UCameraParameterGetterGraphNodeBase(ObjInit)
{
}

UCameraObjectInterfaceParameterBase* UCameraObjectInterfaceParameterGraphNode::GetInterfaceParameter() const
{
	if (UCameraObjectInterfaceParameterGetter* GetterNode = CastChecked<UCameraObjectInterfaceParameterGetter>(GetObject(), ECastCheckedType::NullAllowed))
	{
		return GetterNode->GetInterfaceParameter();
	}
	return nullptr;
}

FText UCameraObjectInterfaceParameterGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (UCameraObjectInterfaceParameterGetter* GetterNode = CastChecked<UCameraObjectInterfaceParameterGetter>(GetObject(), ECastCheckedType::NullAllowed))
	{
		return FText::FromString(GetterNode->GetInterfaceParameterName());
	}
	return FText();
}

TSharedPtr<SGraphNode> UCameraObjectInterfaceParameterGraphNode::CreateVisualWidget()
{
	return SNew(SCameraObjectInterfaceParameterGraphNode).GraphNode(this);
}

void UCameraObjectInterfaceParameterGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	using namespace UE::Cameras;

	Super::GetNodeContextMenuActions(Menu, Context);

	const FCameraObjectInterfaceParametersEditorCommands& Commands = FCameraObjectInterfaceParametersEditorCommands::Get();

	{
		FToolMenuSection& Section = Menu->AddSection(
				"InterfaceParameterActions", LOCTEXT("InterfaceParameterActionsMenuHeader", "Interface Parameter"));

		Section.AddMenuEntry(Commands.GoToInterfaceParameter);
	}
}

FEdGraphPinType UCameraObjectInterfaceParameterGraphNode::GetParameterPinType() const
{
	if (UCameraObjectInterfaceParameterBase* GetterNode = GetInterfaceParameter())
	{
		FCameraObjectInterfaceParameterDefinition ParameterDefinition;
		GetterNode->GetParameterDefinition(ParameterDefinition);

		FEdGraphPinType PinType;
		PinType.PinCategory = UCameraObjectGraphSchemaBase::PC_Self;
		if (ParameterDefinition.ParameterType == ECameraObjectInterfaceParameterType::Blendable)
		{
			PinType.PinSubCategory = UEnum::GetValueAsName(ParameterDefinition.VariableType);
			PinType.PinSubCategoryObject = const_cast<UScriptStruct*>(ParameterDefinition.BlendableStructType.Get());
		}
		else if (ParameterDefinition.ParameterType == ECameraObjectInterfaceParameterType::Data)
		{
			PinType.PinSubCategory = UEnum::GetValueAsName(ParameterDefinition.DataType);
			PinType.PinSubCategoryObject = const_cast<UObject*>(ParameterDefinition.DataTypeObject.Get());
		}
		return PinType;
	}
	return FEdGraphPinType();
}

#undef LOCTEXT_NAMESPACE

