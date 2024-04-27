// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SGraphNode.h"
#include "SGraphPin.h"

class UObjectTreeGraphNode;

/**
 * The widget used by default for object tree graph nodes.
 */
class SObjectTreeGraphNode : public SGraphNode
{
public:

	SLATE_BEGIN_ARGS(SObjectTreeGraphNode)
		: _GraphNode(nullptr)
	{}
		SLATE_ARGUMENT(UObjectTreeGraphNode*, GraphNode)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	UObjectTreeGraphNode* GetObjectGraphNode() const { return ObjectGraphNode; }

public:

	// SNodePanel::SNode interface.
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty=true) override;

protected:

	UObjectTreeGraphNode* ObjectGraphNode;
};

