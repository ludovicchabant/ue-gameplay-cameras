// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigNameGraphPin.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editors/CameraRigPickerConfig.h"
#include "Editors/SCameraRigPickerButton.h"
#include "IContentBrowserSingleton.h"
#include "IGameplayCamerasEditorModule.h"
#include "ScopedTransaction.h"
#include "SGraphPin.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"

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
			SAssignNew(CameraRigPickerButton, SCameraRigPickerButton)
			.CameraRigPickerConfig(this, &SCameraRigNameGraphPin::OnCreateCameraRigPickerConfig)
			.SelectedCameraRigName(this, &SCameraRigNameGraphPin::OnGetSelectedCameraRigName)
			.ButtonToolTipText(this, &SCameraRigNameGraphPin::OnGetCameraRigPickerToolTipText)
			.ButtonForegroundColor(this, &SCameraRigNameGraphPin::OnGetComboForeground)
			.ButtonColorAndOpacity(this, &SCameraRigNameGraphPin::OnGetWidgetBackground)
			.PickerMenuPlacement(MenuPlacement_BelowAnchor)
			.IsEnabled(this, &SGraphPin::IsEditingEnabled)
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

FText SCameraRigNameGraphPin::OnGetSelectedCameraRigName() const
{
	if (GraphPinObj != nullptr)
	{
		if (const UCameraRigAsset* CameraRig = Cast<const UCameraRigAsset>(GraphPinObj->DefaultObject))
		{
			return FText::FromString(CameraRig->GetDisplayName());
		}
		return LOCTEXT("NoCameraRigName", "Select camera rig");
	}
	return LOCTEXT("InvalidGraphPin", "Invalid graph pin");
}

FText SCameraRigNameGraphPin::OnGetCameraRigPickerToolTipText() const
{
	return LOCTEXT("ComboToolTipText", "The name of the camera rig.");
}

FCameraRigPickerConfig SCameraRigNameGraphPin::OnCreateCameraRigPickerConfig() const
{
	FCameraRigPickerConfig CameraRigPickerConfig;
	CameraRigPickerConfig.bFocusCameraRigSearchBoxWhenOpened = true;
	CameraRigPickerConfig.OnCameraRigSelected = FOnCameraRigSelected::CreateSP(this, &SCameraRigNameGraphPin::OnPickerAssetSelected);

	// Find the already specified camera rig, if any.
	if (GraphPinObj)
	{
		UCameraRigAsset* DefaultCameraRig = Cast<UCameraRigAsset>(GraphPinObj->DefaultObject);
		if (DefaultCameraRig)
		{
			CameraRigPickerConfig.InitialCameraAssetSelection = DefaultCameraRig->GetTypedOuter<UCameraAsset>();
			CameraRigPickerConfig.InitialCameraRigSelection = DefaultCameraRig;
		}
	}

	OnCustomizeCameraRigPickerConfig(CameraRigPickerConfig);

	return CameraRigPickerConfig;
}

void SCameraRigNameGraphPin::OnPickerAssetSelected(UCameraRigAsset* SelectedItem) const
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

void SCameraRigNameGraphPin::SetCameraRig(UCameraRigAsset* SelectedCameraRig) const
{
	const FScopedTransaction Transaction(LOCTEXT("ChangeObjectPinValue", "Change Object Pin Value"));

	GraphPinObj->Modify();
	GraphPinObj->GetSchema()->TrySetDefaultObject(*GraphPinObj, SelectedCameraRig);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

