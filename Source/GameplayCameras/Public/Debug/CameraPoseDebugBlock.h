// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraPose.h"
#include "Debug/CameraDebugBlock.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

/**
 * A debug block that displays information about a camera pose.
 */
class FCameraPoseDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(GAMEPLAYCAMERAS_API, FCameraPoseDebugBlock)

public:

	/** Creates a new camera pose debug block. */
	FCameraPoseDebugBlock();
	/** Creates a new camera pose debug block. */
	FCameraPoseDebugBlock(const FCameraPose& InCameraPose);

	/** Sets whether the camera pose values should be printed in the text HUD. */
	FCameraPoseDebugBlock& ShouldDrawText(bool bShouldDraw)
	{
		bDrawText = bShouldDraw;
		return *this;
	}

	/** Sets whether the camera pose should be drawn when in external debug rendering. */
	FCameraPoseDebugBlock& ShouldDrawInExternalRendering(bool bShouldDraw)
	{
		bDrawInExternalRendering = bShouldDraw;
		return *this;
	}

	/** Sets the external rendering color. */
	FCameraPoseDebugBlock& SetExternalRenderingLineColor(const FLinearColor& LineColor)
	{
		CameraPoseLineColor = LineColor;
		return *this;
	}

	/** Sets the external rendering size. */
	FCameraPoseDebugBlock& SetExternalRenderingSize(float CameraSize)
	{
		CameraPoseSize = CameraSize;
		return *this;
	}

	/** 
	 * Specifies the console variable to use to toggle between only showing camera pose
	 * properties that were written to, or showing all camera pose properties.
	 */
	FCameraPoseDebugBlock& WithShowUnchangedCVar(const TCHAR* InShowUnchangedCVarName)
	{
		ShowUnchangedCVarName = InShowUnchangedCVarName;
		return *this;
	}

protected:

	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;
	virtual void OnSerialize(FArchive& Ar) override;

private:

	FCameraPose CameraPose;
	FString ShowUnchangedCVarName;
	FLinearColor CameraPoseLineColor;
	float CameraPoseSize = -1.f;
	bool bDrawText = true;
	bool bDrawInExternalRendering = true;
};

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

