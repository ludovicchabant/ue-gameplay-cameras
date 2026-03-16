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

	FLinearColor GetVariablePinColor(const FName& VariableTypeName) const;
	FLinearColor GetContextDataPinColor(const FName& DataTypeName) const;

private:

	TMap<FName, FLinearColor> VariablePinColors;
	TMap<FName, FLinearColor> DataPinColors;

	FLinearColor DefaultPinColor;
};

}  // namespace UE::Cameras

