// Copyright Epic Games, Inc. All Rights Reserved.

#include "Family/GameplayCamerasFamilyConstants.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigProxyAsset.h"
#include "Styles/GameplayCamerasEditorStyle.h"

namespace UE::Cameras
{

const FSlateBrush* FGameplayCamerasFamilyConstants::GetAssetIcon(UClass* InAssetType)
{
	if (InAssetType == UCameraAsset::StaticClass())
	{
		return FGameplayCamerasEditorStyle::Get()->GetBrush("Family.CameraAsset");
	}
	else if (InAssetType == UCameraRigAsset::StaticClass())
	{
		return FGameplayCamerasEditorStyle::Get()->GetBrush("Family.CameraRigAsset");
	}
	else if (InAssetType == UCameraRigProxyAsset::StaticClass())
	{
		return FGameplayCamerasEditorStyle::Get()->GetBrush("Family.CameraRigProxyAsset");
	}
	return nullptr;
}

FSlateColor FGameplayCamerasFamilyConstants::GetAssetTint(UClass* InAssetType)
{
	static const FColor Teal(23, 126, 137);
	static const FColor MidnightGreen(8, 76, 97);
	static const FColor Poppy(219, 58, 52);
	static const FColor Sunglow(255, 200, 87);

	if (InAssetType == UCameraAsset::StaticClass())
	{
		return FSlateColor(Sunglow);
	}
	else if (InAssetType == UCameraRigAsset::StaticClass())
	{
		return FSlateColor(Teal);
	}
	else if (InAssetType == UCameraRigProxyAsset::StaticClass())
	{
		return FSlateColor(MidnightGreen);
	}
	return FSlateColor();
}

}  // namespace UE::Cameras


