// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/CameraObjectInterfaceParametersEditorCommands.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/InputChord.h"
#include "Framework/Commands/UICommandInfo.h"
#include "GenericPlatform/GenericApplication.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "CameraObjectInterfaceParametersEditorCommands"

namespace UE::Cameras
{

FCameraObjectInterfaceParametersEditorCommands::FCameraObjectInterfaceParametersEditorCommands()
	: TCommands<FCameraObjectInterfaceParametersEditorCommands>(
			"CameraObjectInterfaceParametersEditor",
			LOCTEXT("CameraObjectInterfaceParametersEditor", "Camera Object Interface Parameters Editor"),
			NAME_None,
			FGameplayCamerasEditorStyle::Get()->GetStyleSetName()
		)
{
}

void FCameraObjectInterfaceParametersEditorCommands::RegisterCommands()
{
	UI_COMMAND(RenameInterfaceParameter, "Rename Interface Parameter", "Renames the selected interface parameter",
			EUserInterfaceActionType::Button, FInputChord(EKeys::F2));
	UI_COMMAND(DeleteInterfaceParameter, "Delete Interface Parameter", "Deletes the selected interface parameter(s)",
			EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));

	UI_COMMAND(GoToInterfaceParameter, "Go to Interface Parameter", "Navigates to the interface parameter",
			EUserInterfaceActionType::Button, FInputChord());
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

