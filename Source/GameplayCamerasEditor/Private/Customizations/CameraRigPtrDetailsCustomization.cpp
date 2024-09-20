// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraRigPtrDetailsCustomization.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Helpers/CameraAssetReferenceGatherer.h"
#include "Editors/CameraRigPickerConfig.h"
#include "IDetailChildrenBuilder.h"
#include "IGameplayCamerasEditorModule.h"
#include "PropertyEditorModule.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "CameraRigPtrDetailsCustomization"

namespace UE::Cameras
{

TSharedRef<IPropertyTypeCustomization> FCameraRigPtrDetailsCustomization::MakeInstance()
{
	return MakeShared<FCameraRigPtrDetailsCustomization>();
}

void FCameraRigPtrDetailsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	CameraRigPropertyHandle = StructPropertyHandle;

	EPickerMode PickerMode = DeterminePickerMode();

	TSharedPtr<SWidget> ValueContentWidget;
	if (PickerMode != EPickerMode::PrefabCameraRigPicker)
	{
		FOnGetContent OnGetComboMenuContent;
		if (PickerMode == EPickerMode::AnyCameraRigPicker)
		{
			OnGetComboMenuContent = FOnGetContent::CreateSP(this, &FCameraRigPtrDetailsCustomization::OnBuildAnyCameraRigNamePicker);
		}
		else if (PickerMode == EPickerMode::SelfCameraRigPicker)
		{
			OnGetComboMenuContent = FOnGetContent::CreateSP(this, &FCameraRigPtrDetailsCustomization::OnBuildSelfCameraRigNamePicker);
		}
		else if (PickerMode == EPickerMode::CameraDirectorRigPicker)
		{
			OnGetComboMenuContent = FOnGetContent::CreateSP(this, &FCameraRigPtrDetailsCustomization::OnBuildCameraDirectorRigNamePicker);
		}

		CameraRigPickerButton = SNew(SComboButton)
		.HasDownArrow(true)
		.ContentPadding(FMargin(2.f, 2.f, 2.f, 1.f))
		.ToolTipText(LOCTEXT("SetRig_ToolTip", "Selects the camera rig"))
		.OnGetMenuContent(OnGetComboMenuContent)
		.ButtonContent()
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), "PropertyEditor.AssetClass")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			.Text(this, &FCameraRigPtrDetailsCustomization::OnGetComboText)
			.ToolTipText(this, &FCameraRigPtrDetailsCustomization::OnGetComboToolTipText)
		];

		ValueContentWidget = CameraRigPickerButton;
	}
	else
	{
		ValueContentWidget = StructPropertyHandle->CreatePropertyValueWidget();
	}

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			ValueContentWidget.ToSharedRef()
		];
}

void FCameraRigPtrDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

FCameraRigPtrDetailsCustomization::EPickerMode FCameraRigPtrDetailsCustomization::DeterminePickerMode()
{
	FProperty* CameraRigProperty = CameraRigPropertyHandle->GetProperty();
	FField* MetaDataField = CameraRigProperty;
	if (CameraRigProperty->GetOwnerStruct() == FCameraRigAssetReference::StaticStruct())
	{
		TSharedPtr<IPropertyHandle> CameraRigReferencePropertyHandle = CameraRigPropertyHandle->GetParentHandle();
		ensure(CameraRigReferencePropertyHandle);
		MetaDataField = CameraRigReferencePropertyHandle->GetProperty();
	}

	if (MetaDataField->GetBoolMetaData("UseCameraDirectorRigPicker"))
	{
		return EPickerMode::CameraDirectorRigPicker;
	}
	if (MetaDataField->GetBoolMetaData("UseSelfCameraRigPicker"))
	{
		return EPickerMode::SelfCameraRigPicker;
	}
	if (MetaDataField->GetBoolMetaData("UseCameraRigPicker"))
	{
		return EPickerMode::AnyCameraRigPicker;
	}
	return EPickerMode::PrefabCameraRigPicker;
}

FText FCameraRigPtrDetailsCustomization::OnGetComboText() const
{
	UObject* Value;
	FPropertyAccess::Result PropertyAccess = CameraRigPropertyHandle->GetValue(Value);
	if (PropertyAccess == FPropertyAccess::Success)
	{
		if (const UCameraRigAsset* CameraRig = Cast<const UCameraRigAsset>(Value))
		{
			return FText::FromString(CameraRig->GetDisplayName());
		}
		return LOCTEXT("NoCameraRigValue", "Select camera rig");
	}
	else if (PropertyAccess == FPropertyAccess::MultipleValues)
	{
		return LOCTEXT("MultipleValues", "Multiple values");
	}
	return LOCTEXT("ErrorValue", "Error reading camera rig value");
}

