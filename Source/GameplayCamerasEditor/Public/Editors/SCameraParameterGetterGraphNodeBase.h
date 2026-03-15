// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/SObjectTreeGraphNode.h"

/**
 * Base custom graph editor node widget for all kinds of node parameter getters.
 */
class SCameraParameterGetterGraphNodeBase : public SObjectTreeGraphNode
{
public:

	SLATE_BEGIN_ARGS(SCameraParameterGetterGraphNodeBase)
		: _GraphNode(nullptr)
	{}
		SLATE_ARGUMENT(UObjectTreeGraphNode*, GraphNode)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:

	// SGraphNode interface.
	virtual void UpdateGraphNode() override;
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* InPin) const override;

private:

	FText GetNodeFullTitle() const;
};

