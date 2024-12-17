// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigInterfaceParameterGraphNode.h"

#include "Core/CameraRigAsset.h"
#include "Editors/SCameraRigInterfaceParameterGraphNode.h"
#include "UObject/Object.h"

UCameraRigInterfaceParameterGraphNode::UCameraRigInterfaceParameterGraphNode(const FObjectInitializer& ObjInit)
	: UObjectTreeGraphNode(ObjInit)
{
}

UCameraRigInterfaceParameterBase* UCameraRigInterfaceParameterGraphNode::GetInterfaceParameter() const
{
	return CastChecked<UCameraRigInterfaceParameterBase>(GetObject(), ECastCheckedType::NullAllowed);
}

TSharedPtr<SGraphNode> UCameraRigInterfaceParameterGraphNode::CreateVisualWidget()
{
	return SNew(SCameraRigInterfaceParameterGraphNode).GraphNode(this);
}

