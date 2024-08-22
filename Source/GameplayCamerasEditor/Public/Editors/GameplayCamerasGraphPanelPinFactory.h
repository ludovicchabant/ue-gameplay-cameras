// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraphUtilities.h"

class SGraphPin;
class UK2Node_CallFunction;

namespace UE::Cameras
{

/**
 * Graph editor pin factory for camera-specific pin widgets.
 */
struct FGameplayCamerasGraphPanelPinFactory : public FGraphPanelPinFactory
{
public:

	// FGraphPanelPinFactory interface.
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;

private:

	TSharedPtr<SGraphPin> CreateFunctionParameterPin(UEdGraphPin* Pin, UK2Node_CallFunction* CallFunctionNode) const;

	TSharedPtr<SGraphPin> CreateCameraRigPickerPin(UEdGraphPin* Pin) const;
	TSharedPtr<SGraphPin> CreateCameraVariablePickerPin(UEdGraphPin* Pin) const;
};

}  // namespace UE::Cameras

