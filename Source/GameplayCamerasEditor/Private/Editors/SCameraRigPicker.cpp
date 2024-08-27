// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigPicker.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Views/ITypedTableView.h"
#include "Layout/WidgetPath.h"
#include "PropertyHandle.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Types/SlateEnums.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "SCameraRigPicker"

namespace UE::Cameras
{

void SCameraRigPicker::Construct(const FArguments& InArgs)
{
	const FCameraRigPickerConfig& PickerConfig = InArgs._CameraRigPickerConfig;

	const ISlateStyle& AppStyle = FAppStyle::Get();

	SearchTextFilter = MakeShareable(new FTextFilter(
		FTextFilter::FItemToStringArray::CreateSP(this, &SCameraRigPicker::GetEntryStrings)));

	TSharedRef<SVerticalBox> LayoutBox = SNew(SVerticalBox);
	const float CamerRigPickerFillHeight = PickerConfig.bCanSelectCameraAsset ? 0.45f : 1.f;

	// Camera asset picker.
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

		AssetPickerConfig.SelectionMode = ESelectionMode::Single;
		AssetPickerConfig.InitialAssetViewType = PickerConfig.CameraAssetViewType;
		AssetPickerConfig.SaveSettingsName = PickerConfig.CameraAssetSaveSettingsName;

		AssetPickerConfig.InitialAssetSelection = PickerConfig.InitialCameraAssetSelection;

		LayoutBox->AddSlot()
		.FillHeight(0.55f)
		[
			ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
		];
	}
	// Which camera asset is being shown.
	else
	{
		LayoutBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4.f, 4.f, 0.f, 4.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CameraAssetInfo", "Showing camera rigs from "))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.f, 4.f)
			[
				SNew(SHyperlink)
				.Text(this, &SCameraRigPicker::GetSelectedCameraAssetName)
				.OnNavigate(this, &SCameraRigPicker::NavigateToSelectedCameraAsset)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(4.f, 4.f, 4.f, 4.f)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FAppStyle::Get().GetBrush("Icons.BrowseContent"))
			]
		];
	}

	// Search box.
	LayoutBox->AddSlot()
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
	];

	// List of camera rig names.
	LayoutBox->AddSlot()
	.FillHeight(CamerRigPickerFillHeight)
	.Padding(0.f, 3.f)
	[
		SAssignNew(CameraRigListView, SListView<UCameraRigAsset*>)
		.ListItemsSource(&CameraRigFilteredItemsSource)
		.OnGenerateRow(this, &SCameraRigPicker::OnCameraRigListGenerateRow)
		.OnSelectionChanged(this, &SCameraRigPicker::OnCameraRigListSelectionChanged)
	];

	// Number of items in the camera rig list.
	TSharedPtr<SHorizontalBox> MessageBar;
	LayoutBox->AddSlot()
	.AutoHeight()
	[
		SAssignNew(MessageBar, SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(1.f)
		.VAlign(VAlign_Center)
		.Padding(8, 5)
		[
			SNew(STextBlock)
			.Text(this, &SCameraRigPicker::GetCameraRigCountText)
		]
	];

	// Optional warning message.
	if (!PickerConfig.WarningMessage.IsEmpty())
	{
		MessageBar->InsertSlot(0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush("Icons.WarningWithColor"))
			.ToolTipText(PickerConfig.WarningMessage)
		];
	}

	// Optional error message.
	if (!PickerConfig.ErrorMessage.IsEmpty())
	{
		MessageBar->InsertSlot(0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush("Icons.ErrorWithColor"))
			.ToolTipText(PickerConfig.ErrorMessage)
		];
	}
	
	// Assemble it all.
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

	if (!PickerConfig.bCanSelectCameraAsset)
	{
		FixedCameraAssetSelection = PickerConfig.InitialCameraAssetSelection;
	}

	// If we have an initially selected assets, register a timer to do that next frame.
	// When there is only an initially selected camera asset, do the setup with a null camera rig in order
	// to make sure the list of camera rigs is populated in the list view.
	if (PickerConfig.InitialCameraRigSelection)
	{
		TVariant<UCameraRigAsset*, FGuid> SelectedCameraRig(TInPlaceType<UCameraRigAsset*>(), PickerConfig.InitialCameraRigSelection);
		SetupInitialSelections(PickerConfig.InitialCameraAssetSelection, SelectedCameraRig);
	}
	else if (PickerConfig.InitialCameraAssetSelection.IsValid() && PickerConfig.InitialCameraRigSelectionGuid.IsValid())
	{
		TVariant<UCameraRigAsset*, FGuid> SelectedCameraRig(TInPlaceType<FGuid>(), PickerConfig.InitialCameraRigSelectionGuid);
		SetupInitialSelections(PickerConfig.InitialCameraAssetSelection, SelectedCameraRig);
	}
	else if (PickerConfig.InitialCameraAssetSelection.IsValid())
	{
		TVariant<UCameraRigAsset*, FGuid> NullCameraRig(TInPlaceType<UCameraRigAsset*>(), nullptr);
		SetupInitialSelections(PickerConfig.InitialCameraAssetSelection, NullCameraRig);
	}

	// If we need to focus the search box, register a timer to do that next frame.
	if (PickerConfig.bFocusCameraRigSearchBoxWhenOpened)
	{
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SCameraRigPicker::FocusCameraRigSearchBox));
	}

	// Keep track of miscellaneous stuff.
	OnCameraRigSelected = PickerConfig.OnCameraRigSelected;
	PropertyToSet = PickerConfig.PropertyToSet;
}

