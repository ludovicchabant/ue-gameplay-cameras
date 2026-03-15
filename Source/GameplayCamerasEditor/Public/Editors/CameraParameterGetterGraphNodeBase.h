// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphNode.h"

#include "CameraParameterGetterGraphNodeBase.generated.h"

/**
 * Base class for graph node representing some kind of camera node parameter getter.
 */
UCLASS()
class UCameraParameterGetterGraphNodeBase : public UObjectTreeGraphNode
{
	GENERATED_BODY()

public:

	// UCameraParameterGetterGraphNodeBase interface.
	virtual FEdGraphPinType GetParameterPinType() const { return FEdGraphPinType(); }
};

