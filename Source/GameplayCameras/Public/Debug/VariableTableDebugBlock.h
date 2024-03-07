// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Debug/CameraDebugBlock.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

class FCameraVariableTable;

/**
 * A debug block that prints the contents of a variable table.
 */
class FVariableTableDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(FVariableTableDebugBlock)

public:

	/** Creates a new variable table debug block. */
	FVariableTableDebugBlock();
	/** Creates a new variable table debug block. */
	FVariableTableDebugBlock(const FCameraVariableTable& InVariableTable);

	/** Specifies the console variable to use to toggle the printing of variable IDs. */
	FVariableTableDebugBlock& WithShowVariableIdsCVar(const TCHAR* InShowVariableIdsCVarName)
	{
		ShowVariableIdsCVarName = InShowVariableIdsCVarName;
		return *this;
	}

protected:

	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;

	void Initialize(const FCameraVariableTable& InVariableTable);

private:

	struct FEntryDebugInfo
	{
		uint32 Id;
		FString Name;
		FString Value;
		bool bWritten : 1;
		bool bWrittenThisFrame : 1;
	};
	TArray<FEntryDebugInfo> Entries;

	FString ShowVariableIdsCVarName;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

