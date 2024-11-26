// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraRigTransitionGraphSchemaBase.h"

#include "CameraRigTransitionGraphSchema.generated.h"

/**
 * Schema class for a camera rig's transition graph.
 */
UCLASS()
class UCameraRigTransitionGraphSchema : public UCameraRigTransitionGraphSchemaBase
{
	GENERATED_BODY()

protected:

	// UCameraRigTransitionGraphSchemaBase interface.
	virtual void OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const override;
};

