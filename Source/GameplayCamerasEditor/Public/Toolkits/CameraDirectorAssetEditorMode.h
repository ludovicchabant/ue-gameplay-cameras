// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Toolkits/AssetEditorMode.h"

class IDetailsView;
class UCameraAsset;
class UEdGraphNode;

namespace UE::Cameras
{

class FStandardToolkitLayout;

class FCameraDirectorAssetEditorMode
	: public FAssetEditorMode
{
public:

	static FName ModeName;

	FCameraDirectorAssetEditorMode(UCameraAsset* InCameraAsset);

	bool JumpToObject(UObject* InObject, FName InPropertyName);

protected:

	virtual void OnActivateMode(const FAssetEditorModeActivateParams& InParams) override;
	virtual void OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams) override;

private:

	TSharedRef<SDockTab> SpawnTab_DirectorEditor(const FSpawnTabArgs& Args);

private:

	static FName DirectorEditorTabId;

	UCameraAsset* CameraAsset;

	TSharedPtr<FStandardToolkitLayout> StandardLayout;

	TSharedPtr<IDetailsView> DetailsView;

	bool bInitializedToolkit = false;
};

}  // namespace UE::Cameras

