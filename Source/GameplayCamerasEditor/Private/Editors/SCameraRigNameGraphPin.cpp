// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigNameGraphPin.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Framework/Views/ITypedTableView.h"
#include "Helpers/CameraDirectorHelper.h"
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

	PinMode = InArgs._PinMode;
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
			.ToolTipText(LOCTEXT("ResetButtonToolTip", "Reset the camera rig reference."))
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
		switch (PinMode)
		{
			case ECameraRigNameGraphPinMode::NamePin:
				{
					Value = FText::FromString(GraphPinObj->DefaultValue);
				}
				break;
			case ECameraRigNameGraphPinMode::ReferencePin:
				if (const UCameraRigAsset* CameraRig = Cast<const UCameraRigAsset>(GraphPinObj->DefaultObject))
				{
					Value = FText::FromString(CameraRig->GetDisplayName());
				}
				break;
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
	CameraRigPickerConfig.bCanSelectCameraAsset = false;
	CameraRigPickerConfig.bFocusCameraRigSearchBoxWhenOpened = true;
	CameraRigPickerConfig.OnCameraRigSelected = FOnCameraRigSelected::CreateSP(this, &SCameraRigNameGraphPin::OnPickerAssetSelected);

	TSharedPtr<SGraphNode> OwnerNodeWidget = OwnerNodePtr.Pin();
	check(OwnerNodeWidget);
	
	UEdGraphNode* OwnerNode = OwnerNodeWidget->GetNodeObj();
	UBlueprint* OwnerBlueprint = FBlueprintEditorUtils::FindBlueprintForNode(OwnerNode);

	TArray<UCameraAsset*> ReferencingCameraAssets;
	FCameraDirectorHelper::GetReferencingCameraAssets(OwnerBlueprint, ReferencingCameraAssets);

	if (ReferencingCameraAssets.Num() == 0)
	{
		CameraRigPickerConfig.WarningMessage = LOCTEXT("NoReferencingCameraAssetWarning",
				"No camera asset references this Blueprint, so no camera rig list can be displayed. "
				"Make a camera asset use this Blueprint as its camera director evaluator, or use "
				"ActivateCameraRigViaProxy.");
	}
	else
	{
		CameraRigPickerConfig.InitialCameraAssetSelection = ReferencingCameraAssets[0];

		if (ReferencingCameraAssets.Num() > 1)
		{
			CameraRigPickerConfig.WarningMessage = LOCTEXT("ManyReferencingCameraAssetsWarning",
				"More than one camera asset references this Blueprint. Only camera rigs from the first "
				"one will be displayed. Even then, shared camera director Blueprints should use "
				"ActivateCameraRigViaProxy instead.");
		}
	}

	if (GraphPinObj)
	{
		switch (PinMode)
		{
			case ECameraRigNameGraphPinMode::NamePin:
				if (!ReferencingCameraAssets.IsEmpty())
				{
					TArrayView<const TObjectPtr<UCameraRigAsset>> CameraRigs = ReferencingCameraAssets[0]->GetCameraRigs();
					const TObjectPtr<UCameraRigAsset>* FoundItem = CameraRigs.FindByPredicate(
							[this](UCameraRigAsset* Item)
							{
							return Item->GetDisplayName() == GraphPinObj->DefaultValue;
							});
					if (FoundItem)
					{
						CameraRigPickerConfig.InitialCameraRigSelection = *FoundItem;
					}
				}
				break;
			case ECameraRigNameGraphPinMode::ReferencePin:
				{
					CameraRigPickerConfig.InitialCameraRigSelection = Cast<UCameraRigAsset>(GraphPinObj->DefaultObject);
				}
				break;
		}
	}

	IGameplayCamerasEditorModule& CamerasEditorModule = FModuleManager::LoadModuleChecked<IGameplayCamerasEditorModule>("GameplayCamerasEditor");
	return CamerasEditorModule.CreateCameraRigPicker(CameraRigPickerConfig);
}

void SCameraRigNameGraphPin::OnPickerAssetSelected(UCameraRigAsset* SelectedItem)
{
	if (SelectedItem)
	{
		CameraRigPickerButton->SetIsOpen(false);
		SetCameraRig(SelectedItem);
	}
}

FReply SCameraRigNameGraphPin::OnResetButtonClicked()
{
	CameraRigPickerButton->SetIsOpen(false);
	SetCameraRig(nullptr);
	return FReply::Handled();
}

void SCameraRigNameGraphPin::SetCameraRig(UCameraRigAsset* SelectedCameraRig)
{
	const FScopedTransaction Transaction(LOCTEXT("ChangeObjectPinValue", "Change Object Pin Value"));

	GraphPinObj->Modify();

	switch (PinMode)
	{
		case ECameraRigNameGraphPinMode::NamePin:
			GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, SelectedCameraRig->GetDisplayName());
			break;
		case ECameraRigNameGraphPinMode::ReferencePin:
			GraphPinObj->GetSchema()->TrySetDefaultObject(*GraphPinObj, SelectedCameraRig);
			break;
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

