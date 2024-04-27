// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/SObjectTreeGraphNode.h"

/**
 * Custom graph editor node widget for a camera rig parameter node.
 */
class SCameraRigInterfaceParameterGraphNode : public SObjectTreeGraphNode
{
public:

	SLATE_BEGIN_ARGS(SCameraRigInterfaceParameterGraphNode)
		: _GraphNode(nullptr)
	{}
		SLATE_ARGUMENT(UObjectTreeGraphNode*, GraphNode)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:

	// SGraphNode interface.
	virtual void UpdateGraphNode() override;
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	virtual void GetDiffHighlightBrushes(const FSlateBrush*& BackgroundOut, const FSlateBrush*& ForegroundOut) const override;

private:
	
	FText GetInterfaceParameterName() const;
};

