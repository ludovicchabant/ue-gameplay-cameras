// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraNodeGraphSchema.h"

#include "CameraRigCameraNodeGraphSchema.generated.h"

/**
 * Schema class for camera node graph.
 */
UCLASS()
class UCameraRigCameraNodeGraphSchema : public UCameraNodeGraphSchema
{
	GENERATED_BODY()

public:

	UCameraRigCameraNodeGraphSchema(const FObjectInitializer& ObjInit);

	FObjectTreeGraphConfig BuildGraphConfig() const;

protected:

	// UObjectTreeGraphSchema interface.
	virtual void CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const override;
};

