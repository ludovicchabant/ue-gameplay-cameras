// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraVariableAssetGraphNode.h"

#include "Commands/CameraVariableCollectionEditorCommands.h"
#include "Core/CameraVariableAssets.h"
#include "Editors/CameraObjectGraphSchemaBase.h"
#include "Editors/SCameraVariableAssetGraphNode.h"
#include "ToolMenu.h"
#include "UObject/Object.h"

#define LOCTEXT_NAMESPACE "CameraVariableAssetGraphNode"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableAssetGraphNode)

UCameraVariableAssetGraphNode::UCameraVariableAssetGraphNode(const FObjectInitializer& ObjInit)
	: UCameraParameterGetterGraphNodeBase(ObjInit)
{
}

UCameraVariableAsset* UCameraVariableAssetGraphNode::GetVariableAsset() const
{
	if (UCameraVariableAssetGetter* GetterNode = CastChecked<UCameraVariableAssetGetter>(GetObject(), ECastCheckedType::NullAllowed))
	{
		return GetterNode->Variable;
	}
	return nullptr;
}

FText UCameraVariableAssetGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (UCameraVariableAssetGetter* GetterNode = CastChecked<UCameraVariableAssetGetter>(GetObject(), ECastCheckedType::NullAllowed))
	{
		return FText::FromString(GetterNode->GetVariableDisplayName());
	}
	return FText();
}

TSharedPtr<SGraphNode> UCameraVariableAssetGraphNode::CreateVisualWidget()
{
	return SNew(SCameraVariableAssetGraphNode).GraphNode(this);
}

void UCameraVariableAssetGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	using namespace UE::Cameras;

	Super::GetNodeContextMenuActions(Menu, Context);

	const FCameraVariableCollectionEditorCommands& Commands = FCameraVariableCollectionEditorCommands::Get();

	{
		FToolMenuSection& Section = Menu->AddSection(
				"CameraVariableActions", LOCTEXT("CameraVariableActionsMenuHeader", "Camera Variable"));

		Section.AddMenuEntry(Commands.GoToVariable);
	}
}

FEdGraphPinType UCameraVariableAssetGraphNode::GetParameterPinType() const
{
	if (UCameraVariableAsset* VariableAsset = GetVariableAsset())
	{
		FEdGraphPinType PinType;
		PinType.PinCategory = UCameraObjectGraphSchemaBase::PC_Self;
		PinType.PinSubCategory = UEnum::GetValueAsName(VariableAsset->GetVariableType());
		return PinType;
	}
	return FEdGraphPinType();
}

#undef LOCTEXT_NAMESPACE

