// Copyright Epic Games, Inc. All Rights Reserved.

#include "Family/CameraRigAssetFamily.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraRigAsset.h"
#include "Family/GameplayCamerasFamilyConstants.h"
#include "UObject/ReferencerFinder.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetFamily"

namespace UE::Cameras
{

FCameraRigAssetFamily::FCameraRigAssetFamily(UCameraRigAsset* InRootAsset)
	: RootAsset(InRootAsset)
{
	ensure(InRootAsset);
}

UObject* FCameraRigAssetFamily::GetRootAsset() const
{
	return RootAsset;
}

void FCameraRigAssetFamily::GetAssetTypes(TArray<UClass*>& OutAssetTypes) const
{
	OutAssetTypes.Add(UCameraAsset::StaticClass());
	OutAssetTypes.Add(UCameraRigAsset::StaticClass());
}

void FCameraRigAssetFamily::FindAssetsOfType(UClass* InAssetType, TArray<FAssetData>& OutAssets) const
{
	if (!RootAsset)
	{
		return;
	}

	if (InAssetType == UCameraRigAsset::StaticClass())
	{
		OutAssets.Add(RootAsset);
	}
	else if (InAssetType == UCameraAsset::StaticClass())
	{
		const FName RootPackageName = RootAsset->GetPackage()->GetFName();

		TArray<FAssetData> AllCameraAssets;
		IAssetRegistry& AssetRegistry = FAssetRegistryModule::GetRegistry();
		AssetRegistry.GetAssetsByClass(UCameraAsset::StaticClass()->GetClassPathName(), AllCameraAssets);
		for (const FAssetData& CameraAsset : AllCameraAssets)
		{
			// By default, use the asset tags to know which camera asset uses which camera rigs.
			// If the asset is loaded, it might have been modified and not saved yet, so use the in-memory
			// object instead.
			// If the asset doesn't have tags, it hasn't been saved since tags were added, so load it
			// and also use it directly in memory.
			bool bUseObjectDirectly = (CameraAsset.IsAssetLoaded());
			if (!bUseObjectDirectly)
			{
				FString UsedCameraRigsTag = CameraAsset.GetTagValueRef<FString>(TEXT("UsedCameraRigs"));
				if (!UsedCameraRigsTag.IsEmpty())
				{
					TArray<FString> UsedCameraRigs;
					UsedCameraRigsTag.ParseIntoArray(UsedCameraRigs, TEXT(";"));
					if (UsedCameraRigs.Contains(RootPackageName))
					{
						OutAssets.Add(CameraAsset);
					}
				}
				else
				{
					bUseObjectDirectly = true;
				}
			}
			if (bUseObjectDirectly)
			{
				if (UCameraAsset* LoadedCameraAsset = Cast<UCameraAsset>(CameraAsset.GetAsset()))
				{
					if (UCameraDirector* LoadedCameraDirector = LoadedCameraAsset->GetCameraDirector())
					{
						FCameraDirectorRigUsageInfo UsageInfo;
						LoadedCameraDirector->GatherRigUsageInfo(UsageInfo);
						if (UsageInfo.CameraRigs.Contains(RootAsset))
						{
							OutAssets.Add(CameraAsset);
						}
					}
				}
			}
		}
	}
}

FText FCameraRigAssetFamily::GetAssetTypeTooltip(UClass* InAssetType) const
{
	if (InAssetType == UCameraAsset::StaticClass())
	{
		return LOCTEXT("CameraAssetTypeTooltip", "Open camera assets referencing this asset.");
	}
	return FText();
}

const FSlateBrush* FCameraRigAssetFamily::GetAssetIcon(UClass* InAssetType) const
{
	return FGameplayCamerasFamilyConstants::GetAssetIcon(InAssetType);
}

FSlateColor FCameraRigAssetFamily::GetAssetTint(UClass* InAssetType) const
{
	return FGameplayCamerasFamilyConstants::GetAssetTint(InAssetType);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

