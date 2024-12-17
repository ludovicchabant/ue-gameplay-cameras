// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DragAndDrop/DecoratedDragDropOp.h"

class SGraphEditor;
class UCameraRigInterfaceParameterBase;

class FCameraNodeGraphInterfaceParameterDragDropOp : public FDecoratedDragDropOp
{
public:
	
	DRAG_DROP_OPERATOR_TYPE(FCameraNodeGraphInterfaceParameterDragDropOp, FDecoratedDragDropOp)

	static TSharedRef<FCameraNodeGraphInterfaceParameterDragDropOp> New(UCameraRigInterfaceParameterBase* InInterfaceParameter);

	FReply ExecuteDragOver(TSharedPtr<SGraphEditor> GraphEditor);
	FReply ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const FVector2D& NewLocation);

private:

	UCameraRigInterfaceParameterBase* InterfaceParameter;
};

