// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SObjectTreeGraphNode.h"

#include "Editors/ObjectTreeGraphNode.h"

void SObjectTreeGraphNode::Construct(const FArguments& InArgs)
{
	GraphNode = InArgs._GraphNode;
	ObjectGraphNode = InArgs._GraphNode;

	SetCursor(EMouseCursor::CardinalCross);

	UpdateGraphNode();
}

void SObjectTreeGraphNode::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);

	if (ObjectGraphNode)
	{
		ObjectGraphNode->OnGraphNodeMoved();
	}
}

void SObjectTreeGraphAddArrayItemPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

