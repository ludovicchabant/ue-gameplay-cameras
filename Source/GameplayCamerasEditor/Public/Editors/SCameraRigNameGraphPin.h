// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ContentBrowserModule.h"
#include "Editors/CameraRigPickerConfig.h"
#include "SGraphPin.h"

class UCameraRigAsset;

namespace UE::Cameras
{

class SCameraRigPickerButton;

/**
 * A custom widget for a graph editor pin that shows a camera rig picker dialog.
 */
class SCameraRigNameGraphPin : public SGraphPin
{
public:

	SLATE_BEGIN_ARGS(SCameraRigNameGraphPin)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:

	// SGraphPin interface.
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual bool DoesWidgetHandleSettingEditingEnabled() const override;

	virtual void OnCustomizeCameraRigPickerConfig(FCameraRigPickerConfig& CameraRigPickerConfig) const {}

private:

	static constexpr float ActiveComboAlpha = 1.f;
	static constexpr float InactiveComboAlpha = 0.6f;
	static constexpr float ActivePinForegroundAlpha = 1.f;
	static constexpr float InactivePinForegroundAlpha = 0.15f;
	static constexpr float ActivePinBackgroundAlpha = 0.8f;
	static constexpr float InactivePinBackgroundAlpha = 0.4f;

	FSlateColor OnGetComboForeground() const;
	FSlateColor OnGetWidgetForeground() const;
	FSlateColor OnGetWidgetBackground() const;

	FCameraRigPickerConfig OnCreateCameraRigPickerConfig() const;
	FText OnGetSelectedCameraRigName() const;
	FText OnGetCameraRigPickerToolTipText() const;
	void OnPickerAssetSelected(UCameraRigAsset* SelectedItem) const;

	FReply OnResetButtonClicked();

	void SetCameraRig(UCameraRigAsset* SelectedCameraRig) const;

protected:

	TSharedPtr<SCameraRigPickerButton> CameraRigPickerButton;
};

}  // namespace UE::Cameras

