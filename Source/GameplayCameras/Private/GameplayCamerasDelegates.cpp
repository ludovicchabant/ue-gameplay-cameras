// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasDelegates.h"

namespace UE::Cameras
{

FOnCameraAssetBuilt FGameplayCamerasDelegates::OnCameraAssetBuiltDelegate;
FOnCameraRigAssetBuilt FGameplayCamerasDelegates::OnCameraRigAssetBuiltDelegate;
FOnCameraNodeChanged FGameplayCamerasDelegates::OnCustomCameraNodeParametersChangedDelegate;

}  // namespace UE::Cameras

