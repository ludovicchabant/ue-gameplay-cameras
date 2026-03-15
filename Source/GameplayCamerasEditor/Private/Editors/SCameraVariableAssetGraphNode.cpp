// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraVariableAssetGraphNode.h"

#include "Editors/CameraVariableAssetGraphNode.h"

void SCameraVariableAssetGraphNode::Construct(const FArguments& InArgs)
{
	SCameraParameterGetterGraphNodeBase::FArguments SuperArgs;
	SuperArgs
		.GraphNode(InArgs._GraphNode);
	SCameraParameterGetterGraphNodeBase::Construct(SuperArgs);
}

