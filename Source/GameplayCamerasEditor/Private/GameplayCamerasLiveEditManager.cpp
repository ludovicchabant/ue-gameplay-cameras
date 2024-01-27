// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCamerasLiveEditManager.h"

void FGameplayCamerasLiveEditManager::RegisterInstantiatedObjects(const TMap<UObject*, UObject*> InstantiatedObjects)
{
	for (const TPair<UObject*, UObject*>& Pair : InstantiatedObjects)
	{
		FInstantiationInfo& Info = Instantiations.FindOrAdd(Pair.Key);
		Info.InstantiatedObjects.Add(Pair.Value);
	}
}

void FGameplayCamerasLiveEditManager::ForwardPropertyChange(const UObject* Object, const FPropertyChangedEvent& PropertyChangedEvent)
{
	FInstantiationInfo* Info = Instantiations.Find(Object);
	if (!Info)
	{
		return;
	}

	const UClass* ObjectClass = Object->GetClass();
	const FProperty* ChangedProperty = PropertyChangedEvent.MemberProperty;
	if (!ensure(ChangedProperty))
	{
		return;
	}

	bool bForwardChange = false;
	if (ChangedProperty->IsA<FBoolProperty>()
			|| ChangedProperty->IsA<FNumericProperty>()
			|| ChangedProperty->IsA<FStructProperty>())
	{
		bForwardChange = true;
	}
	if (!bForwardChange)
	{
		return;
	}

	const void* SourceValuePtr = ChangedProperty->ContainerPtrToValuePtr<void>(Object);
	for (auto It = Info->InstantiatedObjects.CreateIterator(); It; ++It)
	{
		if (UObject* Inst = It->Get())
		{
			ensure(Inst->GetClass() == ObjectClass);
			void* InstValuePtr = ChangedProperty->ContainerPtrToValuePtr<void>(Inst);
			ChangedProperty->CopyCompleteValue(InstValuePtr, SourceValuePtr);
		}
		else
		{
			It.RemoveCurrent();
		}
	}
}

void FGameplayCamerasLiveEditManager::ForwardPropertyChange(const UObject* SourceObject, UObject* InstantiatedObject, const FPropertyChangedEvent& PropertyChangedEvent)
{
}

void FGameplayCamerasLiveEditManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto& Pair : Instantiations)
	{
		Collector.AddReferencedObject(Pair.Key);
	}
}

FString FGameplayCamerasLiveEditManager::GetReferencerName() const
{
	return TEXT("FGameplayCamerasLiveEditManager");
}

