// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraRigTransitionGraphSchemaBase.h"

#include "CameraSharedTransitionGraphSchema.generated.h"

/**
 * Schema class for a camera asset's shared transition graph.
 */
UCLASS()
class UCameraSharedTransitionGraphSchema : public UCameraRigTransitionGraphSchemaBase
{
	GENERATED_BODY()

protected:

	// UCameraRigTransitionGraphSchemaBase interface.
	virtual void OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const override;
	virtual ETransitionGraphContextActions GetTransitionGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
};

