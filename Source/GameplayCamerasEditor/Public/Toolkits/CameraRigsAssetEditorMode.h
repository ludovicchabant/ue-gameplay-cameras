// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/SCameraRigList.h"
#include "Toolkits/AssetEditorMode.h"

class FDocumentTabFactory;
class FDocumentTracker;
class UCameraAsset;
class UCameraRigAsset;
class UEdGraphNode;
struct FFindInObjectTreeGraphSource;

namespace UE::Cameras
{

class FCameraRigAssetEditorToolkitBase;

class FCameraRigsAssetEditorMode
	: public FAssetEditorMode
{
public:

	static FName ModeName;

	FCameraRigsAssetEditorMode(UCameraAsset* InCameraAsset);

	void OnGetRootObjectsToSearch(TArray<FFindInObjectTreeGraphSource>& OutSources);
	bool JumpToObject(UObject* InObject, FName PropertyName);

protected:

	virtual void OnActivateMode(const FAssetEditorModeActivateParams& InParams) override;
	virtual void OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams) override;

private:

	TSharedRef<SDockTab> SpawnTab_CameraRigs(const FSpawnTabArgs& Args);

	void OnCameraRigListChanged(TArrayView<UCameraRigAsset* const> InCameraRigs);
	void OnCameraRigEditRequested(UCameraRigAsset* InCameraRig);
	void OnCameraRigDeleted(const TArray<UCameraRigAsset*>& InCameraRigs);

private:

	static const FName CameraRigsTabId;

	UCameraAsset* CameraAsset;

	TSharedPtr<FCameraRigAssetEditorToolkitBase> Impl;

	TSharedPtr<SCameraRigList> CameraRigsListWidget;

	FObjectTreeGraphConfig NodeGraphConfig;
	FObjectTreeGraphConfig TransitionGraphConfig;

	bool bInitializedToolkit = false;
};

}  // namespace UE::Cameras

