// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IGameplayCamerasFamily.h"

class UCameraRigAsset;

namespace UE::Cameras
{

class FCameraRigAssetFamily : public IGameplayCamerasFamily
{
public:

	FCameraRigAssetFamily(UCameraRigAsset* InRootAsset);

	// IGameplayCamerasFamily interface.
	virtual UObject* GetRootAsset() const override;
	virtual void GetAssetTypes(TArray<UClass*>& OutAssetTypes) const override;
	virtual void FindAssetsOfType(UClass* InAssetType, TArray<FAssetData>& OutAssets) const override;
	virtual FText GetAssetTypeTooltip(UClass* InAssetType) const override;
	virtual const FSlateBrush* GetAssetIcon(UClass* InAssetType) const override;
	virtual FSlateColor GetAssetTint(UClass* InAssetType) const override;

private:

	UCameraRigAsset* RootAsset;
};

}  // namespace UE::Cameras

