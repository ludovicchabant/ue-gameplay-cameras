// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasLiveEditManager.h"

#include "IGameplayCamerasLiveEditListener.h"
#include "GameplayCamerasEditorSettings.h"
#include "Misc/CoreDelegates.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

namespace UE::Cameras
{

namespace Internal
{

using FListenerArray = TArray<IGameplayCamerasLiveEditListener*, TInlineAllocator<4>>;

template<typename ObjectType>
void AddListenerImpl(
		TMap<TWeakObjectPtr<const ObjectType>, FListenerArray>& ListenerMap,
		const ObjectType* Object,
		IGameplayCamerasLiveEditListener* Listener)
{
	if (ensure(Object && Listener))
	{
		FListenerArray& Listeners = ListenerMap.FindOrAdd(Object);
		Listeners.Add(Listener);
	}
}

template<typename ObjectType>
void RemoveListenerImpl(
		TMap<TWeakObjectPtr<const ObjectType>, FListenerArray>& ListenerMap,
		const ObjectType* Object,
		IGameplayCamerasLiveEditListener* Listener)
{
	if (ensure(Object && Listener))
	{
		FListenerArray* Listeners = ListenerMap.Find(Object);
		if (ensure(Listeners))
		{
			const int32 NumRemoved = Listeners->RemoveSwap(Listener);
			ensure(NumRemoved == 1);
			if (Listeners->IsEmpty())
			{
				ListenerMap.Remove(Object);
			}
		}
	}
}

}  // namespace Internal

FGameplayCamerasLiveEditManager::FGameplayCamerasLiveEditManager()
{
	FCoreUObjectDelegates::GetPostGarbageCollect().AddRaw(this, &FGameplayCamerasLiveEditManager::OnPostGarbageCollection);
}

FGameplayCamerasLiveEditManager::~FGameplayCamerasLiveEditManager()
{
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
}

bool FGameplayCamerasLiveEditManager::CanRunInEditor() const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();
	return Settings->bEnableRunInEditor;
}

void FGameplayCamerasLiveEditManager::NotifyPostBuildAsset(const UPackage* InAssetPackage) const
{
	if (const FListenerArray* Listeners = PackageListenerMap.Find(InAssetPackage))
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
	Internal::AddListenerImpl(PackageListenerMap, InAssetPackage, Listener);
}

void FGameplayCamerasLiveEditManager::RemoveListener(const UPackage* InAssetPackage, IGameplayCamerasLiveEditListener* Listener)
{
	Internal::RemoveListenerImpl(PackageListenerMap, InAssetPackage, Listener);
}

void FGameplayCamerasLiveEditManager::NotifyPostEditChangeProperty(const UCameraNode* InCameraNode, const FPropertyChangedEvent& PropertyChangedEvent) const
{
	if (const FListenerArray* Listeners = NodeListenerMap.Find(InCameraNode))
	{
		for (IGameplayCamerasLiveEditListener* Listener : *Listeners)
		{
			Listener->PostEditChangeProperty(InCameraNode, PropertyChangedEvent);
		}
	}
}

void FGameplayCamerasLiveEditManager::AddListener(const UCameraNode* InCameraNode, IGameplayCamerasLiveEditListener* Listener)
{
	Internal::AddListenerImpl(NodeListenerMap, InCameraNode, Listener);
}

void FGameplayCamerasLiveEditManager::RemoveListener(const UCameraNode* InCameraNode, IGameplayCamerasLiveEditListener* Listener)
{
	Internal::RemoveListenerImpl(NodeListenerMap, InCameraNode, Listener);
}

void FGameplayCamerasLiveEditManager::RemoveListener(IGameplayCamerasLiveEditListener* Listener)
{
	if (ensure(Listener))
	{
		for (auto It = PackageListenerMap.CreateIterator(); It; ++It)
		{
			It.Value().Remove(Listener);
			if (It.Value().IsEmpty())
			{
				It.RemoveCurrent();
			}
		}
		for (auto It = NodeListenerMap.CreateIterator(); It; ++It)
		{
			It.Value().Remove(Listener);
			if (It.Value().IsEmpty())
			{
				It.RemoveCurrent();
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
	for (auto It = PackageListenerMap.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
		}
	}
}

}  // namespace UE::Cameras

