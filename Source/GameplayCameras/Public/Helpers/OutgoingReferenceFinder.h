// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "CoreTypes.h"
#include "Serialization/ArchiveUObject.h"

#define UE_API GAMEPLAYCAMERAS_API

namespace UE::Cameras
{

/**
 * A utility class for finding outgoing references from a given package to external UObjects
 * of a given class.
 */
class FOutgoingReferenceFinder : public FArchiveUObject
{
public:

	/** Creates a new instance of the reference finder. */
	UE_API FOutgoingReferenceFinder(UObject* InRootObject, UClass* InReferencedObjectClass);

	/** Creates a new instance of the reference finder. */
	UE_API FOutgoingReferenceFinder(UObject* InRootObject, TArrayView<UClass*> InReferencedObjectClasses);

	/** Runs the reference finding. */
	UE_API void CollectReferences();

	/** Gets the references of a given class. */
	template<typename ObjectClass>
	bool GetReferencesOfClass(TArray<ObjectClass*>& OutReferencedObjects) const;

	/** Gets all referenced objects. */
	UE_API bool GetAllReferences(TArray<UObject*>& OutReferencedObjects) const;

protected:

	// FArchive interface.
	UE_API virtual FArchive& operator<<(UObject*& ObjRef) override;

private:

	UE_API void Initialize(UObject* InRootObject);

	UE_API bool MatchesAnyTargetClass(UClass* InObjClass) const;

private:

	UObject* RootObject;
	UPackage* PackageScope;

	TSet<UClass*> TargetObjectClasses;

	TArray<UObject*> ObjectsToVisit;
	TSet<UObject*> VisitedObjects;

	TMap<UClass*, TSet<UObject*>> ReferencedObjects;
};

template<typename ObjectClass>
bool FOutgoingReferenceFinder::GetReferencesOfClass(TArray<ObjectClass*>& OutReferencedObjects) const
{
	if (const TSet<UObject*>* ReferencesOfClass = ReferencedObjects.Find(ObjectClass::StaticClass()))
	{
		for (UObject* Obj : (*ReferencesOfClass))
		{
			OutReferencedObjects.Add(CastChecked<ObjectClass>(Obj));
		}
		return !ReferencesOfClass->IsEmpty();
	}
	return false;
}

}  // namespace UE::Cameras

#undef UE_API
