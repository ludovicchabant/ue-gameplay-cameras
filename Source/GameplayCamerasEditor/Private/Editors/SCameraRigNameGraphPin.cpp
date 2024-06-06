// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigNameGraphPin.h"

#include "Algo/Transform.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Framework/Views/ITypedTableView.h"
#include "IContentBrowserSingleton.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "SGraphPin.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SCameraRigNameGraphPin"

namespace UE::Cameras
{

void SCameraRigNameGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget>	SCameraRigNameGraphPin::GetDefaultValueWidget()
{
	if (!GraphPinObj)
	{
		return SNullWidget::NullWidget;
	}

	return SNew(SHorizontalBox)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2.f)
		.MaxWidth(200.f)
		[
			SAssignNew(CameraRigPickerButton, SComboButton)
			.ButtonStyle(FAppStyle::Get(), "PropertyEditor.AssetComboStyle")
			.ContentPadding(FMargin(2.f, 2.f, 2.f, 1.f))
			.ForegroundColor(this, &SCameraRigNameGraphPin::OnGetComboForeground)
			.ButtonColorAndOpacity(this, &SCameraRigNameGraphPin::OnGetWidgetBackground)
			.MenuPlacement(MenuPlacement_BelowAnchor)
			.IsEnabled(this, &SGraphPin::IsEditingEnabled)
			.ButtonContent()
			[
				SNew(STextBlock)
				.ColorAndOpacity(this, &SCameraRigNameGraphPin::OnGetComboForeground)
				.TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
				.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.Text(this, &SCameraRigNameGraphPin::OnGetComboText)
				.ToolTipText(this, &SCameraRigNameGraphPin::OnGetComboToolTipText)
			]
			.OnGetMenuContent(this, &SCameraRigNameGraphPin::OnBuildCameraRigNamePicker)
		]
		// Reset button
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(1,0)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.ButtonColorAndOpacity(this, &SCameraRigNameGraphPin::OnGetWidgetBackground)
			.OnClicked(this, &SCameraRigNameGraphPin::OnResetButtonClicked)
			.ContentPadding(1.f)
			.ToolTipText(LOCTEXT("ResetButtonToolTip", "Reset the name to an empty string."))
			.IsEnabled(this, &SGraphPin::IsEditingEnabled)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SCameraRigNameGraphPin::OnGetWidgetForeground)
				.Image(FAppStyle::GetBrush(TEXT("Icons.CircleArrowLeft")))
			]
		];
}

bool SCameraRigNameGraphPin::DoesWidgetHandleSettingEditingEnabled() const
{
	return true;
}

FSlateColor SCameraRigNameGraphPin::OnGetComboForeground() const
{
	float Alpha = (IsHovered() || bOnlyShowDefaultValue) ? ActiveComboAlpha : InactiveComboAlpha;
	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha));
}

FSlateColor SCameraRigNameGraphPin::OnGetWidgetForeground() const
{
	float Alpha = (IsHovered() || bOnlyShowDefaultValue) ? ActivePinForegroundAlpha : InactivePinForegroundAlpha;
	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha));
}

FSlateColor SCameraRigNameGraphPin::OnGetWidgetBackground() const
{
	float Alpha = (IsHovered() || bOnlyShowDefaultValue) ? ActivePinBackgroundAlpha : InactivePinBackgroundAlpha;
	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha));
}

FText SCameraRigNameGraphPin::GetDefaultComboText() const
{
	return LOCTEXT("DefaultComboText", "Select Camera Rig");
}

FText SCameraRigNameGraphPin::OnGetComboText() const
{
	FText Value = GetDefaultComboText();
	
	if (GraphPinObj != nullptr)
	{
		if (!GraphPinObj->DefaultValue.IsEmpty())
		{
			Value = FText::FromString(GraphPinObj->DefaultValue);
		}
	}
	return Value;
}

FText SCameraRigNameGraphPin::OnGetComboToolTipText() const
{
	return LOCTEXT("ComboToolTipText", "The name of the camera rig to activate.");
}

