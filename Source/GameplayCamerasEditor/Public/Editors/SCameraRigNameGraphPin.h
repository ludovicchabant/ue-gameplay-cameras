// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SGraphPin.h"

#include "ContentBrowserModule.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SListView.h"

namespace UE::Cameras
{

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

	FText GetDefaultComboText() const;
	FText OnGetComboText() const;
	FText OnGetComboToolTipText() const;

	TSharedRef<SWidget> OnBuildCameraRigNamePicker();
	void OnPickerAssetSelected(const FAssetData& AssetData);

	FReply OnResetButtonClicked();

	TSharedRef<ITableRow> OnCameraRigListGenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnCameraRigListSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	void UpdateListItemsSource(const TArray<FAssetData>& Assets);

	void SetCameraRigName(const FString& InCameraRigName);

private:

	TSharedPtr<SComboButton> CameraRigPickerButton;

	FGetCurrentSelectionDelegate GetCurrentAssetPickerSelection;

	TSharedPtr<SListView<TSharedPtr<FString>>> CameraRigNameListView;
	TArray<TSharedPtr<FString>> CameraRigsItemsSource;
	bool bSuppressCameraRigListSelectionChanged = false;
};

}  // namespace UE::Cameras

