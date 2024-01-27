// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IGameplayCamerasLiveEditManager.h"

#include "CoreTypes.h"
#include "UObject/GCObject.h"
#include "UObject/WeakObjectPtr.h"

class FGameplayCamerasLiveEditManager 
	: public IGameplayCamerasLiveEditManager
	, public FGCObject
{
public:

	virtual void RegisterInstantiatedObjects(const TMap<UObject*, UObject*> InstantiatedObjects) override;

	virtual void ForwardPropertyChange(const UObject* Object, const FPropertyChangedEvent& PropertyChangedEvent) override;

private:

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

	void ForwardPropertyChange(const UObject* SourceObject, UObject* InstantiatedObject, const FPropertyChangedEvent& PropertyChangedEvent);

private:
	struct FInstantiationInfo
	{
		TArray<TWeakObjectPtr<>> InstantiatedObjects;
	};
	TMap<TObjectPtr<const UObject>, FInstantiationInfo> Instantiations;
};

