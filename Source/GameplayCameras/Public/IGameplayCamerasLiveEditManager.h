// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"
#include "UObject/UnrealType.h"

class UPackage;

namespace UE::Cameras
{

#if WITH_EDITOR

class IGameplayCamerasLiveEditListener;

/**
 * Interface for an object that can centralize the live-editing features of the camera system.
 */
class IGameplayCamerasLiveEditManager : public TSharedFromThis<IGameplayCamerasLiveEditManager>
{
public:

	virtual ~IGameplayCamerasLiveEditManager() {}

	/** Notify all listeners to reload cameras related to the given package. */
	virtual void NotifyPostBuildAsset(const UPackage* InAssetPackage) const = 0;

	/** Add a listener for the given package. */
	virtual void AddListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener) = 0;
	/** Removes a listener for the given package. */
	virtual void RemoveListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener) = 0;
};

#endif  // WITH_EDITOR

}  // namespace UE::Cameras

