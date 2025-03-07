// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraObjectInterfaceParameterGraphNode.h"

#include "Core/CameraRigAsset.h"
#include "Editors/SCameraObjectInterfaceParameterGraphNode.h"
#include "UObject/Object.h"

UCameraObjectInterfaceParameterGraphNode::UCameraObjectInterfaceParameterGraphNode(const FObjectInitializer& ObjInit)
	: UObjectTreeGraphNode(ObjInit)
{
}

UCameraObjectInterfaceParameterBase* UCameraObjectInterfaceParameterGraphNode::GetInterfaceParameter() const
{
	return CastChecked<UCameraObjectInterfaceParameterBase>(GetObject(), ECastCheckedType::NullAllowed);
}

TSharedPtr<SGraphNode> UCameraObjectInterfaceParameterGraphNode::CreateVisualWidget()
{
	return SNew(SCameraObjectInterfaceParameterGraphNode).GraphNode(this);
}

