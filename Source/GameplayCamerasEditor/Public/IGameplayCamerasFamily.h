// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetRegistry/AssetData.h"
#include "Styling/SlateColor.h"
#include "Templates/SharedPointer.h"

struct FSlateBrush;

namespace UE::Cameras
{

class IGameplayCamerasFamily : public TSharedFromThis<IGameplayCamerasFamily>
{
public:

	virtual ~IGameplayCamerasFamily() {}

	virtual UObject* GetRootAsset() const = 0;

	virtual void GetAssetTypes(TArray<UClass*>& OutAssetTypes) const = 0;

	virtual void FindAssetsOfType(UClass* InAssetType, TArray<FAssetData>& OutAssets) const = 0;

	virtual FText GetAssetTypeTooltip(UClass* InAssetType) const = 0;

	virtual const FSlateBrush* GetAssetIcon(UClass* InAssetType) const = 0;

	virtual FSlateColor GetAssetTint(UClass* InAssetType) const = 0;

public:

	static TSharedPtr<IGameplayCamerasFamily> CreateFamily(UObject* InAsset);
};

}  // namespace UE::Cameras

