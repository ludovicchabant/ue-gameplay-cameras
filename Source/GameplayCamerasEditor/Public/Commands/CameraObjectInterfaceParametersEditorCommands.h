// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

namespace UE::Cameras
{

class FCameraObjectInterfaceParametersEditorCommands : public TCommands<FCameraObjectInterfaceParametersEditorCommands>
{
public:

	FCameraObjectInterfaceParametersEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> RenameInterfaceParameter;
	TSharedPtr<FUICommandInfo> DeleteInterfaceParameter;

	TSharedPtr<FUICommandInfo> GoToInterfaceParameter;
};

}  // namespace UE::Cameras

