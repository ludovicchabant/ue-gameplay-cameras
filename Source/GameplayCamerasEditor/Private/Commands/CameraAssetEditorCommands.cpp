// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/CameraAssetEditorCommands.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandInfo.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SequencerCommands"

namespace UE::Cameras
{

FCameraAssetEditorCommands::FCameraAssetEditorCommands()
	: TCommands<FCameraAssetEditorCommands>(
			"GameplayCameras_CameraAssetEditor",
			NSLOCTEXT("Contexts", "GameplayCameras_CameraAssetEditor", "Camera Asset Editor"),
			NAME_None,
			FAppStyle::GetAppStyleSetName()
		)
{
}

void FCameraAssetEditorCommands::RegisterCommands()
{
	UI_COMMAND(Build, "Build", "Builds the asset and refreshes it in PIE",
			EUserInterfaceActionType::Button, FInputChord(EKeys::F7));
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

