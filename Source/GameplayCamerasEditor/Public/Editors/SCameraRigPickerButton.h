// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/CameraRigPickerConfig.h"
#include "Widgets/SCompoundWidget.h"

class SComboButton;
class UCameraRigAsset;

namespace UE::Cameras
{

/**
 * A simple combo button that shows a camera rig picker dialog.
 */
class SCameraRigPickerButton : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCameraRigPickerButton)
	{}
		SLATE_ATTRIBUTE(FCameraRigPickerConfig, CameraRigPickerConfig)
		SLATE_ATTRIBUTE(FText, SelectedCameraRigName)
		SLATE_ATTRIBUTE(FText, ButtonToolTipText)
		SLATE_ATTRIBUTE(FSlateColor, ButtonForegroundColor)
		SLATE_ATTRIBUTE(FSlateColor, ButtonColorAndOpacity)
		SLATE_ATTRIBUTE(EMenuPlacement, PickerMenuPlacement)
		SLATE_ATTRIBUTE(bool, IsEnabled)
		SLATE_ATTRIBUTE(UObject*, ShowOnlyCameraAssetsReferencingObject)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	bool IsOpen() const;
	void SetIsOpen(bool bIsOpen);

private:

	FText GetDefaultComboText() const;
	FText GetDefaultComboToolTipText() const;
	FText OnGetComboText() const;

	TSharedRef<SWidget> OnBuildCameraRigNamePicker();

private:

	TSharedPtr<SComboButton> CameraRigPickerButton;

	TAttribute<FCameraRigPickerConfig> CameraRigPickerConfigAttribute;
	TAttribute<FText> OnGetSelectedCameraRigName;
	TAttribute<UObject*> ShowOnlyCameraAssetsReferencingObjectAttribute;
};

}  // namespace UE::Cameras

