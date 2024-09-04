// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraNodeGraphNode.h"

#include "CameraRigNodeGraphNode.generated.h"

UCLASS()
class UCameraRigNodeGraphNode : public UCameraNodeGraphNode
{
	GENERATED_BODY()

public:

	UCameraRigNodeGraphNode(const FObjectInitializer& ObjInit);

public:

	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
};

