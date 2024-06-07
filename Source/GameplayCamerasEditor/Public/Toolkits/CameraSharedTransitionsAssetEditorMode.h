// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Toolkits/AssetEditorMode.h"

class UCameraAsset;
class UEdGraphNode;
struct FFindInObjectTreeGraphSource;

namespace UE::Cameras
{

class FCameraRigTransitionEditorToolkitBase;

class FCameraSharedTransitionsAssetEditorMode
	: public FAssetEditorMode
{
public:

	static FName ModeName;

	FCameraSharedTransitionsAssetEditorMode(UCameraAsset* InCameraAsset);

	void OnGetRootObjectsToSearch(TArray<FFindInObjectTreeGraphSource>& OutSources);
	bool JumpToObject(UObject* InObject, FName PropertyName);

protected:

	virtual void OnActivateMode(const FAssetEditorModeActivateParams& InParams) override;
	virtual void OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams) override;

private:

	UCameraAsset* CameraAsset;

	TSharedPtr<FCameraRigTransitionEditorToolkitBase> Impl;

	FObjectTreeGraphConfig TransitionGraphConfig;

	bool bInitializedToolkit = false;
};

}  // namespace UE::Cameras

