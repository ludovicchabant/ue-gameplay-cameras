// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "CoreTypes.h"
#include "Serialization/ArchiveUObject.h"

namespace UE::Cameras
{

/**
 * A utility class for finding outgoing references from a given package to external UObjects
 * of a given class.
 */
class GAMEPLAYCAMERAS_API FOutgoingReferenceFinder : public FArchiveUObject
{
public:

	/** Creates a new instance of the reference finder. */
	FOutgoingReferenceFinder(UObject* InRootObject, UClass* InReferencedObjectClass);

	/** Creates a new instance of the reference finder. */
	FOutgoingReferenceFinder(UObject* InRootObject, TArrayView<UClass*> InReferencedObjectClasses);

	/** Runs the reference finding. */
	void CollectReferences();

	/** Gets the references of a given class. */
	template<typename ObjectClass>
	bool GetReferencesOfClass(TArray<ObjectClass*>& OutReferencedObjects) const;

	/** Gets all referenced objects. */
	bool GetAllReferences(TArray<UObject*>& OutReferencedObjects) const;

protected:

	// FArchive interface.
	virtual FArchive& operator<<(UObject*& ObjRef) override;

private:

	void Initialize(UObject* InRootObject);

	bool MatchesAnyTargetClass(UClass* InObjClass) const;

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

