// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraNodeGraphDragDropOp.h"

#include "Core/CameraRigAsset.h"
#include "Editors/CameraNodeGraphSchema.h"
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
		if (!InterfaceParameter->bHasGraphNode)
		{
			const FSlateBrush* OKIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
			SetToolTip(LOCTEXT("OnDragOver_Success", "Add interface parameter"), OKIcon);
		}
		else
		{
			const FSlateBrush* ErrorIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
			SetToolTip(
					LOCTEXT("OnDragOver_Error", "This interface parameter is already in the graph"),
					ErrorIcon);
		}
	}

	return FReply::Handled();
}

FReply FCameraNodeGraphInterfaceParameterDragDropOp::ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const FSlateCompatVector2f& NewLocation)
{
	if (!InterfaceParameter || InterfaceParameter->bHasGraphNode)
	{
		return FReply::Handled();
	}

	const FScopedTransaction Transaction(LOCTEXT("DropObjectClasses", "Drop New Nodes"));

	UEdGraph* Graph = GraphEditor->GetCurrentGraph();

	GraphEditor->ClearSelectionSet();

	FCameraNodeGraphSchemaAction_AddInterfaceParameterNode Action;
	Action.InterfaceParameter = InterfaceParameter;
	UEdGraphNode* NewNode = Action.PerformAction(Graph, nullptr, NewLocation, false);
	GraphEditor->SetNodeSelection(NewNode, true);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

