// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ContentBrowserModule.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Misc/TextFilter.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class SSearchBox;
class UCameraRigAsset;

namespace UE::Cameras
{

class SCameraRigPicker : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCameraRigPicker)
	{}
		SLATE_ARGUMENT(FCameraRigPickerConfig, CameraRigPickerConfig)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:

	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:

	void SetupInitialSelections(const FAssetData& InSelectedCameraAssetData, TVariant<UCameraRigAsset*, FGuid> InSelectedCameraRig);

	EActiveTimerReturnType FocusCameraRigSearchBox(double InCurrentTime, float InDeltaTime);

	UCameraAsset* GetSelectedCameraAsset() const;
	FText GetSelectedCameraAssetName() const;
	void NavigateToSelectedCameraAsset() const;

	void OnCameraAssetSelected(const FAssetData& AssetData);

	TSharedRef<ITableRow> OnCameraRigListGenerateRow(UCameraRigAsset* Item, const TSharedRef<STableViewBase>& OwnerTable);
	void OnCameraRigListSelectionChanged(UCameraRigAsset* Item, ESelectInfo::Type SelectInfo);
	void UpdateCameraRigItemsSource(UCameraAsset* InCameraAsset = nullptr);
	void UpdateCameraRigFilteredItemsSource();
	FText GetCameraRigCountText() const;

	void GetEntryStrings(const UCameraRigAsset* InItem, TArray<FString>& OutStrings);
	void OnSearchTextChanged(const FText& InFilterText);
	void OnSearchTextCommitted(const FText& InFilterText, ETextCommit::Type InCommitType);
	FReply OnSearchKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);
	FText GetHighlightText() const;

private:

	FAssetData FixedCameraAssetSelection;
	FGetCurrentSelectionDelegate GetCurrentCameraAssetPickerSelection;

	TSharedPtr<SListView<UCameraRigAsset*>> CameraRigListView;
	TArray<UCameraRigAsset*> CameraRigItemsSource;
	TArray<UCameraRigAsset*> CameraRigFilteredItemsSource;

	bool bUpdateItemsSource = false;
	bool bUpdateFilteredItemsSource = false;

	using FTextFilter = TTextFilter<const UCameraRigAsset*>;
	TSharedPtr<FTextFilter> SearchTextFilter;
	TSharedPtr<SSearchBox> SearchBox;

	FOnCameraRigSelected OnCameraRigSelected;
	TSharedPtr<IPropertyHandle> PropertyToSet;
};

}  // namespace UE::Cameras

