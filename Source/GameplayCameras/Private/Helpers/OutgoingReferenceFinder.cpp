// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/OutgoingReferenceFinder.h"

#include "UObject/Class.h"
#include "UObject/Object.h"
#include "UObject/Package.h"

namespace UE::Cameras
{

FOutgoingReferenceFinder::FOutgoingReferenceFinder(UObject* InRootObject, UClass* InReferencedObjectClass)
{
	Initialize(InRootObject);

	TargetObjectClasses.Add(InReferencedObjectClass);
}

FOutgoingReferenceFinder::FOutgoingReferenceFinder(UObject* InRootObject, TArrayView<UClass*> InReferencedObjectClasses)
{
	Initialize(InRootObject);

	TargetObjectClasses.Append(InReferencedObjectClasses);
}

void FOutgoingReferenceFinder::Initialize(UObject* InRootObject)
{
	RootObject = InRootObject;
	PackageScope = InRootObject->GetOutermost();

	SetIsPersistent(true);
	SetIsSaving(true);
	SetFilterEditorOnly(false);

	ArIsObjectReferenceCollector = true;
	ArShouldSkipBulkData = true;
}

void FOutgoingReferenceFinder::CollectReferences()
{
	ObjectsToVisit.Reset();
	VisitedObjects.Reset();

	ObjectsToVisit.Add(RootObject);
	while (ObjectsToVisit.Num() > 0)
	{
		UObject* CurObj = ObjectsToVisit.Pop(EAllowShrinking::No);
		if (!VisitedObjects.Contains(CurObj))
		{
			VisitedObjects.Add(CurObj);
			CurObj->Serialize(*this);
		}
	}
}

bool FOutgoingReferenceFinder::GetAllReferences(TArray<UObject*>& OutReferencedObjects) const
{
	bool bHasAnyReference = false;
	for (TPair<UClass*, TSet<UObject*>> Pair : ReferencedObjects)
	{
		for (UObject* Obj : Pair.Value)
		{
			OutReferencedObjects.Add(Obj);
		}
		bHasAnyReference |= (!Pair.Value.IsEmpty());
	}
	return bHasAnyReference;
}

FArchive& FOutgoingReferenceFinder::operator<<(UObject*& ObjRef)
{
	if (ObjRef != nullptr)
	{
		UClass* ObjClass = ObjRef->GetClass();
		if (MatchesAnyTargetClass(ObjClass))
		{
			TSet<UObject*>& ReferencesOfClass = ReferencedObjects.FindOrAdd(ObjClass);
			ReferencesOfClass.Add(ObjRef);
		}

		if (ObjRef->IsIn(PackageScope) && !VisitedObjects.Contains(ObjRef))
		{
			ObjectsToVisit.Add(ObjRef);
		}
	}
	return *this;
}

bool FOutgoingReferenceFinder::MatchesAnyTargetClass(UClass* InObjClass) const
{
	for (UClass* TargetObjectClass : TargetObjectClasses)
	{
		if (InObjClass->IsChildOf(TargetObjectClass))
		{
			return true;
		}
	}

	return false;
}

}  // namespace UE::Cameras

