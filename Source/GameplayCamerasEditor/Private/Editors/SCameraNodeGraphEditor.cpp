// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraNodeGraphEditor.h"

#include "Editors/CameraNodeGraphDragDropOp.h"
#include "SGraphPanel.h"

FReply SCameraNodeGraphEditor::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FCameraNodeGraphInterfaceParameterDragDropOp> InterfaceParameterOp = 
		DragDropEvent.GetOperationAs<FCameraNodeGraphInterfaceParameterDragDropOp>();
	if (InterfaceParameterOp)
	{
		return InterfaceParameterOp->ExecuteDragOver(GraphEditor);
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
		FVector2D NewLocation = GraphPanel->PanelCoordToGraphCoord(MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()));

		return InterfaceParameterOp->ExecuteDrop(GraphEditor, NewLocation);
	}

	return SObjectTreeGraphEditor::OnDrop(MyGeometry, DragDropEvent);
}

