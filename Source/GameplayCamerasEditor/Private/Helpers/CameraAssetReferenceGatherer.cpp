// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/CameraAssetReferenceGatherer.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Core/CameraAsset.h"
#include "Misc/AssetRegistryInterface.h"
#include "UObject/Object.h"
#include "UObject/Package.h"
#include "UObject/ReferencerFinder.h"

namespace UE::Cameras
{

void FCameraAssetReferenceGatherer::GetReferencingCameraAssets(UObject* ReferencedObject, TArray<UCameraAsset*>& OutReferencers)
{
	TSet<UCameraAsset*> UniqueReferencers;
	UPackage* ReferencedObjectPackage = ReferencedObject->GetOutermost();

	// Assume the asset registry module is already loaded.
	IAssetRegistry* AssetRegistry = IAssetRegistry::Get();

	// Get on-disk referencers.
	TArray<FAssetIdentifier> ReferencerIds;
	FAssetIdentifier AssetIdentifier(ReferencedObjectPackage->GetFName());
	AssetRegistry->GetReferencers(AssetIdentifier, ReferencerIds);

	TArray<FAssetData> AllAssetData;
	for (FAssetIdentifier ReferencerId : ReferencerIds)
	{
		AssetRegistry->GetAssetsByPackageName(ReferencerId.PackageName, AllAssetData);
	}
	for (const FAssetData& AssetData : AllAssetData)
	{
		UObject* Asset = AssetData.GetAsset();
		if (UCameraAsset* CameraAsset = Cast<UCameraAsset>(Asset))
		{
			UniqueReferencers.Add(CameraAsset);
		}
	}

	// Get in-memory referencers.
	TArray<UObject*> ReferencedObjects;
	GetObjectsWithPackage(ReferencedObjectPackage, ReferencedObjects);
	ReferencedObjects.Add(ReferencedObjectPackage);

	TArray<UObject*> AllReferencers = FReferencerFinder::GetAllReferencers(
			ReferencedObjects, nullptr, EReferencerFinderFlags::SkipWeakReferences);
	for (UObject* Referencer : AllReferencers)
	{
		if (UCameraAsset* CameraAsset = Cast<UCameraAsset>(Referencer))
		{
			UniqueReferencers.Add(CameraAsset);
		}
		else if (UCameraAsset* OuterCameraAsset = Referencer->GetTypedOuter<UCameraAsset>())
		{
			UniqueReferencers.Add(OuterCameraAsset);
		}
	}

	OutReferencers.Append(UniqueReferencers.Array());
}

}  // namespace UE::Cameras

