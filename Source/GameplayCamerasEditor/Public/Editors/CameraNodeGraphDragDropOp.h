// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Compat/EditorCompat.h"
#include "DragAndDrop/DecoratedDragDropOp.h"

class SGraphEditor;
class UCameraObjectInterfaceParameterBase;
class UCameraVariableAsset;

class FCameraNodeGraphInterfaceParameterDragDropOp : public FDecoratedDragDropOp
{
public:
	
	DRAG_DROP_OPERATOR_TYPE(FCameraNodeGraphInterfaceParameterDragDropOp, FDecoratedDragDropOp)

	static TSharedRef<FCameraNodeGraphInterfaceParameterDragDropOp> New(UCameraObjectInterfaceParameterBase* InInterfaceParameter);

	FReply ExecuteDragOver(TSharedPtr<SGraphEditor> GraphEditor);
	FReply ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const FSlateCompatVector2f& NewLocation);

private:

	UCameraObjectInterfaceParameterBase* InterfaceParameter = nullptr;
};

class FCameraVariableAssetDragDropOp : public FDecoratedDragDropOp
{
public:

	DRAG_DROP_OPERATOR_TYPE(FCameraVariableAssetDragDropOp, FDecoratedDragDropOp)

	static TSharedRef<FCameraVariableAssetDragDropOp> New(UCameraVariableAsset* InVariable);

	FReply ExecuteDragOver(TSharedPtr<SGraphEditor> GraphEditor);
	FReply ExecuteDrop(TSharedPtr<SGraphEditor> GraphEditor, const FSlateCompatVector2f& NewLocation);

private:

	UCameraVariableAsset* Variable = nullptr;
};

