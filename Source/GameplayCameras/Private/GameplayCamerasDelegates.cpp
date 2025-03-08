// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasDelegates.h"

namespace UE::Cameras
{

FOnCameraAssetBuilt FGameplayCamerasDelegates::OnCameraAssetBuiltDelegate;
FOnCameraRigAssetBuilt FGameplayCamerasDelegates::OnCameraRigAssetBuiltDelegate;
FOnCameraShakeAssetBuilt FGameplayCamerasDelegates::OnCameraShakeAssetBuiltDelegate;
FOnCameraNodeChanged FGameplayCamerasDelegates::OnCustomCameraNodeParametersChangedDelegate;

}  // namespace UE::Cameras

