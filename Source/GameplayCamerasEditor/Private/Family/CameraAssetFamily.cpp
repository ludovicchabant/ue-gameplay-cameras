// Copyright Epic Games, Inc. All Rights Reserved.

#include "Family/CameraAssetFamily.h"

#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigProxyAsset.h"
#include "Family/GameplayCamerasFamilyConstants.h"

#define LOCTEXT_NAMESPACE "CameraAssetFamily"

namespace UE::Cameras
{

FCameraAssetFamily::FCameraAssetFamily(UCameraAsset* InRootAsset)
	: RootAsset(InRootAsset)
{
	ensure(InRootAsset);
}

UObject* FCameraAssetFamily::GetRootAsset() const
{
	return RootAsset;
}

void FCameraAssetFamily::GetAssetTypes(TArray<UClass*>& OutAssetTypes) const
{
	OutAssetTypes.Add(UCameraAsset::StaticClass());
	OutAssetTypes.Add(UCameraRigAsset::StaticClass());
	OutAssetTypes.Add(UCameraRigProxyAsset::StaticClass());
}

void FCameraAssetFamily::FindAssetsOfType(UClass* InAssetType, TArray<FAssetData>& OutAssets) const
{
	if (!RootAsset)
	{
		return;
	}

	if (InAssetType == UCameraAsset::StaticClass())
	{
		OutAssets.Add(RootAsset);
		return;
	}

	UCameraDirector* CameraDirector = RootAsset->GetCameraDirector();
	if (!CameraDirector)
	{
		return;
	}

	FCameraDirectorRigUsageInfo UsageInfo;
	CameraDirector->GatherRigUsageInfo(UsageInfo);

	if (InAssetType == UCameraRigAsset::StaticClass())
	{
		for (UCameraRigAsset* CameraRig : UsageInfo.CameraRigs)
		{
			OutAssets.Add(FAssetData(CameraRig));
		}
	}
	else if (InAssetType == UCameraRigProxyAsset::StaticClass())
	{
	}
}

FText FCameraAssetFamily::GetAssetTypeTooltip(UClass* InAssetType) const
{
	if (InAssetType == UCameraRigAsset::StaticClass())
	{
		return LOCTEXT("CameraRigAssetTypeTooltip", "Open camera rigs referenced by this asset.");
	}
	else if (InAssetType == UCameraRigProxyAsset::StaticClass())
	{
		return LOCTEXT("CameraRigAssetTypeTooltip", "Open camera rig proxies referenced by this asset.");
	}
	return FText();
}

const FSlateBrush* FCameraAssetFamily::GetAssetIcon(UClass* InAssetType) const
{
	return FGameplayCamerasFamilyConstants::GetAssetIcon(InAssetType);
}

FSlateColor FCameraAssetFamily::GetAssetTint(UClass* InAssetType) const
{
	return FGameplayCamerasFamilyConstants::GetAssetTint(InAssetType);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

