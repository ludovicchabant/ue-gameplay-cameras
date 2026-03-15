// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraParameterDetailsCustomizations.h"

#include "ContentBrowserModule.h"
#include "Customizations/MathStructCustomizations.h"
#include "DetailLayoutBuilder.h"
#include "Editors/CameraVariablePickerConfig.h"
#include "IContentBrowserSingleton.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailPropertyRow.h"
#include "IGameplayCamerasEditorModule.h"
#include "IPropertyUtilities.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditorModule.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "UObject/Object.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "CameraParameterDetailsCustomization"

namespace UE::Cameras
{

static TArrayView<FName> GetCameraParameterMetaDataToCopy()
{
	static TArray<FName> MetaDataKeys{ 
		TEXT("UIMin"), TEXT("UIMax"),
		TEXT("ClampMin"), TEXT("ClampMax"),
		TEXT("Units"), TEXT("ForceUnits")
	};
	return MetaDataKeys;
}

void FCameraParameterDetailsCustomization::Register(FPropertyEditorModule& PropertyEditorModule)
{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(\
			F##ValueName##CameraParameter::StaticStruct()->GetFName(),\
			FOnGetPropertyTypeCustomizationInstance::CreateLambda(\
				[]{ return MakeShared<F##ValueName##CameraParameterDetailsCustomization>(); }));
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
}

void FCameraParameterDetailsCustomization::Unregister(FPropertyEditorModule& PropertyEditorModule)
{
	if (UObjectInitialized())
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(\
				F##ValueName##CameraParameter::StaticStruct()->GetFName());
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}
}

void FCameraParameterDetailsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Gather up the things we need.
	PropertyUtilities = CustomizationUtils.GetPropertyUtilities();
	StructProperty = PropertyHandle;

	// All camera parameters should have a "Value" property.
	ValueProperty = PropertyHandle->GetChildHandle("Value");
	ensure(ValueProperty);

	// Copy select details-view-related metadata from the camera parameter struct onto the property handle.
	for (const FName Key : GetCameraParameterMetaDataToCopy())
	{
		const FString& MetaDataValue = StructProperty->GetMetaData(Key);
		if (!MetaDataValue.IsEmpty())
		{
			ValueProperty->SetInstanceMetaData(Key, MetaDataValue);
		}
	}

	// Update our variable info once now. We will then update it every tick, since the UI needs it
	// for various things.
	UpdateCachedInfo();

	// Create the parameter value editor (float editor, vector editor, etc.)
	TSharedRef<SWidget> ValueWidget = ValueProperty->CreatePropertyValueWidgetWithCustomization(nullptr);
	ValueWidget->SetEnabled(TAttribute<bool>::CreateSP(this, &FCameraParameterDetailsCustomization::IsValueEditorEnabled));

	// Create the whole UI layout.
	TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasStyle = FGameplayCamerasEditorStyle::Get();

	HeaderRow
	.NameContent()
	[
		StructProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(100.f)
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(0)
		.FillWidth(1.f)
		[
			ValueWidget
		]
		+SHorizontalBox::Slot()
		.Padding(0)
		.AutoWidth()
		.HAlign(HAlign_Right)
		[
			ValueProperty->CreateDefaultPropertyButtonWidgets()
		]
	];

	HeaderRow.OverrideResetToDefault(
			FResetToDefaultOverride::Create(
				FIsResetToDefaultVisible::CreateSP(this, &FCameraParameterDetailsCustomization::IsResetToDefaultVisible),
				FResetToDefaultHandler::CreateSP(this, &FCameraParameterDetailsCustomization::OnResetToDefault)));
}

void FCameraParameterDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	uint32 NumChildren = 0;
	FPropertyAccess::Result Result = ValueProperty->GetNumChildren(NumChildren);
	if (Result == FPropertyAccess::Success)
	{
		for (uint32 Index = 0; Index < NumChildren; ++Index)
		{
			TSharedPtr<IPropertyHandle> ChildProperty = ValueProperty->GetChildHandle(Index);
			if (ChildProperty)
			{
				ChildBuilder.AddProperty(ChildProperty.ToSharedRef());
			}
		}
	}
}

void FCameraParameterDetailsCustomization::Tick(float DeltaTime)
{
	// Use the editor tick to query the property values only once per frame.
	UpdateCachedInfo();
}

void FCameraParameterDetailsCustomization::UpdateCachedInfo()
{
	CachedInfo = FCachedInfo();

	if (StructProperty->IsValidHandle())
	{
		StructProperty->EnumerateRawData(
				[this](void* RawData, const int32 ValueIndex, const int32 NumValues)
				{
					if (RawData)
					{
						CachedInfo.bHasOverride |= HasOverride(RawData);
					}
					return true;
				});
	}
}

bool FCameraParameterDetailsCustomization::IsValueEditorEnabled() const
{
	// The value widget is enabled (i.e. the user can change the value) if the parameter isn't driven by
	// a variable or a rig parameter.
	return !CachedInfo.bHasOverride;
}

bool FCameraParameterDetailsCustomization::IsResetToDefaultVisible(TSharedPtr<IPropertyHandle> InPropertyHandle) const
{
	return ValueProperty->CanResetToDefault();
}

void FCameraParameterDetailsCustomization::OnResetToDefault(TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	ValueProperty->ResetToDefault();
	PropertyUtilities->RequestForceRefresh();
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
bool F##ValueName##CameraParameterDetailsCustomization::HasOverride(void* InRawData)\
{\
	F##ValueName##CameraParameter* TypedData = reinterpret_cast<F##ValueName##CameraParameter*>(InRawData);\
	return TypedData->HasOverride();\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

