// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"
#include "CoreTypes.h"
#include "IPropertyTypeCustomization.h"

class FPropertyEditorModule;
class IDetailLayoutBuilder;
class IPropertyUtilities;
class SComboButton;
class SWidget;

namespace UE::Cameras
{

/**
 * Base details customization for camera parameters.
 */
class FCameraParameterDetailsCustomization : public IPropertyTypeCustomization
{
public:

	/** Registers details customizations for all camera parameter types. */
	static void Register(FPropertyEditorModule& PropertyEditorModule);
	/** Unregisters details customizations for all camera parameter types. */
	static void Unregister(FPropertyEditorModule& PropertyEditorModule);

public:

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

protected:

	virtual void GetValueAndVariablePropertyNames(FName& OutValueProperty, FName& OutVariableProperty) = 0;
	virtual void SetParameterVariable(void* InRawData, UCameraVariableAsset* InVariable) = 0;

private:

	TSharedRef<SWidget> BuildCameraVariableBrowser();

	void UpdateVariableInfo();
	bool HasVariableInfoText() const;

	bool CanClearVariable() const;
	void OnClearVariable();

	void OnSetVariable(UCameraVariableAsset* InVariable);
	void OnResetToDefault();

protected:

	UClass* VariableClass = nullptr;

	UCameraVariableAsset* CommonVariable = nullptr;
	FText VariableInfoText;
	FText VariableErrorText;
	bool bIsExposedParameterVariable = false;

	TSharedPtr<IPropertyUtilities> PropertyUtilities;

	TSharedPtr<IPropertyHandle> StructProperty;
	TSharedPtr<IPropertyHandle> ValueProperty;
	TSharedPtr<IPropertyHandle> VariableProperty;

	TSharedPtr<SComboButton> VariableBrowserButton;
};

// Create all the individual classes.
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
class F##ValueName##CameraParameterDetailsCustomization : public FCameraParameterDetailsCustomization\
{\
protected:\
	virtual void GetValueAndVariablePropertyNames(FName& OutValueProperty, FName& OutVariableProperty) override\
	{\
		OutValueProperty = GET_MEMBER_NAME_CHECKED(F##ValueName##CameraParameter, Value);\
		OutVariableProperty = GET_MEMBER_NAME_CHECKED(F##ValueName##CameraParameter, Variable);\
	}\
	virtual void SetParameterVariable(void* InRawData, UCameraVariableAsset* InVariable) override;\
};
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

}  // namespace UE::Cameras
 
