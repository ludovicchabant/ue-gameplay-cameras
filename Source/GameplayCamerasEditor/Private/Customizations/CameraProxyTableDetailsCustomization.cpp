// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraProxyTableDetailsCustomization.h"

#include "ContentBrowserModule.h"
#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigProxyTable.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IContentBrowserSingleton.h"
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

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TObjectPtr<UCameraRigAsset>* CameraRigPtr = (TObjectPtr<UCameraRigAsset>*)RawData[0];
	UCameraAsset* OuterCameraAsset = ProxyTables[0]->GetTypedOuter<UCameraAsset>();

	FAssetPickerConfig CameraRigPickerConfig;

	FARFilter ARFilter;
	ARFilter.ClassPaths.Add(FTopLevelAssetPath(UCameraRigAsset::StaticClass()->GetPathName()));

	CameraRigPickerConfig.bShowBottomToolbar = true;
	CameraRigPickerConfig.bAllowNullSelection = true;
	CameraRigPickerConfig.bFocusSearchBoxWhenOpened = true;
	CameraRigPickerConfig.SelectionMode = ESelectionMode::Single;
	CameraRigPickerConfig.Filter = ARFilter;
	CameraRigPickerConfig.SaveSettingsName = TEXT("CameraProxyTableEntryRigPickerSettings");
	CameraRigPickerConfig.InitialAssetViewType = EAssetViewType::List;
	CameraRigPickerConfig.InitialAssetSelection = FAssetData(*CameraRigPtr);
	CameraRigPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(
			this, &FCameraProxyTableEntryDetailsCustomization::OnCameraRigSelected);
	CameraRigPickerConfig.PropertyHandle = CameraRigPropertyHandle;

	return ContentBrowserModule.Get().CreateAssetPicker(CameraRigPickerConfig);
}

void FCameraProxyTableEntryDetailsCustomization::OnCameraRigSelected(const FAssetData& InSelectedAsset)
{
	ComboButton->SetIsOpen(false);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

