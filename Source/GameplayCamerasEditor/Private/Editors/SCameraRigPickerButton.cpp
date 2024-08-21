// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigPickerButton.h"

#include "Editors/CameraRigPickerConfig.h"
#include "IGameplayCamerasEditorModule.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "SCameraRigPickerButton"

namespace UE::Cameras
{

void SCameraRigPickerButton::Construct(const FArguments& InArgs)
{
	CameraRigPickerConfigAttribute = InArgs._CameraRigPickerConfig;
	OnGetSelectedCameraRigName = InArgs._SelectedCameraRigName;

	TAttribute<FText> ComboToolTipTextAttribute = InArgs._ButtonToolTipText;
	if (!ComboToolTipTextAttribute.IsSet())
	{
		ComboToolTipTextAttribute = GetDefaultComboToolTipText();
	}

	ChildSlot
	[
		SAssignNew(CameraRigPickerButton, SComboButton)
			.ButtonStyle(FAppStyle::Get(), "PropertyEditor.AssetComboStyle")
			.ContentPadding(FMargin(2.f, 2.f, 2.f, 1.f))
			.ForegroundColor(InArgs._ButtonForegroundColor)
			.ButtonColorAndOpacity(InArgs._ButtonColorAndOpacity)
			.MenuPlacement(InArgs._PickerMenuPlacement)
			.IsEnabled(InArgs._IsEnabled)
			.ButtonContent()
			[
				SNew(STextBlock)
					.ColorAndOpacity(InArgs._ButtonForegroundColor)
					.TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
					.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
					.Text(this, &SCameraRigPickerButton::OnGetComboText)
					.ToolTipText(ComboToolTipTextAttribute)
			]
			.OnGetMenuContent(this, &SCameraRigPickerButton::OnBuildCameraRigNamePicker)
	];
}

bool SCameraRigPickerButton::IsOpen() const
{
	return CameraRigPickerButton->IsOpen();
}

void SCameraRigPickerButton::SetIsOpen(bool bIsOpen)
{
	CameraRigPickerButton->SetIsOpen(bIsOpen);
}

FText SCameraRigPickerButton::GetDefaultComboText() const
{
	return LOCTEXT("DefaultComboText", "Select Camera Rig");
}

FText SCameraRigPickerButton::GetDefaultComboToolTipText() const
{
	return LOCTEXT("ComboToolTipText", "The selected camera rig, if any.");
}

FText SCameraRigPickerButton::OnGetComboText() const
{
	FText Value = OnGetSelectedCameraRigName.Get();
	if (Value.IsEmpty())
	{
		Value = GetDefaultComboText();
	}
	return Value;
}

TSharedRef<SWidget> SCameraRigPickerButton::OnBuildCameraRigNamePicker()
{
	FCameraRigPickerConfig CameraRigPickerConfig = CameraRigPickerConfigAttribute.Get();

	IGameplayCamerasEditorModule& CamerasEditorModule = IGameplayCamerasEditorModule::Get();
	return CamerasEditorModule.CreateCameraRigPicker(CameraRigPickerConfig);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

