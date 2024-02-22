// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class UCameraAsset;
class UCameraAssetEditor;
class UCameraRigAsset;
class UCameraRigAssetEditor;

/**
 * The gameplay cameras editor module.
 */
class IGameplayCamerasEditorModule : public IModuleInterface
{
public:

	static const FName GameplayCamerasEditorAppIdentifier;

	static const FName CameraRigAssetEditorToolBarName;

	virtual ~IGameplayCamerasEditorModule() = default;

public:

	/** Creates an editor for the given camera asset */
	virtual UCameraAssetEditor* CreateCameraAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraAsset* CameraAsset) = 0;

	/** Creates an editor for the given camera rig asset */
	virtual UCameraRigAssetEditor* CreateCameraRigEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCameraRigAsset* CameraRig) = 0;
};

