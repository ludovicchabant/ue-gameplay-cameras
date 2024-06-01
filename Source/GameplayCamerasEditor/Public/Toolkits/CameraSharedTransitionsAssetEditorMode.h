// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Toolkits/AssetEditorMode.h"

class UCameraAsset;
class UEdGraphNode;

namespace UE::Cameras
{

class FCameraRigTransitionEditorToolkitBase;

class FCameraSharedTransitionsAssetEditorMode
	: public FAssetEditorMode
{
public:

	static FName ModeName;

	FCameraSharedTransitionsAssetEditorMode(UCameraAsset* InCameraAsset);

	bool JumpToNode(UEdGraphNode* InNode);
	bool JumpToObject(UObject* InObject);

protected:

	virtual void OnActivateMode(const FAssetEditorModeActivateParams& InParams) override;
	virtual void OnDeactivateMode(const FAssetEditorModeDeactivateParams& InParams) override;

private:

	UCameraAsset* CameraAsset;

	TSharedPtr<FCameraRigTransitionEditorToolkitBase> Impl;

	bool bInitializedToolkit = false;
};

}  // namespace UE::Cameras

