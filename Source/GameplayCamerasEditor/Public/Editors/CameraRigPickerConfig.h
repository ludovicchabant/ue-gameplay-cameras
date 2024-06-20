// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Views/ITypedTableView.h"
#include "IContentBrowserSingleton.h"
#include "UObject/ObjectKey.h"

class IPropertyHandle;
class UCameraRigAsset;

namespace UE::Cameras
{

DECLARE_DELEGATE_OneParam(FOnCameraRigSelected, UCameraRigAsset*);

/**
 * Configuration structure for a camera rig picker widget.
 * This is a widget that shows:
 *	- An asset picker for a camera asset, and below it...
 *	- A list view with the camera rigs inside that camera asset.
 *
 * See IGameplayCamerasEditorModule for creating that widget.
 */
struct FCameraRigPickerConfig
{
	/** The initially selected camera asset, if any. */
	FAssetData InitialCameraAssetSelection;

	/** 
	 * The initially selected camera rig, specified as a pointer, if any.
	 * This shouldn't be set if InitialCameraAssetSelectionName is set.
	 */
	UCameraRigAsset* InitialCameraRigSelection;

	/**
	 * The initially selected camera rig, picked by name, if any.
	 * This shouldn't be set if InitialCameraAssetSelection is set.
	 */
	FString InitialCameraRigSelectionName;

	/** 
	 * Whether a camera asset can be picked, or whether the InitialCameraAssetSelection
	 * is the only asset that should be used. If the latter (when the value is false),
	 * then no camera asset picker is shown. Only the list of rigs is shown.
	 */
	bool bCanSelectCameraAsset = false;

	/** Asset picker view type for the camera asset picker. */
	EAssetViewType::Type CameraAssetViewType = EAssetViewType::List;
	/** Asset picker selection mode for the camera asset picker. */
	ESelectionMode::Type CameraAssetSelectionMode = ESelectionMode::Single;
	/** Asset picker settings name for the camera asset picker. */
	FString CameraAssetSaveSettingsName;

	/** Whether the camera rig search box should be focused initially. */
	bool bFocusCameraRigSearchBoxWhenOpened = true;

	/** Callback for when a camera rig has been selected. */
	FOnCameraRigSelected OnCameraRigSelected;

	TSharedPtr<IPropertyHandle> PropertyToSet;
};

}  // namespace UE::Cameras

