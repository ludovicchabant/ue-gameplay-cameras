// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Styling/SlateColor.h"

class UClass;
struct FSlateBrush;

namespace UE::Cameras
{

class FGameplayCamerasFamilyConstants
{
public:

	static const FSlateBrush* GetAssetIcon(UClass* InAssetType);

	static FSlateColor GetAssetTint(UClass* InAssetType);
};

}  // namespace UE::Cameras

