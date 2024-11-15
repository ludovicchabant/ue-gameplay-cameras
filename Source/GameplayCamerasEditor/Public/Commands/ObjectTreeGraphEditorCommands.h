// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Framework/Commands/Commands.h"

class FUICommandInfo;
class UClass;
struct FInputChord;

namespace UE::Cameras
{

class FObjectTreeGraphEditorCommands : public TCommands<FObjectTreeGraphEditorCommands>
{
public:

	FObjectTreeGraphEditorCommands();

	virtual void RegisterCommands() override;
	
public:

	TSharedPtr<FUICommandInfo> InsertArrayItemPinBefore;
	TSharedPtr<FUICommandInfo> InsertArrayItemPinAfter;
	TSharedPtr<FUICommandInfo> RemoveArrayItemPin;
};

}  // namespace UE::Cameras

