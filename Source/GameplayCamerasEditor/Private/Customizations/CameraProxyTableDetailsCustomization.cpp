// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraProxyTableDetailsCustomization.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigProxyTable.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Editors/CameraRigPickerConfig.h"
#include "IDetailChildrenBuilder.h"
#include "IGameplayCamerasEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SNullWidget.h"

#define LOCTEXT_NAMESPACE "CameraProxyTableDetailsCustomization"

namespace UE::Cameras
{

TSharedRef<IPropertyTypeCustomization> FCameraProxyTableEntryDetailsCustomization::MakeInstance()
{
	return MakeShared<FCameraProxyTableEntryDetailsCustomization>();
}

void FCameraProxyTableEntryDetailsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		];
}

void FCameraProxyTableEntryDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TArray<UObject*> OuterObjects;
	StructPropertyHandle->GetOuterObjects(OuterObjects);

	ProxyTables.Reset();
	for (UObject* OuterObject : OuterObjects)
	{
		ProxyTables.Add(CastChecked<UCameraRigProxyTable>(OuterObject));
	}

	CameraRigPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCameraRigProxyTableEntry, CameraRig));
	CameraRigProxyPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCameraRigProxyTableEntry, CameraRigProxy));

	StructBuilder.AddProperty(CameraRigProxyPropertyHandle.ToSharedRef());

	StructBuilder.AddProperty(CameraRigPropertyHandle.ToSharedRef())
		.IsEnabled(ProxyTables.Num() == 1)
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
					.Text(this, &FCameraProxyTableEntryDetailsCustomization::OnGetComboButtonText)
				]
				.OnGetMenuContent(this, &FCameraProxyTableEntryDetailsCustomization::OnBuildCameraRigPicker)
			];
}

FText FCameraProxyTableEntryDetailsCustomization::OnGetComboButtonText() const
{
	TArray<void*> RawData;
	CameraRigPropertyHandle->AccessRawData(RawData);

	if (RawData.Num() == 0)
	{
		return LOCTEXT("NoCameraRigs", "None");
	}
	else if (RawData.Num() > 1)
	{
		return LOCTEXT("MultipleCameraRigs", "Multiple Values");
	}
	else
	{
		FText DisplayText(LOCTEXT("NullCameraRig", "None"));
		TObjectPtr<UCameraRigAsset>* CameraRigPtr = (TObjectPtr<UCameraRigAsset>*)RawData[0];
		if (*CameraRigPtr)
		{
			DisplayText = FText::FromString((*CameraRigPtr)->GetDisplayName());
		}
		return DisplayText;
	}
}

TSharedRef<SWidget> FCameraProxyTableEntryDetailsCustomization::OnBuildCameraRigPicker()
{
	TArray<void*> RawData;
	CameraRigPropertyHandle->AccessRawData(RawData);

	if (RawData.Num() != 1)
	{
		return SNullWidget::NullWidget;
	}

	IGameplayCamerasEditorModule& CamerasEditorModule = FModuleManager::LoadModuleChecked<IGameplayCamerasEditorModule>("GameplayCamerasEditor");

	TObjectPtr<UCameraRigAsset>* CameraRigPtr = (TObjectPtr<UCameraRigAsset>*)RawData[0];
	UCameraAsset* OuterCameraAsset = ProxyTables[0]->GetTypedOuter<UCameraAsset>();

	FCameraRigPickerConfig CameraRigPickerConfig;
	CameraRigPickerConfig.bCanSelectCameraAsset = false;
	CameraRigPickerConfig.InitialCameraAssetSelection = FAssetData(OuterCameraAsset);
	CameraRigPickerConfig.OnCameraRigSelected = FOnCameraRigSelected::CreateSP(
			this, &FCameraProxyTableEntryDetailsCustomization::OnCameraRigSelected);
	CameraRigPickerConfig.PropertyToSet = CameraRigPropertyHandle;
	CameraRigPickerConfig.InitialCameraRigSelection = (*CameraRigPtr);

	return CamerasEditorModule.CreateCameraRigPicker(CameraRigPickerConfig);
}

void FCameraProxyTableEntryDetailsCustomization::OnCameraRigSelected(UCameraRigAsset* CameraRig)
{
	ComboButton->SetIsOpen(false);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

