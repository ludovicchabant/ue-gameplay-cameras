// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigPicker.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "IContentBrowserSingleton.h"
#include "Layout/WidgetPath.h"
#include "PropertyHandle.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Types/SlateEnums.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "SCameraRigPicker"

namespace UE::Cameras
{

void SCameraRigPicker::Construct(const FArguments& InArgs)
{
	const FCameraRigPickerConfig& PickerConfig = InArgs._CameraRigPickerConfig;

	SearchTextFilter = MakeShareable(new FTextFilter(
		FTextFilter::FItemToStringArray::CreateSP(this, &SCameraRigPicker::GetEntryStrings)));

	// Camera asset picker.
	TSharedRef<SVerticalBox> LayoutBox = SNew(SVerticalBox);

	if (PickerConfig.bCanSelectCameraAsset)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

		FAssetPickerConfig AssetPickerConfig;
		AssetPickerConfig.bAllowDragging = false;
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.Filter.ClassPaths.Add(UCameraAsset::StaticClass()->GetClassPathName());
		AssetPickerConfig.Filter.bRecursiveClasses = true;
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SCameraRigPicker::OnCameraAssetSelected);
		AssetPickerConfig.GetCurrentSelectionDelegates.Add(&GetCurrentCameraAssetPickerSelection);

		AssetPickerConfig.SelectionMode = PickerConfig.CameraAssetSelectionMode;
		AssetPickerConfig.InitialAssetViewType = PickerConfig.CameraAssetViewType;
		AssetPickerConfig.SaveSettingsName = PickerConfig.CameraAssetSaveSettingsName;
		AssetPickerConfig.InitialAssetSelection = PickerConfig.InitialCameraAssetSelection;

		LayoutBox->AddSlot()
		.FillHeight(0.55f)
		[
			ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
		];
	}

	// Camera rig picker.
	const float CamerRigPickerFillHeight = PickerConfig.bCanSelectCameraAsset ? 0.45f : 1.f;
	LayoutBox->AddSlot()
	.FillHeight(CamerRigPickerFillHeight)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(8.f)
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("SearchHint", "Search"))
				.OnTextChanged(this, &SCameraRigPicker::OnSearchTextChanged)
				.OnTextCommitted(this, &SCameraRigPicker::OnSearchTextCommitted)
				.OnKeyDownHandler(this, &SCameraRigPicker::OnSearchKeyDown)
			]
		]
		+SVerticalBox::Slot()
		.Padding(0.f, 3.f)
		[
			SAssignNew(CameraRigListView, SListView<UCameraRigAsset*>)
			.ListItemsSource(&CameraRigFilteredItemsSource)
			.OnGenerateRow(this, &SCameraRigPicker::OnCameraRigListGenerateRow)
			.OnSelectionChanged(this, &SCameraRigPicker::OnCameraRigListSelectionChanged)
		]
	];
	
	// Full layout.
	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(400)
		.WidthOverride(350)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Menu.Background"))
			[
				LayoutBox
			]
		]
	];

	// If we have an initially selected camera asset, fill the list of camera rigs immediately.
	if (PickerConfig.InitialCameraAssetSelection.IsValid())
	{
		UpdateCameraRigItemsSource(TArray<FAssetData>{ PickerConfig.InitialCameraAssetSelection });
	}

	UpdateCameraRigFilteredItemsSource();

	// If we have an initially selected camera rig, select it in the list immediately too.
	UCameraRigAsset* InitialCameraRigSelection = PickerConfig.InitialCameraRigSelection;
	if (!PickerConfig.InitialCameraRigSelectionName.IsEmpty() && !InitialCameraRigSelection)
	{
		UCameraRigAsset** FoundItem = CameraRigFilteredItemsSource.FindByPredicate(
				[&PickerConfig](UCameraRigAsset* Item)
				{
					return Item->GetDisplayName() == PickerConfig.InitialCameraRigSelectionName;
				});
		if (FoundItem)
		{
			InitialCameraRigSelection = *FoundItem;
		}
	}
	if (InitialCameraRigSelection)
	{
		CameraRigListView->RequestScrollIntoView(InitialCameraRigSelection);
		CameraRigListView->SetSelection(InitialCameraRigSelection);
	}

	// If we need to focus the search box, register a time to do that next frame.
	if (PickerConfig.bFocusCameraRigSearchBoxWhenOpened)
	{
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SCameraRigPicker::FocusCameraRigSearchBox));
	}

	// Keep track of miscellaneous stuff.
	OnCameraRigSelected = PickerConfig.OnCameraRigSelected;
	PropertyToSet = PickerConfig.PropertyToSet;
}

EActiveTimerReturnType SCameraRigPicker::FocusCameraRigSearchBox(double InCurrentTime, float InDeltaTime)
{
	if (SearchBox)
	{
		FWidgetPath WidgetToFocusPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchBox.ToSharedRef(), WidgetToFocusPath);
		FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
		WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(SearchBox);
		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

void SCameraRigPicker::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpdateItemsSource)
	{
		if (GetCurrentCameraAssetPickerSelection.IsBound())
		{
			TArray<FAssetData> SelectedAssets = GetCurrentCameraAssetPickerSelection.Execute();
			UpdateCameraRigItemsSource(SelectedAssets);
		}
	}
	if (bUpdateFilteredItemsSource || bUpdateItemsSource)
	{
		UpdateCameraRigFilteredItemsSource();
	}

	const bool bRequestListRefresh = bUpdateItemsSource || bUpdateFilteredItemsSource;
	bUpdateItemsSource = false;
	bUpdateFilteredItemsSource = false;

	if (bRequestListRefresh)
	{
		CameraRigListView->RequestListRefresh();
	}

	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SCameraRigPicker::OnCameraAssetSelected(const FAssetData& AssetData)
{
	bUpdateItemsSource = true;
}

