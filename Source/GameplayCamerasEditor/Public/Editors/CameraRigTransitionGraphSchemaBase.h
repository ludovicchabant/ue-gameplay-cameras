// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraObjectGraphSchemaBase.h"

#include "CameraRigTransitionGraphSchemaBase.generated.h"

struct FObjectTreeGraphConfig;

/**
 * Base schema class for camera transition graph.
 */
UCLASS()
class UCameraRigTransitionGraphSchemaBase : public UCameraObjectGraphSchemaBase
{
	GENERATED_BODY()

protected:

	// UObjectTreeGraphSchema interface.
	virtual void CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const override;

	// UCameraObjectGraphSchemaBase interface.
	virtual void OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const override;
};

