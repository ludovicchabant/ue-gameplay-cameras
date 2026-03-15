// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphDragDropOp.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableAssets.h"
#include "Editors/CameraObjectGraphSchemaBase.h"
#include "GraphEditor.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "CameraNodeGraphDragDropOp"

TSharedRef<FCameraNodeGraphInterfaceParameterDragDropOp> FCameraNodeGraphInterfaceParameterDragDropOp::New(UCameraObjectInterfaceParameterBase* InInterfaceParameter)
{
	TSharedRef<FCameraNodeGraphInterfaceParameterDragDropOp> Operation = MakeShared<FCameraNodeGraphInterfaceParameterDragDropOp>();
	Operation->InterfaceParameter = InInterfaceParameter;
	Operation->Construct();
	return Operation;
}

FReply FCameraNodeGraphInterfaceParameterDragDropOp::ExecuteDragOver(TSharedPtr<SGraphEditor> GraphEditor)
{
	if (InterfaceParameter)
	{
		const FSlateBrush* OKIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
		SetToolTip(LOCTEXT("OnInterfaceParameterDragOver_Success", "Add interface parameter getter"), OKIcon);
	}

	return FReply::Handled();
}

FReply FCameraNodeGraphInterfaceParameterDragDropOp::ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const FSlateCompatVector2f& NewLocation)
{
	if (!InterfaceParameter)
	{
		return FReply::Handled();
	}

	const FScopedTransaction Transaction(LOCTEXT("DropInterfaceParameter", "Drop interface parameter"));

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();

	GraphEditor->ClearSelectionSet();

	FCameraObjectGraphSchemaAction_AddInterfaceParameterGetterNode Action;
	Action.InterfaceParameter = InterfaceParameter;
	UEdGraphNode* NewNode = Action.PerformAction(Graph, nullptr, NewLocation, false);
	GraphEditor->SetNodeSelection(NewNode, true);

	return FReply::Handled();
}

TSharedRef<FCameraVariableAssetDragDropOp> FCameraVariableAssetDragDropOp::New(UCameraVariableAsset* InVariable)
{
	TSharedRef<FCameraVariableAssetDragDropOp> Operation = MakeShared<FCameraVariableAssetDragDropOp>();
	Operation->Variable = InVariable;
	Operation->Construct();
	return Operation;
}

FReply FCameraVariableAssetDragDropOp::ExecuteDragOver(TSharedPtr<SGraphEditor> GraphEditor)
{
	if (Variable)
	{
		const FSlateBrush* OKIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
		SetToolTip(LOCTEXT("OnVariableAssetDragOver_Success", "Add camera variable getter"), OKIcon);
	}

	return FReply::Handled();
}

FReply FCameraVariableAssetDragDropOp::ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const FSlateCompatVector2f& NewLocation)
{
	if (!Variable)
	{
		return FReply::Handled();
	}

	const FScopedTransaction Transaction(LOCTEXT("DropVariableAsset", "Drop camera variable"));

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();

	GraphEditor->ClearSelectionSet();

	FCameraObjectGraphSchemaAction_AddVariableAssetGetterNode Action;
	Action.SoftVariable = Variable->GetSoftPointer();
	UEdGraphNode* NewNode = Action.PerformAction(Graph, nullptr, NewLocation, false);
	GraphEditor->SetNodeSelection(NewNode, true);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

