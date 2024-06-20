// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigNameGraphPin.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Framework/Views/ITypedTableView.h"
#include "IContentBrowserSingleton.h"
#include "IGameplayCamerasEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Modules/ModuleManager.h"
#include "ScopedTransaction.h"
#include "SGraphPin.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
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
	FCameraRigPickerConfig CameraRigPickerConfig;
	CameraRigPickerConfig.bCanSelectCameraAsset = true;
	CameraRigPickerConfig.CameraAssetSelectionMode = ESelectionMode::Multi;
	CameraRigPickerConfig.CameraAssetViewType = EAssetViewType::List;
	CameraRigPickerConfig.CameraAssetSaveSettingsName = TEXT("CameraRigNamePicker");
	CameraRigPickerConfig.OnCameraRigSelected = FOnCameraRigSelected::CreateSP(this, &SCameraRigNameGraphPin::OnPickerAssetSelected);

	if (TSharedPtr<SGraphNode> OwnerNodeWidget = OwnerNodePtr.Pin())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

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
			CameraRigPickerConfig.InitialCameraAssetSelection = ReferencerAssetData[0];
		}
	}

	if (GraphPinObj && !GraphPinObj->DefaultValue.IsEmpty())
	{
		CameraRigPickerConfig.InitialCameraRigSelectionName = GraphPinObj->DefaultValue;
	}

	IGameplayCamerasEditorModule& CamerasEditorModule = FModuleManager::LoadModuleChecked<IGameplayCamerasEditorModule>("GameplayCamerasEditor");
	return CamerasEditorModule.CreateCameraRigPicker(CameraRigPickerConfig);
}

void SCameraRigNameGraphPin::OnPickerAssetSelected(UCameraRigAsset* SelectedItem)
{
	if (SelectedItem)
	{
		CameraRigPickerButton->SetIsOpen(false);
		SetCameraRigName(SelectedItem->GetDisplayName());
	}
}

FReply SCameraRigNameGraphPin::OnResetButtonClicked()
{
	CameraRigPickerButton->SetIsOpen(false);
	SetCameraRigName(FString());
	return FReply::Handled();
}

void SCameraRigNameGraphPin::SetCameraRigName(const FString& InCameraRigName)
{
	const FScopedTransaction Transaction(LOCTEXT("ChangeObjectPinValue", "Change Object Pin Value"));
	GraphPinObj->Modify();
	GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, InCameraRigName);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

