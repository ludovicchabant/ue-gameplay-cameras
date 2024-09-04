// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Delegates/Delegate.h"

class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraBuildLog;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCameraRigAssetBuilt, UCameraRigAsset*, FCameraBuildLog&);

/**
 * Global delegates for the GameplayCameras module.
 */
class GAMEPLAYCAMERAS_API FGameplayCamerasDelegates
{
public:

	/** Broadcast for when a camera rig has been built. */
	static inline FOnCameraRigAssetBuilt& OnCameraRigAssetBuilt()
	{
		return OnCameraRigAssetBuiltDelegates;
	}

private:

	static FOnCameraRigAssetBuilt OnCameraRigAssetBuiltDelegates;
};

}  // namespace UE::Cameras

