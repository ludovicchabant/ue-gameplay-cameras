// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

namespace UE::Cameras
{

class FCameraShakeAssetEditorCommands : public TCommands<FCameraShakeAssetEditorCommands>
{
public:

	FCameraShakeAssetEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> Build;
	
	TSharedPtr<FUICommandInfo> ShowMessages;
	TSharedPtr<FUICommandInfo> FindInCameraShake;
	TSharedPtr<FUICommandInfo> FocusHome;
};

}  // namespace UE::Cameras

