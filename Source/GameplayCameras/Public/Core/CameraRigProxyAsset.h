// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraRigProxyAsset.generated.h"

/**
 * A proxy for a camera rig asset.
 *
 * This is useful for indicating that a camera rig should be activated in a camera director,
 * but without hard-referencing a particular camera rig. This way, that camera director can
 * be used by multiple cameras with their own camera rigs.
 */
UCLASS(MinimalAPI)
class UCameraRigProxyAsset : public UObject
{
	GENERATED_BODY()

public:

	UCameraRigProxyAsset(const FObjectInitializer& ObjectInit);

public:

	// UObject interface.
#if WITH_EDITORONLY_DATA
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
#endif

public:

	/** Unique identifier for this camera rig proxy. */
	UPROPERTY()
	FGuid Guid;
};

