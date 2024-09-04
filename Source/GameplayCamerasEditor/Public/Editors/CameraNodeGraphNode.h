// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphNode.h"

#include "CameraNodeGraphNode.generated.h"

/**
 * Custom graph node for camera nodes. They mostly differ by showing input pins for any 
 * camera parameter property.
 */
UCLASS()
class UCameraNodeGraphNode : public UObjectTreeGraphNode
{
	GENERATED_BODY()

public:

	/** Creates a new graph node. */
	UCameraNodeGraphNode(const FObjectInitializer& ObjInit);

	/** Gets input pin for given camera parameter property on the underlying camera node. */
	UEdGraphPin* GetPinForCameraParameterProperty(const FName& InPropertyName) const;
	/** Gets camera parameter property name on the underlying camera node for the given input pin. */
	FName GetCameraParameterPropertyForPin(const UEdGraphPin* InPin) const;

public:

	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
};