TSharedRef<ITableRow> SCameraRigPicker::OnCameraRigListGenerateRow(UCameraRigAsset* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasStyle = FGameplayCamerasEditorStyle::Get();

	return SNew(STableRow<UCameraRigAsset*>, OwnerTable)
		.Padding(FMargin(2.f))
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("NoBorder"))
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(GameplayCamerasStyle->GetBrush("CameraAssetEditor.ShowCameraRigs"))
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(4.f, 2.f)
				[
					SNew(STextBlock)
					.HighlightText(this, &SCameraRigPicker::GetHighlightText)
					.Text_Lambda([Item]() { return FText::FromString(Item->GetDisplayName()); })
				]
			]
		];
}

void SCameraRigPicker::OnCameraRigListSelectionChanged(UCameraRigAsset* Item, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo != ESelectInfo::Direct)
	{
		if (PropertyToSet)
		{
			PropertyToSet->SetValue(Item);
		}

		OnCameraRigSelected.ExecuteIfBound(Item);
	}
}

void SCameraRigPicker::UpdateCameraRigItemsSource(const TArray<FAssetData>& Assets)
{
	CameraRigItemsSource.Reset();

	for (const FAssetData& SelectedAsset : Assets)
	{
		if (const UCameraAsset* CameraAsset = Cast<UCameraAsset>(SelectedAsset.GetAsset()))
		{
			CameraRigItemsSource.Append(CameraAsset->CameraRigs);
		}
	}
}

void SCameraRigPicker::UpdateCameraRigFilteredItemsSource()
{
	CameraRigFilteredItemsSource = CameraRigItemsSource;
	CameraRigFilteredItemsSource.StableSort([](UCameraRigAsset& A, UCameraRigAsset& B)
			{ 
				return A.GetDisplayName().Compare(B.GetDisplayName()) < 0;
			});

	if (!SearchTextFilter->GetRawFilterText().IsEmpty())
	{
		CameraRigFilteredItemsSource = CameraRigFilteredItemsSource.FilterByPredicate(
				[this](UCameraRigAsset* Item)
				{
					return SearchTextFilter->PassesFilter(Item);
				});
	}
}

void SCameraRigPicker::GetEntryStrings(const UCameraRigAsset* InItem, TArray<FString>& OutStrings)
{
	OutStrings.Add(InItem->GetDisplayName());
}

void SCameraRigPicker::OnSearchTextChanged(const FText& InFilterText)
{
	SearchTextFilter->SetRawFilterText(InFilterText);
	SearchBox->SetError(SearchTextFilter->GetFilterErrorText());

	bUpdateFilteredItemsSource = true;
}

void SCameraRigPicker::OnSearchTextCommitted(const FText& InFilterText, ETextCommit::Type InCommitType)
{
	OnSearchTextChanged(InFilterText);

	if (InCommitType == ETextCommit::OnEnter)
	{
		TArray<UCameraRigAsset*> SelectedItems = CameraRigListView->GetSelectedItems();
		if (SelectedItems.Num() > 0)
		{
			OnCameraRigListSelectionChanged(SelectedItems[0], ESelectInfo::OnKeyPress);
		}
	}
}

FReply SCameraRigPicker::OnSearchKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	int32 SelectionDelta = 0;

	if (InKeyEvent.GetKey() == EKeys::Up)
	{
		SelectionDelta = -1;
	}
	else if (InKeyEvent.GetKey() == EKeys::Down)
	{
		SelectionDelta = +1;
	}

	if (SelectionDelta != 0 && !CameraRigFilteredItemsSource.IsEmpty())
	{
		TArray<UCameraRigAsset*> SelectedItems = CameraRigListView->GetSelectedItems();
		if (SelectedItems.IsEmpty())
		{
			// No items already selected... select the first or last depending on the key pressed.
			if (SelectionDelta > 0)
			{
				CameraRigListView->SetSelection(CameraRigFilteredItemsSource[0]);
			}
			else if (SelectionDelta < 0)
			{
				CameraRigListView->SetSelection(CameraRigFilteredItemsSource.Last());
			}

			return FReply::Handled();
		}

		int32 SelectedIndex = CameraRigFilteredItemsSource.Find(SelectedItems[0]);
		if (ensure(SelectedIndex >= 0))
		{
			// Set the selection to the previous/next item, wrapping around the list.
			SelectedIndex = (SelectedIndex + SelectionDelta + CameraRigFilteredItemsSource.Num()) % CameraRigFilteredItemsSource.Num();
			CameraRigListView->RequestScrollIntoView(CameraRigFilteredItemsSource[SelectedIndex]);
			CameraRigListView->SetSelection(CameraRigFilteredItemsSource[SelectedIndex]);

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FText SCameraRigPicker::GetHighlightText() const
{
	return SearchTextFilter->GetRawFilterText();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

