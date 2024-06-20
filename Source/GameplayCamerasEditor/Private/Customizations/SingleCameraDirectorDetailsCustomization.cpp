// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/SingleCameraDirectorDetailsCustomization.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Directors/SingleCameraDirector.h"
#include "Editors/CameraRigPickerConfig.h"
#include "IDetailChildrenBuilder.h"
#include "IGameplayCamerasEditorModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyCustomizationHelpers.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SNullWidget.h"

#define LOCTEXT_NAMESPACE "SingleCameraDirectorDetailsCustomization"

namespace UE::Cameras
{

TSharedRef<IDetailCustomization> FSingleCameraDirectorDetailsCustomization::MakeInstance()
{
	return MakeShared<FSingleCameraDirectorDetailsCustomization>();
}

void FSingleCameraDirectorDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<USingleCameraDirector>> WeakDirectors = DetailBuilder.GetSelectedObjectsOfType<USingleCameraDirector>();
	WeakSelectedDirector = WeakDirectors.Num() == 1 ? WeakDirectors[0] : nullptr;

	CameraRigPropertyHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(USingleCameraDirector, CameraRig));

	IDetailCategoryBuilder& CommonCategory = DetailBuilder.EditCategory(TEXT("Common"));
	CommonCategory.AddProperty(CameraRigPropertyHandle)
		.IsEnabled(WeakSelectedDirector.IsValid())
		.CustomWidget()
			.NameContent()
			[
				CameraRigPropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			[
				SAssignNew(ComboButton, SComboButton)
				.ToolTipText(CameraRigPropertyHandle->GetToolTipText())
				.ContentPadding(2.f)
				.ButtonContent()
				[
					SNew(STextBlock)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
					.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
					.Text(this, &FSingleCameraDirectorDetailsCustomization::OnGetComboButtonText)
				]
				.OnGetMenuContent(this, &FSingleCameraDirectorDetailsCustomization::OnBuildCameraRigPicker)
			];
}

FText FSingleCameraDirectorDetailsCustomization::OnGetComboButtonText() const
{
	if (USingleCameraDirector* SelectedDirector = WeakSelectedDirector.Get())
	{
		FText DisplayText(LOCTEXT("NullCameraRig", "None"));
		if (SelectedDirector->CameraRig)
		{
			DisplayText = FText::FromString(SelectedDirector->CameraRig->GetDisplayName());
		}
		return DisplayText;
	}
	return LOCTEXT("MultipleCameraRigs", "Multiple Values");
}

TSharedRef<SWidget> FSingleCameraDirectorDetailsCustomization::OnBuildCameraRigPicker()
{
	USingleCameraDirector* SelectedDirector = WeakSelectedDirector.Get();
	if (!SelectedDirector)
	{
		return SNullWidget::NullWidget;
	}

	IGameplayCamerasEditorModule& CamerasEditorModule = FModuleManager::LoadModuleChecked<IGameplayCamerasEditorModule>("GameplayCamerasEditor");

	UCameraAsset* OuterCameraAsset = SelectedDirector->GetTypedOuter<UCameraAsset>();

	FCameraRigPickerConfig CameraRigPickerConfig;
	CameraRigPickerConfig.bCanSelectCameraAsset = false;
	CameraRigPickerConfig.InitialCameraAssetSelection = FAssetData(OuterCameraAsset);
	CameraRigPickerConfig.OnCameraRigSelected = FOnCameraRigSelected::CreateSP(
			this, &FSingleCameraDirectorDetailsCustomization::OnCameraRigSelected);
	CameraRigPickerConfig.PropertyToSet = CameraRigPropertyHandle;
	CameraRigPickerConfig.InitialCameraRigSelection = SelectedDirector->CameraRig;

	return CamerasEditorModule.CreateCameraRigPicker(CameraRigPickerConfig);
}

void FSingleCameraDirectorDetailsCustomization::OnCameraRigSelected(UCameraRigAsset* CameraRig)
{
	ComboButton->SetIsOpen(false);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

