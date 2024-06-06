// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraphUtilities.h"

namespace UE::Cameras
{

/**
 * Graph editor pin factory for camera-specific pin widgets.
 */
struct FGameplayCamerasGraphPanelPinFactory : public FGraphPanelPinFactory
{
public:

	// FGraphPanelPinFactory interface.
	virtual TSharedPtr<class SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};

}  // namespace UE::Cameras

