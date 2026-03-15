// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

namespace UE::Cameras
{

class FCameraRigAssetEditorCommands : public TCommands<FCameraRigAssetEditorCommands>
{
public:

	FCameraRigAssetEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> Build;
	
	TSharedPtr<FUICommandInfo> ShowNodeHierarchy;
	TSharedPtr<FUICommandInfo> ShowTransitions;

	TSharedPtr<FUICommandInfo> ShowMessages;
	TSharedPtr<FUICommandInfo> FindInCameraRig;
	TSharedPtr<FUICommandInfo> FocusHome;
};

}  // namespace UE::Cameras

