// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"  // IWYU pragma: keep
#include "IPropertyTypeCustomization.h"
#include "TickableEditorObject.h"

class FPropertyEditorModule;
class IDetailLayoutBuilder;
class IPropertyUtilities;
class SComboButton;
class SHorizontalBox;
class SWidget;

namespace UE::Cameras
{

/**
 * Base details customization for camera parameters.
 */
class FCameraParameterDetailsCustomization 
	: public IPropertyTypeCustomization
	, public FTickableEditorObject
{
public:

	/** Registers details customizations for all camera parameter types. */
	static void Register(FPropertyEditorModule& PropertyEditorModule);
	/** Unregisters details customizations for all camera parameter types. */
	static void Unregister(FPropertyEditorModule& PropertyEditorModule);

public:

	// IPropertyTypeCustomization interface.
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	// FTickableEditorObject interface.
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FCameraParameterDetailsCustomization, STATGROUP_Tickables); }

protected:

	virtual bool HasOverride(void* InRawData) = 0;

private:

	void UpdateCachedInfo();

	bool IsValueEditorEnabled() const;

	bool IsResetToDefaultVisible(TSharedPtr<IPropertyHandle> InPropertyHandle) const;
	void OnResetToDefault(TSharedPtr<IPropertyHandle> InPropertyHandle);

protected:

	TSharedPtr<IPropertyUtilities> PropertyUtilities;

	TSharedPtr<IPropertyHandle> StructProperty;
	TSharedPtr<IPropertyHandle> ValueProperty;

private:

	struct FCachedInfo
	{
		bool bHasOverride = false;
	};
	FCachedInfo CachedInfo;
};

// Create all the individual classes.
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
class F##ValueName##CameraParameterDetailsCustomization : public FCameraParameterDetailsCustomization\
{\
	virtual bool HasOverride(void* InRawData) override;\
};
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

}  // namespace UE::Cameras
 
