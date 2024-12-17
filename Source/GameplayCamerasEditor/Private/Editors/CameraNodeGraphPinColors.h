// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Math/Color.h"
#include "UObject/NameTypes.h"

namespace UE::Cameras
{

struct FCameraNodeGraphPinColors
{
	void Initialize();

	FLinearColor GetPinColor(const FName& TypeName) const;
	FLinearColor GetStructPinColor() const;

private:

	TMap<FName, FLinearColor> PinColors;
	FLinearColor DefaultPinColor;
	FLinearColor StructPinColor;
};

}  // namespace UE::Cameras

