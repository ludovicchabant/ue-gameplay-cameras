// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

namespace UE::Cameras
{

class FCameraAssetEditorCommands : public TCommands<FCameraAssetEditorCommands>
{
public:

	FCameraAssetEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> Build;

	TSharedPtr<FUICommandInfo> ShowCameraDirector;
	TSharedPtr<FUICommandInfo> ShowCameraRigs;
	TSharedPtr<FUICommandInfo> ShowSharedTransitions;

	TSharedPtr<FUICommandInfo> EditCameraRig;

	TSharedPtr<FUICommandInfo> AddCameraRig;
	TSharedPtr<FUICommandInfo> DeleteCameraRig;
	TSharedPtr<FUICommandInfo> RenameCameraRig;

	TSharedPtr<FUICommandInfo> ShowMessages;
	TSharedPtr<FUICommandInfo> FindInCamera;
};

}  // namespace UE::Cameras

