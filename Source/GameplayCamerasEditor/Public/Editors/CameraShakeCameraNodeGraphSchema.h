// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraNodeGraphSchema.h"

#include "CameraShakeCameraNodeGraphSchema.generated.h"

/**
 * Schema class for camera node graph.
 */
UCLASS()
class UCameraShakeCameraNodeGraphSchema : public UCameraNodeGraphSchema
{
	GENERATED_BODY()

public:

	UCameraShakeCameraNodeGraphSchema(const FObjectInitializer& ObjInit);

	FObjectTreeGraphConfig BuildGraphConfig() const;
};

