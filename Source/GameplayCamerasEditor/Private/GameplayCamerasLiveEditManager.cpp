// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasLiveEditManager.h"

#include "IGameplayCamerasLiveEditListener.h"
#include "Misc/CoreDelegates.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

FGameplayCamerasLiveEditManager::FGameplayCamerasLiveEditManager()
{
	FCoreUObjectDelegates::GetPostGarbageCollect().AddRaw(this, &FGameplayCamerasLiveEditManager::OnPostGarbageCollection);
}

FGameplayCamerasLiveEditManager::~FGameplayCamerasLiveEditManager()
{
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
}

void FGameplayCamerasLiveEditManager::NotifyPostBuildAsset(const UPackage* InAssetPackage) const
{
	if (const FListenerArray* Listeners = ListenerMap.Find(InAssetPackage))
	{
		FGameplayCameraAssetBuildEvent BuildEvent;
		BuildEvent.AssetPackage = InAssetPackage;

		for (IGameplayCamerasLiveEditListener* Listener : *Listeners)
		{
			Listener->PostBuildAsset(BuildEvent);
		}
	}
}

void FGameplayCamerasLiveEditManager::AddListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener)
{
	if (ensure(InAssetPackage && Listener))
	{
		FListenerArray& Listeners = ListenerMap.FindOrAdd(InAssetPackage);
		Listeners.Add(Listener);
	}
}

void FGameplayCamerasLiveEditManager::RemoveListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener)
{
	if (ensure(InAssetPackage && Listener))
	{
		FListenerArray* Listeners = ListenerMap.Find(InAssetPackage);
		if (ensure(Listeners))
		{
			const int32 NumRemoved = Listeners->RemoveSwap(Listener);
			ensure(NumRemoved == 1);
			if (Listeners->IsEmpty())
			{
				ListenerMap.Remove(InAssetPackage);
			}
		}
	}
}

void FGameplayCamerasLiveEditManager::OnPostGarbageCollection()
{
	RemoveGarbage();
}

void FGameplayCamerasLiveEditManager::RemoveGarbage()
{
	for (auto It = ListenerMap.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
		}
	}
}

}  // namespace UE::Cameras

