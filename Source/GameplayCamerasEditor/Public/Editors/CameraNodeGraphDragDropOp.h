// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DragAndDrop/DecoratedDragDropOp.h"

class SGraphEditor;
class UCameraObjectInterfaceParameterBase;

class FCameraNodeGraphInterfaceParameterDragDropOp : public FDecoratedDragDropOp
{
public:
	
	DRAG_DROP_OPERATOR_TYPE(FCameraNodeGraphInterfaceParameterDragDropOp, FDecoratedDragDropOp)

	static TSharedRef<FCameraNodeGraphInterfaceParameterDragDropOp> New(UCameraObjectInterfaceParameterBase* InInterfaceParameter);

	FReply ExecuteDragOver(TSharedPtr<SGraphEditor> GraphEditor);
	FReply ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const UE::Slate::FDeprecateVector2DParameter& NewLocation);

private:

	UCameraObjectInterfaceParameterBase* InterfaceParameter;
};

