// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraParameterDetailsCustomizations.h"

#include "ContentBrowserModule.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableCollection.h"
#include "Customizations/MathStructCustomizations.h"
#include "DetailLayoutBuilder.h"
#include "Editors/CameraVariablePickerConfig.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IContentBrowserSingleton.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailPropertyRow.h"
#include "IGameplayCamerasEditorModule.h"
#include "IPropertyUtilities.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Styling/StyleColors.h"
#include "UObject/Object.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "CameraParameterDetailsCustomization"

namespace UE::Cameras
{

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

	// Figure out the parameter value and driving variable property names for the actual camera
	// parameter type we are displaying. In theory these are always called "Value" and "Variable", but
	// we do this in a statically-typed way via polyphormism, instead of in a duck-typing way.
	FName ValuePropertyName, VariablePropertyName;
	GetValueAndVariablePropertyNames(ValuePropertyName, VariablePropertyName);

	ValueProperty = PropertyHandle->GetChildHandle(ValuePropertyName);
	VariableProperty = PropertyHandle->GetChildHandle(VariablePropertyName);

	// Get the type of camera variable we need for this camera parameter (bool variable, float variable, etc.)
	VariableClass = nullptr;
	if (FObjectProperty* VariableObjectProperty = CastField<FObjectProperty>(VariableProperty->GetProperty()))
	{
		VariableClass = VariableObjectProperty->PropertyClass;
	}

	// Update our variable info once now. We will then update it every tick, since the UI needs it
	// for various things.
	UpdateVariableInfo();

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
		SAssignNew(LayoutBox, SHorizontalBox)
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
		+SHorizontalBox::Slot()
		.Padding(2.f)
		.AutoWidth()
		[
			SAssignNew(VariableBrowserButton, SComboButton)
			.HasDownArrow(true)
			.ContentPadding(1.f)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.IsEnabled(this, &FCameraParameterDetailsCustomization::IsCameraVariableBrowserEnabled)
			.ToolTipText(LOCTEXT("SetVariable_ToolTip", "Selects a camera variable to drive this parameter"))
			.ButtonContent()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(GameplayCamerasStyle->GetBrush("CameraParameter.VariableBrowser"))
				]
				+SHorizontalBox::Slot()
				.FillWidth(0.3f)
				.Padding(2.f)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Visibility(this, &FCameraParameterDetailsCustomization::GetVariableInfoTextVisibility)
					.MaxDesiredWidth(this, &FCameraParameterDetailsCustomization::GetVariableInfoTextMaxWidth)
					[
						SNew(STextBlock)
						.Text(this, &FCameraParameterDetailsCustomization::GetVariableInfoText)
						.MinDesiredWidth(20.f)
						.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
					]
				]
				+SHorizontalBox::Slot()
				.FillWidth(0.3f)
				.Padding(2.f)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Visibility(this, &FCameraParameterDetailsCustomization::GetVariableErrorTextVisibility)
					.MaxDesiredWidth(this, &FCameraParameterDetailsCustomization::GetVariableErrorTextMaxWidth)
					[
						SNew(STextBlock)
						.Text(this, &FCameraParameterDetailsCustomization::GetVariableErrorText)
						.MinDesiredWidth(20.f)
						.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
						.ColorAndOpacity(FStyleColors::Error)
					]
				]
			]
			.OnGetMenuContent(this, &FCameraParameterDetailsCustomization::BuildCameraVariableBrowser)
		]
	];

	// Setup some custom reset-to-default behavior, if we are allowed to.
	// (see the code in FCameraRigAssetReferenceDetailsCustomization that sets this metadata)
	const bool bNoResetToDefault = StructProperty->GetBoolMetaData("NoCustomCameraParameterResetToDefault");
	if (!bNoResetToDefault)
	{
		HeaderRow.OverrideResetToDefault(
				FResetToDefaultOverride::Create(
					FIsResetToDefaultVisible::CreateSP(this, &FCameraParameterDetailsCustomization::IsResetToDefaultVisible),
					FResetToDefaultHandler::CreateSP(this, &FCameraParameterDetailsCustomization::OnResetToDefault)));
	}
}

void FCameraParameterDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FCameraParameterDetailsCustomization::Tick(float DeltaTime)
{
	// Use the editor tick to query the property values only once per frame.
	UpdateVariableInfo();
}

