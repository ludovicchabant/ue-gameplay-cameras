// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

class FCameraModeAssetEditorCommands : public TCommands<FCameraModeAssetEditorCommands>
{
public:

	FCameraModeAssetEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> Build;
};

