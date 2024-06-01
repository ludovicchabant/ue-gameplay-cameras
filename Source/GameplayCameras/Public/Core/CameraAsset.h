// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildStatus.h"
#include "Core/CameraRigTransition.h"
#include "CoreTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraAsset.generated.h"

class UCameraDirector;
class UCameraRigAsset;

namespace UE::Cameras { class FCameraBuildLog; }

/**
 * A complete camera asset.
 */
UCLASS(MinimalAPI)
class UCameraAsset 
	: public UObject
	, public IHasCameraBuildStatus
{
	GENERATED_BODY()

public:

	/** The camera director to use in this camera. */
	UPROPERTY(Instanced)
	TObjectPtr<UCameraDirector> CameraDirector;

	/** The list of camera rigs used by this camera. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigAsset>> CameraRigs;

	/** A list of default enter transitions for all the camera rigs in this asset. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** A list of default exit transitions for all the camera rigs in this asset. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;

public:

	/** The current build state of this camera asset. */
	UPROPERTY(Transient)
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Dirty;

	/**
	 * Builds and validates this camera, including all its camera rigs.
	 * Errors and warnings will go to the console.
	 */
	GAMEPLAYCAMERAS_API void BuildCamera();

	/**
	 * Builds and validates this camera, including all its camera rigs.
	 * Errors and warnings will go to the provided build log.
	 */
	GAMEPLAYCAMERAS_API void BuildCamera(UE::Cameras::FCameraBuildLog& InBuildLog);

public:

	// IHasCameraBuildStatus interface.
	virtual ECameraBuildStatus GetBuildStatus() const override { return BuildStatus; }
	virtual void DirtyBuildStatus() override;
};

