// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraNodeGraphEditor.h"

#include "Compat/EditorCompat.h"
#include "Core/CameraVariableCollection.h"
#include "Editors/CameraNodeGraphDragDropOp.h"
#include "Editors/CameraObjectGraphSchemaBase.h"
#include "SGraphPanel.h"
#include "ScopedTransaction.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SCameraNodeGraphEditor"

FReply SCameraNodeGraphEditor::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FCameraNodeGraphInterfaceParameterDragDropOp> InterfaceParameterOp = 
		DragDropEvent.GetOperationAs<FCameraNodeGraphInterfaceParameterDragDropOp>();
	if (InterfaceParameterOp)
	{
		return InterfaceParameterOp->ExecuteDragOver(GraphEditor);
	}

	TSharedPtr<FCameraVariableAssetDragDropOp> VariableAssetOp = 
		DragDropEvent.GetOperationAs<FCameraVariableAssetDragDropOp>();
	if (VariableAssetOp)
	{
		return VariableAssetOp->ExecuteDragOver(GraphEditor);
	}

	return SObjectTreeGraphEditor::OnDragOver(MyGeometry, DragDropEvent);
}

FReply SCameraNodeGraphEditor::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FCameraNodeGraphInterfaceParameterDragDropOp> InterfaceParameterOp = 
		DragDropEvent.GetOperationAs<FCameraNodeGraphInterfaceParameterDragDropOp>();
	if (InterfaceParameterOp)
	{
		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();
		FSlateCompatVector2f NewLocation = GraphPanel->PanelCoordToGraphCoord(MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()));

		return InterfaceParameterOp->ExecuteDrop(GraphEditor, NewLocation);
	}

	TSharedPtr<FCameraVariableAssetDragDropOp> VariableAssetOp = 
		DragDropEvent.GetOperationAs<FCameraVariableAssetDragDropOp>();
	if (VariableAssetOp)
	{
		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();
		FSlateCompatVector2f NewLocation = GraphPanel->PanelCoordToGraphCoord(MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()));

		return VariableAssetOp->ExecuteDrop(GraphEditor, NewLocation);
	}

	return SObjectTreeGraphEditor::OnDrop(MyGeometry, DragDropEvent);
}

#undef LOCTEXT_NAMESPACE

