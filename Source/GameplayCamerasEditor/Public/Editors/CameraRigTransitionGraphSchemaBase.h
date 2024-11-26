// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraRigTransitionGraphSchemaBase.generated.h"

struct FObjectTreeGraphConfig;

/**
 * Base schema class for camera transition graph.
 */
UCLASS()
class UCameraRigTransitionGraphSchemaBase : public UObjectTreeGraphSchema
{
	GENERATED_BODY()

public:

	FObjectTreeGraphConfig BuildGraphConfig() const;

protected:

	// UObjectTreeGraphSchema interface.
	virtual void CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const override;

protected:

	// UCameraRigTransitionGraphSchemaBase interface.
	virtual void OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const {}
};

