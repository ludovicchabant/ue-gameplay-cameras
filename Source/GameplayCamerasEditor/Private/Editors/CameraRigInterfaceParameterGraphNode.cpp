// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigInterfaceParameterGraphNode.h"

#include "Core/CameraRigAsset.h"
#include "Editors/SCameraRigInterfaceParameterGraphNode.h"

UCameraRigInterfaceParameterGraphNode::UCameraRigInterfaceParameterGraphNode(const FObjectInitializer& ObjInit)
	: UObjectTreeGraphNode(ObjInit)
{
}

TSharedPtr<SGraphNode> UCameraRigInterfaceParameterGraphNode::CreateVisualWidget()
{
	return SNew(SCameraRigInterfaceParameterGraphNode).GraphNode(this);
}

