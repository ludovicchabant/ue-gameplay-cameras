// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraAsset.h"
#include "Toolkits/CameraDirectorAssetEditorMode.h"

namespace UE::Cameras
{

class FSingleCameraDirectorAssetEditorMode
	: public FCameraDirectorAssetEditorMode
	, public ICameraAssetEventHandler
{
public:

	static TSharedPtr<FCameraDirectorAssetEditorMode> CreateInstance(UCameraAsset* InCameraAsset);

	FSingleCameraDirectorAssetEditorMode(UCameraAsset* InCameraAsset);

protected:

	virtual void OnCameraRigsChanged(UCameraAsset* InCameraAsset, const TCameraArrayChangedEvent<UCameraRigAsset*>& Event) override;

private:

	TCameraEventHandler<ICameraAssetEventHandler> EventHandler;
};

}  // namespace UE::Cameras

