// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphNode.h"

#include "CameraRigInterfaceParameterGraphNode.generated.h"

/**
 * Custom graph editor node for a camera rig parameter.
 */
UCLASS()
class UCameraRigInterfaceParameterGraphNode : public UObjectTreeGraphNode
{
	GENERATED_BODY()

public:

	/** Creates a new graph node. */
	UCameraRigInterfaceParameterGraphNode(const FObjectInitializer& ObjInit);

public:

	// UEdGraphNode interface
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
};

