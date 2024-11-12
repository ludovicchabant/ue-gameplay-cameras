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
 * A utility class for finding references from a given package to a set of UObjects.
 */
class GAMEPLAYCAMERAS_API FObjectReferenceFinder : public FArchiveUObject
{
public:

	/** Creates a new instance of the reference finder. */
	FObjectReferenceFinder(UObject* InRootObject, TArrayView<UObject*> InReferencedObjects);

	/** Runs the reference finding. */
	void CollectReferences();

	/** Whether any object was found in the package that references any of the target objects. */
	bool HasAnyObjectReference() const;

	/** Returns the number of found references to the given target object. */
	int32 GetObjectReferenceCount(UObject* InObject) const;

protected:

	// FArchive interface.
	virtual FArchive& operator<<(UObject*& ObjRef) override;

private:

	void Initialize(UObject* InRootObject);

private:

	UObject* RootObject;
	UPackage* PackageScope;

	TSet<UObject*> TargetObjects;

	TArray<UObject*> ObjectsToVisit;
	TSet<UObject*> VisitedObjects;

	TMap<UObject*, int32> ObjectReferenceCounts;
};

}  // namespace UE::Cameras