FText FCameraRigPtrDetailsCustomization::OnGetComboToolTipText() const
{
	return LOCTEXT("ComboToolTipText", "The name of the camera rig to activate.");
}

TSharedRef<SWidget> FCameraRigPtrDetailsCustomization::OnBuildCameraDirectorRigNamePicker()
{
	FCameraRigPickerConfig PickerConfig;
	PickerConfig.bCanSelectCameraAsset = false;

	// Figure out what camera asset is referencing our outermost objects (main package objects).
	TArray<UObject*> OuterObjects;
	CameraRigPropertyHandle->GetOuterObjects(OuterObjects);

	TSet<UObject*> CameraDirectorObjects;
	for (UObject* OuterObject : OuterObjects)
	{
		if (UObject* OutermostObject = OuterObject->GetOutermostObject())
		{
			CameraDirectorObjects.Add(OutermostObject);
		}
	}

	// Automatically show the rigs of the referencing camera asset. Show a warning if none or multiple
	// camera assets are referencing us, similar to the warning for the Blueprint camera director
	// rig activation picker.
	TArray<UCameraAsset*> ReferencingCameraAssets;
	for (UObject* CameraDirectorObject : CameraDirectorObjects)
	{
		FCameraAssetReferenceGatherer::GetReferencingCameraAssets(CameraDirectorObject, ReferencingCameraAssets);
	}

	if (ReferencingCameraAssets.Num() == 0)
	{
		PickerConfig.WarningMessage = LOCTEXT("NoReferencingCameraAssetWarning",
				"No camera asset references this camera director, so no camera rig list can be displayed. "
				"Make a camera asset use this asset as its camera director evaluator, or use camera rig"
				"proxy assets instead.");
	}
	else
	{
		PickerConfig.InitialCameraAssetSelection = ReferencingCameraAssets[0];

		if (ReferencingCameraAssets.Num() > 1)
		{
			PickerConfig.WarningMessage = LOCTEXT("ManyReferencingCameraAssetsWarning",
				"More than one camera asset references this camera director. Only camera rigs from the first "
				"one will be displayed. Even then, shared camera director assets should use camera rig"
				"proxy assets instead.");
		}
	}

	return BuildCameraRigNamePickerImpl(PickerConfig);
}

TSharedRef<SWidget> FCameraRigPtrDetailsCustomization::OnBuildSelfCameraRigNamePicker()
{
	FCameraRigPickerConfig PickerConfig;
	PickerConfig.bCanSelectCameraAsset = false;

	TArray<UObject*> OuterObjects;
	CameraRigPropertyHandle->GetOuterObjects(OuterObjects);

	TSet<UCameraAsset*> OuterCameraAssets;
	for (UObject* OuterObject : OuterObjects)
	{
		UCameraAsset* OuterCameraAsset = OuterObject->GetTypedOuter<UCameraAsset>();
		if (OuterCameraAsset)
		{
			OuterCameraAssets.Add(OuterCameraAsset);
		}
	}

	for (UCameraAsset* OuterCameraAsset : OuterCameraAssets)
	{
		PickerConfig.InitialCameraAssetSelection = FAssetData(OuterCameraAsset);
		break;
	}

	return BuildCameraRigNamePickerImpl(PickerConfig);
}

TSharedRef<SWidget> FCameraRigPtrDetailsCustomization::OnBuildAnyCameraRigNamePicker()
{
	FCameraRigPickerConfig PickerConfig;
	PickerConfig.bCanSelectCameraAsset = true;

	return BuildCameraRigNamePickerImpl(PickerConfig);
}

TSharedRef<SWidget> FCameraRigPtrDetailsCustomization::BuildCameraRigNamePickerImpl(FCameraRigPickerConfig& PickerConfig)
{
	PickerConfig.bFocusCameraRigSearchBoxWhenOpened = true;
	PickerConfig.OnCameraRigSelected = FOnCameraRigSelected::CreateSP(this, &FCameraRigPtrDetailsCustomization::OnPickerAssetSelected);

	UObject* SelectedCameraRig;
	if (CameraRigPropertyHandle->GetValue(SelectedCameraRig) == FPropertyAccess::Success)
	{
		PickerConfig.InitialCameraRigSelection = Cast<UCameraRigAsset>(SelectedCameraRig);
	}

	IGameplayCamerasEditorModule& CamerasEditorModule = IGameplayCamerasEditorModule::Get();
	return CamerasEditorModule.CreateCameraRigPicker(PickerConfig);
}

void FCameraRigPtrDetailsCustomization::OnPickerAssetSelected(UCameraRigAsset* SelectedItem)
{
	if (SelectedItem)
	{
		CameraRigPickerButton->SetIsOpen(false);
		CameraRigPropertyHandle->SetValue(SelectedItem);
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

