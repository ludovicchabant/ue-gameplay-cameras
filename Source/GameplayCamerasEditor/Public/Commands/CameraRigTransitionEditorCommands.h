// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

namespace UE::Cameras
{

class FCameraRigTransitionEditorCommands : public TCommands<FCameraRigTransitionEditorCommands>
{
public:

	FCameraRigTransitionEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> FindInTransitions;
	TSharedPtr<FUICommandInfo> FocusHome;
};

}  // namespace UE::Cameras

