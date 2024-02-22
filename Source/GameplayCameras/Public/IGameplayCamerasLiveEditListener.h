// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#if WITH_EDITOR

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"
#include "UObject/UnrealType.h"

class UPackage;

struct FGameplayCameraAssetBuildEvent
{
	const UPackage* AssetPackage = nullptr;
};

/**
 * Interface for an object that can handle an asset being hot-reloaded at runtime.
 */
class IGameplayCamerasLiveEditListener
{
public:

	virtual ~IGameplayCamerasLiveEditListener() {}

	void PostBuildAsset(const FGameplayCameraAssetBuildEvent& BuildEvent) { OnPostBuildAsset(BuildEvent); }

protected:

	virtual void OnPostBuildAsset(const FGameplayCameraAssetBuildEvent& BuildEvent) = 0;
};

#endif  // WITH_EDITOR