void FCameraParameterDetailsCustomization::UpdateVariableInfo()
{
	VariableInfo = FCameraVariableInfo();

	UObject* VariableObject = nullptr;
	FPropertyAccess::Result PropertyAccessResult = VariableProperty->GetValue(VariableObject);
	if (PropertyAccessResult == FPropertyAccess::Success)
	{
		if (VariableObject)
		{
			if (UCameraVariableAsset* Variable = Cast<UCameraVariableAsset>(VariableObject))
			{
				VariableInfo.VariableValue = ECameraVariableValue::Set;
				VariableInfo.CommonVariable = Variable;
				VariableInfo.InfoText = Variable->DisplayName.IsEmpty() ?
					FText::FromName(Variable->GetFName()) :
					FText::FromString(Variable->DisplayName);
				VariableInfo.bIsExposedParameterVariable = VariableInfo.CommonVariable->bIsPrivate;
			}
			else
			{
				VariableInfo.VariableValue = ECameraVariableValue::Invalid;
				VariableInfo.ErrorText = LOCTEXT("InvalidVariableObject", "Invalid Variable");
			}
		}
		// else: variable is not set
	}
	else if (PropertyAccessResult == FPropertyAccess::MultipleValues)
	{
		VariableInfo.VariableValue = ECameraVariableValue::MultipleSet;
		VariableInfo.InfoText = LOCTEXT("MultipleVariableValues", "Multiple Variables");
	}
	else
	{
		VariableInfo.VariableValue = ECameraVariableValue::Invalid;
		VariableInfo.ErrorText = LOCTEXT("ErrorReadingVariable", "Error Reading Variable");
	}
}

TSharedRef<SWidget> FCameraParameterDetailsCustomization::BuildCameraVariableBrowser()
{
	const bool bShouldCloseWindowAfterMenuSelection = true;
	const bool bCloseSelfOnly = true;
	const bool bSearchable = false;
	FMenuBuilder MenuBuilder(true, nullptr, nullptr, bCloseSelfOnly, &FCoreStyle::Get(), bSearchable);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CameraVariableOperations", "Current Parameter"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ClearVariable", "Clear"),
			LOCTEXT("ClearVariable_ToolTip", "Clears the variable from the camera parameter"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Delete"),
			FUIAction(
				FExecuteAction::CreateSP(this, &FCameraParameterDetailsCustomization::OnClearVariable),
				FCanExecuteAction::CreateSP(this, &FCameraParameterDetailsCustomization::CanClearVariable))
			);
	}
	MenuBuilder.EndSection();

	FCameraVariablePickerConfig PickerConfig;
	PickerConfig.CameraVariableClass = VariableClass;
	PickerConfig.InitialCameraVariableSelection = VariableInfo.CommonVariable;
	PickerConfig.CameraVariableCollectionSaveSettingsName = TEXT("CameraParameterVariablePropertyPicker");
	PickerConfig.OnCameraVariableSelected = FOnCameraVariableSelected::CreateSP(
			this, &FCameraParameterDetailsCustomization::OnSetVariable);
	IGameplayCamerasEditorModule& GameplayCamerasEditorModule = IGameplayCamerasEditorModule::Get();
	TSharedRef<SWidget> PickerWidget = GameplayCamerasEditorModule.CreateCameraVariablePicker(PickerConfig);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CameraVariableBrowser", "Browse"));
	{
		TSharedRef<SWidget> VariableBrowser = SNew(SBox)
			.MinDesiredWidth(300.f)
			.MinDesiredHeight(300.f)
			[
				PickerWidget
			];
		MenuBuilder.AddWidget(VariableBrowser, FText(), true, false);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

bool FCameraParameterDetailsCustomization::IsValueEditorEnabled() const
{
	// The value widget is enabled (i.e. the user can change the value) if the parameter isn't driven by
	// a variable, or if that variable is a private variable meant to expose the parameter on the rig interface.
	return (VariableInfo.VariableValue == ECameraVariableValue::NotSet || VariableInfo.bIsExposedParameterVariable);
}

bool FCameraParameterDetailsCustomization::IsCameraVariableBrowserEnabled() const
{
	// The variable picker is enabled if the parameter isn't exposed to the rig interface via a private variable,
	// since we can't drive a value with both an interface parameter and a user-defined variable.
	return !VariableInfo.bIsExposedParameterVariable;
}

FText FCameraParameterDetailsCustomization::GetVariableInfoText() const
{
	return VariableInfo.InfoText;
}

EVisibility FCameraParameterDetailsCustomization::GetVariableInfoTextVisibility() const
{
	const bool bShowVariableInfoText = !VariableInfo.InfoText.IsEmpty() && !VariableInfo.bIsExposedParameterVariable;
	return bShowVariableInfoText ? EVisibility::Visible : EVisibility::Collapsed;
}

FOptionalSize FCameraParameterDetailsCustomization::GetVariableInfoTextMaxWidth() const
{
	// We want this text to take at most 30% of the free-standing space of the combo button.
	// Free-standing space excludes fixed things like the combo button icon, the dropdown icon, paddings, etc.
	// IMPORTANT: update this if the main layout changes inside Construct()
	const float FixedSpace = 1.f + (2.f+ 16.f + 2.f) + (2.f + 16.f + 2.f) + 1.f;

	const bool bShowVariableInfoText = !VariableInfo.InfoText.IsEmpty() && !VariableInfo.bIsExposedParameterVariable;
	const float LayoutBoxWidth = LayoutBox ? LayoutBox->GetPaintSpaceGeometry().GetLocalSize().X : 0.f;
	return bShowVariableInfoText ? FOptionalSize((LayoutBoxWidth - FixedSpace) / 3.f) : FOptionalSize(0);
}

FText FCameraParameterDetailsCustomization::GetVariableErrorText() const
{
	return VariableInfo.ErrorText;
}

EVisibility FCameraParameterDetailsCustomization::GetVariableErrorTextVisibility() const
{
	const bool bShowVariableErrorText = !VariableInfo.ErrorText.IsEmpty() && !VariableInfo.bIsExposedParameterVariable;
	return bShowVariableErrorText ? EVisibility::Visible : EVisibility::Collapsed;
}

FOptionalSize FCameraParameterDetailsCustomization::GetVariableErrorTextMaxWidth() const
{
	// See comments in GetVariableInfoTextMaxWidth.
	const float FixedSpace = 1.f + (2.f+ 16.f + 2.f) + (2.f + 16.f + 2.f) + 1.f;

	const bool bShowVariableErrorText = !VariableInfo.ErrorText.IsEmpty() && !VariableInfo.bIsExposedParameterVariable;
	const float LayoutBoxWidth = LayoutBox ? LayoutBox->GetPaintSpaceGeometry().GetLocalSize().X : 0.f;
	return bShowVariableErrorText ? FOptionalSize((LayoutBoxWidth - FixedSpace) / 3.f) : FOptionalSize(0);
}

bool FCameraParameterDetailsCustomization::CanClearVariable() const
{
	return VariableProperty->CanResetToDefault();
}

void FCameraParameterDetailsCustomization::OnClearVariable()
{
	VariableProperty->ResetToDefault();
	PropertyUtilities->RequestForceRefresh();
}

void FCameraParameterDetailsCustomization::OnSetVariable(UCameraVariableAsset* InVariable)
{
	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);

	TArray<UObject*> OuterObjects;
	StructProperty->GetOuterObjects(OuterObjects);

	check(!OuterObjects.Num() || OuterObjects.Num() == RawData.Num());
	
	{
		FScopedTransaction Transaction(FText::Format(
					LOCTEXT("SetPropertyValue", "Set {0}"), StructProperty->GetPropertyDisplayName()));

		StructProperty->NotifyPreChange();

		for (int32 ValueIndex = 0; ValueIndex < RawData.Num(); ++ValueIndex)
		{
			SetParameterVariable(RawData[ValueIndex], InVariable);
		}

		StructProperty->NotifyPostChange(EPropertyChangeType::ValueSet);
	}

	FPropertyChangedEvent ChangeEvent(StructProperty->GetProperty(), EPropertyChangeType::ValueSet, OuterObjects);
	PropertyUtilities->NotifyFinishedChangingProperties(ChangeEvent);

	PropertyUtilities->RequestForceRefresh();
	VariableBrowserButton->SetIsOpen(false);
}

