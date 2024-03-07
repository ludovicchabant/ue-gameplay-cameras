// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/CameraRigAssetEditorCommands.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetEditorCommands"

namespace UE::Cameras
{

FCameraRigAssetEditorCommands::FCameraRigAssetEditorCommands()
	: TCommands<FCameraRigAssetEditorCommands>(
			"GameplayCameras_CameraRigAssetEditor",
			NSLOCTEXT("Contexts", "GameplayCameras_CameraRigAssetEditor", "Camera Rig Asset Editor"),
			NAME_None,
			FAppStyle::GetAppStyleSetName()
		)
{
}

void FCameraRigAssetEditorCommands::RegisterCommands()
{
	UI_COMMAND(Build, "Build", "Builds the asset and refreshes it in PIE",
			EUserInterfaceActionType::Button, FInputChord(EKeys::F7));
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

