// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/SCameraRigNameGraphPin.h"

namespace UE::Cameras
{

/**
 * A version of the camera rig picker pin widget to be used specifically with 
 * the ActivateCameraRig method of a BlueprintCameraDirector.
 */
class SBlueprintCameraDirectorRigNameGraphPin : public SCameraRigNameGraphPin
{
public:

	SLATE_BEGIN_ARGS(SBlueprintCameraDirectorRigNameGraphPin)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:

	virtual void OnCustomizeCameraRigPickerConfig(FCameraRigPickerConfig& CameraRigPickerConfig) const override;
};

}  // namespace UE::Cameras