TSharedRef<SWidget> SCameraRigNameGraphPin::OnBuildCameraRigNamePicker()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.bAllowDragging = false;
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.SelectionMode = ESelectionMode::Multi;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
	AssetPickerConfig.Filter.ClassPaths.Add(UCameraAsset::StaticClass()->GetClassPathName());
	AssetPickerConfig.Filter.bRecursiveClasses = true;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SCameraRigNameGraphPin::OnPickerAssetSelected);
	AssetPickerConfig.GetCurrentSelectionDelegates.Add(&GetCurrentAssetPickerSelection);

	if (TSharedPtr<SGraphNode> OwnerNodeWidget = OwnerNodePtr.Pin())
	{
		UEdGraphNode* OwnerNode = OwnerNodeWidget->GetNodeObj();
		UBlueprint* OwnerBlueprint = FBlueprintEditorUtils::FindBlueprintForNode(OwnerNode);

		FARFilter Filter;
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		AssetRegistry.GetReferencers(OwnerBlueprint->GetOutermost()->GetFName(), Filter.PackageNames);
		TArray<FAssetData> ReferencerAssetData;
		AssetRegistry.GetAssets(Filter, ReferencerAssetData);
		if (!ReferencerAssetData.IsEmpty())
		{
			//TODO: multi-select all referencers.
			AssetPickerConfig.InitialAssetSelection = ReferencerAssetData[0];
		}

		UpdateListItemsSource(ReferencerAssetData);
	}

	TSharedRef<SWidget> PickerWidget = SNew(SBox)
		.HeightOverride(600)
		.WidthOverride(350)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Menu.Background"))
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(0.6f)
				[
					ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
				]
				+SVerticalBox::Slot()
				.FillHeight(0.4f)
				[
					SAssignNew(CameraRigNameListView, SListView<TSharedPtr<FString>>)
					.ListItemsSource(&CameraRigsItemsSource)
					.OnGenerateRow(this, &SCameraRigNameGraphPin::OnCameraRigListGenerateRow)
					.OnSelectionChanged(this, &SCameraRigNameGraphPin::OnCameraRigListSelectionChanged)
				]
			]
		];

	if (GraphPinObj && !GraphPinObj->DefaultValue.IsEmpty())
	{
		TSharedPtr<FString>* FoundItem = CameraRigsItemsSource.FindByPredicate(
				[this](const TSharedPtr<FString>& Item)
				{
					return GraphPinObj->DefaultValue == *Item.Get();
				});
		if (FoundItem)
		{
			bSuppressCameraRigListSelectionChanged = true;
			CameraRigNameListView->SetSelection(*FoundItem);
			CameraRigNameListView->RequestScrollIntoView(*FoundItem);
			bSuppressCameraRigListSelectionChanged = false;
		}
	}

	return PickerWidget;
}

void SCameraRigNameGraphPin::OnPickerAssetSelected(const FAssetData& AssetData)
{
	TArray<FAssetData> SelectedAssets;
	if (GetCurrentAssetPickerSelection.IsBound())
	{
		SelectedAssets = GetCurrentAssetPickerSelection.Execute();
	}

	UpdateListItemsSource(SelectedAssets);
	CameraRigNameListView->RequestListRefresh();
}

FReply SCameraRigNameGraphPin::OnResetButtonClicked()
{
	CameraRigPickerButton->SetIsOpen(false);
	SetCameraRigName(FString());
	return FReply::Handled();
}

TSharedRef<ITableRow> SCameraRigNameGraphPin::OnCameraRigListGenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasStyle = FGameplayCamerasEditorStyle::Get();

	const FText DisplayName = FText::FromString(*Item);

	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
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
				.Text(DisplayName)
			]
		];
}

void SCameraRigNameGraphPin::OnCameraRigListSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	if (!bSuppressCameraRigListSelectionChanged && Item)
	{
		CameraRigPickerButton->SetIsOpen(false);
		SetCameraRigName(*Item.Get());
	}
}

void SCameraRigNameGraphPin::UpdateListItemsSource(const TArray<FAssetData>& Assets)
{
	// Only show the names that are in common between all selected camera assets.
	TSet<FString> CommonNames;
	bool bCommonNamesInitialized = false;
	for (const FAssetData& SelectedAsset : Assets)
	{
		if (const UCameraAsset* CameraAsset = Cast<UCameraAsset>(SelectedAsset.GetAsset()))
		{
			TSet<FString> SelectedAssetNames;
			Algo::Transform(CameraAsset->CameraRigs, SelectedAssetNames, 
					[](const UCameraRigAsset* Item) { return Item->GetDisplayName(); });

			if (!bCommonNamesInitialized)
			{
				CommonNames.Append(SelectedAssetNames);
				bCommonNamesInitialized = true;
			}
			else
			{
				CommonNames = CommonNames.Intersect(SelectedAssetNames);
			}
		}
	}

	CameraRigsItemsSource.Reset();
	for (const FString& CommonName : CommonNames)
	{
		CameraRigsItemsSource.Add(MakeShared<FString>(CommonName));
	}
}

void SCameraRigNameGraphPin::SetCameraRigName(const FString& InCameraRigName)
{
	const FScopedTransaction Transaction(LOCTEXT("ChangeObjectPinValue", "Change Object Pin Value"));
	GraphPinObj->Modify();
	GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, InCameraRigName);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

