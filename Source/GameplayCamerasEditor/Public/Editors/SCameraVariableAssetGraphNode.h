// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/SCameraParameterGetterGraphNodeBase.h"

class UCameraVariableAssetGraphNode;

/**
 * Custom graph editor node widget for a camera rig parameter getter node.
 */
class SCameraVariableAssetGraphNode : public SCameraParameterGetterGraphNodeBase
{
public:

	SLATE_BEGIN_ARGS(SCameraVariableAssetGraphNode)
		: _GraphNode(nullptr)
	{}
		SLATE_ARGUMENT(UCameraVariableAssetGraphNode*, GraphNode)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};

