// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Toolkits/AssetEditorMode.h"
#include "Editors/SCameraRigList.h"

class FDocumentTabFactory;
class FDocumentTracker;
class UCameraAsset;
class UCameraRigAsset;
class UEdGraphNode;

namespace UE::Cameras
{

class FCameraRigAssetEditorToolkitBase;

class FCameraRigsAssetEditorMode
	: public FAssetEditorMode
{
public:

	static FName ModeName;

	FCameraRigsAssetEditorMode(UCameraAsset* InCameraAsset);

	bool JumpToNode(UEdGraphNode* InNode);
	bool JumpToObject(UObject* InObject);

protected:

	virtual void OnActivateMode(const FAssetEditorModeActivateParams& InParams) override;
	virtual void OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams) override;

private:

	TSharedRef<SDockTab> SpawnTab_CameraRigs(const FSpawnTabArgs& Args);

	void OnCameraRigListChanged(TArrayView<UCameraRigAsset* const> InCameraRigs);
	void OnCameraRigEditRequested(UCameraRigAsset* InCameraRig);

private:

	static const FName CameraRigsTabId;

	UCameraAsset* CameraAsset;

	TSharedPtr<FCameraRigAssetEditorToolkitBase> Impl;

	TSharedPtr<SCameraRigList> CameraRigsListWidget;

	bool bInitializedToolkit = false;
};

}  // namespace UE::Cameras