bool FCameraParameterDetailsCustomization::IsResetToDefaultVisible(TSharedPtr<IPropertyHandle> InPropertyHandle) const
{
	// The user can reset the camera parameter to its default if the value is non-default, and/or the
	// variable is a user-defined variable. In other words, if the variable is private because the parameter
	// is exposed, then we don't want to reset that -- we only want to reset the value.
	return 
		ValueProperty->CanResetToDefault() || 
		(VariableProperty->CanResetToDefault() && !VariableInfo.bIsExposedParameterVariable);
}

void FCameraParameterDetailsCustomization::OnResetToDefault(TSharedPtr<IPropertyHandle> InPropertyHandle)
{
	// As mentioned above, if the camera parameter is exposed publicly on the camera rig, we only want
	// to reset the value to the default (and keep the private variable set on it).
	// Otherwise, we can reset both the value and the variable.
	if (VariableInfo.bIsExposedParameterVariable)
	{
		ValueProperty->ResetToDefault();
	}
	else
	{
		StructProperty->ResetToDefault();
	}

	PropertyUtilities->RequestForceRefresh();
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
void F##ValueName##CameraParameterDetailsCustomization::SetParameterVariable(void* InRawData, UCameraVariableAsset* InVariable)\
{\
	F##ValueName##CameraParameter* TypedData = reinterpret_cast<F##ValueName##CameraParameter*>(InRawData);\
	TypedData->Variable = CastChecked<U##ValueName##CameraVariable>(InVariable, ECastCheckedType::NullAllowed);\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE
