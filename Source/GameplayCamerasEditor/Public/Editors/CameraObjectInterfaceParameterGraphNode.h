// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphNode.h"

#include "CameraObjectInterfaceParameterGraphNode.generated.h"

class UCameraObjectInterfaceParameterBase;

/**
 * Custom graph editor node for a camera rig parameter.
 */
UCLASS()
class UCameraObjectInterfaceParameterGraphNode : public UObjectTreeGraphNode
{
	GENERATED_BODY()

public:

	/** Creates a new graph node. */
	UCameraObjectInterfaceParameterGraphNode(const FObjectInitializer& ObjInit);

	/** Gets the underlying object as a camera rig interface parameter. */
	UCameraObjectInterfaceParameterBase* GetInterfaceParameter() const;

public:

	// UEdGraphNode interface
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
};

