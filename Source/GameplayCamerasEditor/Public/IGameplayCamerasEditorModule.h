// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class UCameraAsset;
class UCameraAssetEditor;
class UCameraMode;
class UCameraModeAssetEditor;

/**
 * The gameplay cameras editor module.
 */
class IGameplayCamerasEditorModule : public IModuleInterface
{
public:

	static const FName GameplayCamerasEditorAppIdentifier;

	virtual ~IGameplayCamerasEditorModule() = default;

	/** Creates an editor for the given camera asset */
	virtual UCameraAssetEditor* CreateCameraAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraAsset* CameraAsset) = 0;

	/** Creates an editor for the given camera mode asset */
	virtual UCameraModeAssetEditor* CreateCameraModeEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraMode* CameraMode) = 0;
};

