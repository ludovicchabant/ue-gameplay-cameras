// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Delegates/Delegate.h"

class UCameraAsset;
class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraBuildLog;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraAssetBuilt, const UCameraAsset*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraRigAssetBuilt, const UCameraRigAsset*);

/**
 * Global delegates for the GameplayCameras module.
 */
class GAMEPLAYCAMERAS_API FGameplayCamerasDelegates
{
public:

	/** Broadcast when a camera asset has been built. */
	static inline FOnCameraAssetBuilt& OnCameraAssetBuilt()
	{
		return OnCameraAssetBuiltDelegates;
	}

	/** Broadcast when a camera rig has been built. */
	static inline FOnCameraRigAssetBuilt& OnCameraRigAssetBuilt()
	{
		return OnCameraRigAssetBuiltDelegates;
	}

private:

	static FOnCameraAssetBuilt OnCameraAssetBuiltDelegates;
	static FOnCameraRigAssetBuilt OnCameraRigAssetBuiltDelegates;
};

}  // namespace UE::Cameras

