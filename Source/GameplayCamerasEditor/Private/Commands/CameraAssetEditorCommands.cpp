// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/CameraAssetEditorCommands.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "CameraAssetEditorCommands"

namespace UE::Cameras
{

FCameraAssetEditorCommands::FCameraAssetEditorCommands()
	: TCommands<FCameraAssetEditorCommands>(
			"CameraAssetEditor",
			LOCTEXT("CameraAssetEditor", "Camera Asset Editor"),
			NAME_None,
			FGameplayCamerasEditorStyle::Get()->GetStyleSetName()
		)
{
}

void FCameraAssetEditorCommands::RegisterCommands()
{
	UI_COMMAND(Build, "Build", "Builds the asset and refreshes it in PIE",
			EUserInterfaceActionType::Button, FInputChord(EKeys::F7));

	UI_COMMAND(ShowCameraDirector, "Director", "Shows the camera director editor",
			EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ShowCameraRigs, "Rigs", "Shows the camera rigs",
			EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ShowSharedTransitions, "Shared Transitions", "Shows the shared transitions",
			EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(EditCameraRig, "Edit Camera Rig", "Edits the selected camera rig",
			EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(AddCameraRig, "Add Camera Rig", "Adds a new camera rig",
			EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeleteCameraRig, "Delete Camera Rig", "Deletes the selected camera rig(s)",
			EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
	UI_COMMAND(RenameCameraRig, "Rename Camera Rig", "Renames the selected camera rig",
			EUserInterfaceActionType::Button, FInputChord(EKeys::F2));

	UI_COMMAND(ShowMessages, "Messages", "Shows the message log for this camera asset",
			EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(FindInCamera, "Search", "Searches for nodes in this camera asset",
			EUserInterfaceActionType::Button, FInputChord());
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

