// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Delegates/Delegate.h"

class UCameraAsset;
class UCameraNode;
class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraBuildLog;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraAssetBuilt, const UCameraAsset*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraRigAssetBuilt, const UCameraRigAsset*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraNodeChanged, const UCameraNode*);

/**
 * Global delegates for the GameplayCameras module.
 */
class GAMEPLAYCAMERAS_API FGameplayCamerasDelegates
{
public:

	/** Broadcast when a camera asset has been built. */
	static inline FOnCameraAssetBuilt& OnCameraAssetBuilt()
	{
		return OnCameraAssetBuiltDelegate;
	}

	/** Broadcast when a camera rig has been built. */
	static inline FOnCameraRigAssetBuilt& OnCameraRigAssetBuilt()
	{
		return OnCameraRigAssetBuiltDelegate;
	}

	/** Broadcast when a custom camera parameter provider node changes it parameters. */
	static inline FOnCameraNodeChanged& OnCustomCameraNodeParametersChanged()
	{
		return OnCustomCameraNodeParametersChangedDelegate;
	}

private:

	static FOnCameraAssetBuilt OnCameraAssetBuiltDelegate;
	static FOnCameraRigAssetBuilt OnCameraRigAssetBuiltDelegate;
	static FOnCameraNodeChanged OnCustomCameraNodeParametersChangedDelegate;
};

}  // namespace UE::Cameras