void SCameraRigPicker::SetupInitialSelections(const FAssetData& InSelectedCameraAssetData, TVariant<UCameraRigAsset*, FGuid> InSelectedCameraRig)
{
	UCameraAsset* SelectedCameraAsset = Cast<UCameraAsset>(InSelectedCameraAssetData.GetAsset());
	UpdateCameraRigItemsSource(SelectedCameraAsset);
	UpdateCameraRigFilteredItemsSource();

	UCameraRigAsset* InitialCameraRigSelection = nullptr;
	if (InSelectedCameraRig.IsType<UCameraRigAsset*>())
	{
		InitialCameraRigSelection = InSelectedCameraRig.Get<UCameraRigAsset*>();
	}
	if (InSelectedCameraRig.IsType<FGuid>())
	{
		if (UCameraAsset* CameraAsset = GetSelectedCameraAsset())
		{
			const FGuid CameraRigGuid = InSelectedCameraRig.Get<FGuid>();
			const TObjectPtr<UCameraRigAsset>* FoundItem = CameraAsset->GetCameraRigs().FindByPredicate(
					[&CameraRigGuid](UCameraRigAsset* Item)
					{
						return Item->GetGuid() == CameraRigGuid;
					});
			if (FoundItem)
			{
				InitialCameraRigSelection = *FoundItem;
			}
		}
	}
	if (InitialCameraRigSelection)
	{
		CameraRigListView->RequestScrollIntoView(InitialCameraRigSelection);
		CameraRigListView->SetSelection(InitialCameraRigSelection);
	}
}

EActiveTimerReturnType SCameraRigPicker::FocusCameraRigSearchBox(double InCurrentTime, float InDeltaTime)
{
	if (SearchBox)
	{
		FWidgetPath WidgetToFocusPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchBox.ToSharedRef(), WidgetToFocusPath);
		FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
		if (WidgetToFocusPath.IsValid())
		{
			WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(SearchBox);
			return EActiveTimerReturnType::Stop;
		}
	}

	return EActiveTimerReturnType::Continue;
}

void SCameraRigPicker::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpdateItemsSource)
	{
		UpdateCameraRigItemsSource();
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

UCameraAsset* SCameraRigPicker::GetSelectedCameraAsset() const
{
	if (GetCurrentCameraAssetPickerSelection.IsBound())
	{
		TArray<FAssetData> Selection = GetCurrentCameraAssetPickerSelection.Execute();
		if (!Selection.IsEmpty())
		{
			return Cast<UCameraAsset>(Selection[0].GetAsset());
		}
		return nullptr;
	}
	else
	{
		return Cast<UCameraAsset>(FixedCameraAssetSelection.GetAsset());
	}
}

FText SCameraRigPicker::GetSelectedCameraAssetName() const
{
	if (UCameraAsset* CameraAsset = GetSelectedCameraAsset())
	{
		return FText::FromName(CameraAsset->GetFName());
	}
	return LOCTEXT("NoCameraAssetName", "None");
}

void SCameraRigPicker::NavigateToSelectedCameraAsset() const
{
	if (UCameraAsset* CameraAsset = GetSelectedCameraAsset())
	{
		FAssetData AssetData(CameraAsset);
		GEditor->SyncBrowserToObject(AssetData);
	}
}

void SCameraRigPicker::OnCameraAssetSelected(const FAssetData& AssetData)
{
	bUpdateItemsSource = true;
}

void SCameraRigPicker::OnCameraRigListSelectionChanged(UCameraRigAsset* Item, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo != ESelectInfo::Direct)
	{
		if (PropertyToSet)
		{
			FProperty* Property = PropertyToSet->GetProperty();
			if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
			{
				if (ensure(ObjectProperty->PropertyClass && ObjectProperty->PropertyClass->IsChildOf<UCameraRigAsset>()))
				{
					PropertyToSet->SetValue(Item);
				}
			}
			else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
			{
				PropertyToSet->SetValue(Item->GetDisplayName());
			}
			else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				if (StructProperty->Struct && StructProperty->Struct == TBaseStructure<FGuid>::Get())
				{
					PropertyToSet->SetValue(Item->GetGuid().ToString());
				}
			}
			else
			{
				ensureMsgf(false, TEXT("Don't know how to set camera rig on property: %s"), *Property->GetFullName());
			}
		}

		OnCameraRigSelected.ExecuteIfBound(Item);
	}
}

void SCameraRigPicker::UpdateCameraRigItemsSource(UCameraAsset* InCameraAsset)
{
	UCameraAsset* CameraAsset = InCameraAsset ? InCameraAsset : GetSelectedCameraAsset();
	if (CameraAsset)
	{
		CameraRigItemsSource = CameraAsset->GetCameraRigs();
	}
	else
	{
		CameraRigItemsSource.Reset();
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

FText SCameraRigPicker::GetCameraRigCountText() const
{
	const int32 NumCameraRigs = CameraRigFilteredItemsSource.Num();

	FText CountText = FText::GetEmpty();
	if (NumCameraRigs == 1)
	{
		CountText = LOCTEXT("CameraRigCountTextSingular", "1 item");
	}
	else
	{
		CountText = FText::Format(LOCTEXT("CameraRigCountTextPlural", "{0} items"), FText::AsNumber(NumCameraRigs));
	}
	return CountText;
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

