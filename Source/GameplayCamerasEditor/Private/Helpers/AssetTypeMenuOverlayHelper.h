// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"

class SWidget;
class UClass;

namespace UE::Cameras
{

class FAssetTypeMenuOverlayHelper
{
public:

	static TSharedRef<SWidget> CreateMenuOverlay(UClass* InAssetType);
};

}  // namespace UE::Cameras

