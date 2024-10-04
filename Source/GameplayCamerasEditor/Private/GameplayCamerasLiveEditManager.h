// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IGameplayCamerasLiveEditManager.h"

#include "CoreTypes.h"
#include "UObject/WeakObjectPtr.h"

class UPackage;

namespace UE::Cameras
{

class FGameplayCamerasLiveEditManager : public IGameplayCamerasLiveEditManager
{
public:

	FGameplayCamerasLiveEditManager();
	~FGameplayCamerasLiveEditManager();

public:

	// IGameplayCamerasLiveEditManager interface
	virtual void NotifyPostBuildAsset(const UPackage* InAssetPackage) const override;
	virtual void AddListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener) override;
	virtual void RemoveListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener) override;

private:

	void OnPostGarbageCollection();

	void RemoveGarbage();

private:

	using FListenerArray = TArray<IGameplayCamerasLiveEditListener*>;
	using FListenerMap = TMap<TWeakObjectPtr<const UPackage>, FListenerArray>;
	FListenerMap ListenerMap;
};

}  // namespace UE::Cameras

