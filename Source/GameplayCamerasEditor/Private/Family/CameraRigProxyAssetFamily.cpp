// Copyright Epic Games, Inc. All Rights Reserved.

#include "Family/CameraRigProxyAssetFamily.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigProxyAsset.h"
#include "Family/GameplayCamerasFamilyConstants.h"

#define LOCTEXT_NAMESPACE "CameraRigProxyAssetFamily"

namespace UE::Cameras
{

FCameraRigProxyAssetFamily::FCameraRigProxyAssetFamily(UCameraRigProxyAsset* InRootAsset)
	: RootAsset(InRootAsset)
{
	ensure(InRootAsset);
}

UObject* FCameraRigProxyAssetFamily::GetRootAsset() const
{
	return RootAsset;
}

void FCameraRigProxyAssetFamily::GetAssetTypes(TArray<UClass*>& OutAssetTypes) const
{
	OutAssetTypes.Add(UCameraAsset::StaticClass());
	OutAssetTypes.Add(UCameraRigProxyAsset::StaticClass());
}

void FCameraRigProxyAssetFamily::FindAssetsOfType(UClass* InAssetType, TArray<FAssetData>& OutAssets) const
{
	if (InAssetType == UCameraAsset::StaticClass())
	{
	}
}

FText FCameraRigProxyAssetFamily::GetAssetTypeTooltip(UClass* InAssetType) const
{
	if (InAssetType == UCameraAsset::StaticClass())
	{
		return LOCTEXT("CameraRigAssetTypeTooltip", "Open camera assets referencing this asset.");
	}
	return FText();
}

const FSlateBrush* FCameraRigProxyAssetFamily::GetAssetIcon(UClass* InAssetType) const
{
	return FGameplayCamerasFamilyConstants::GetAssetIcon(InAssetType);
}

FSlateColor FCameraRigProxyAssetFamily::GetAssetTint(UClass* InAssetType) const
{
	return FGameplayCamerasFamilyConstants::GetAssetTint(InAssetType);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

