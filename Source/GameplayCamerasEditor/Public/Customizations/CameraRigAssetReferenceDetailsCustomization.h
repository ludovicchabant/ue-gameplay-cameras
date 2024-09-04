// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class FStructOnScope;
class IStructureDataProvider;
class UCameraRigAsset;
class UCameraRigInterfaceParameter;
struct FCameraRigAssetReference;

namespace UE::Cameras
{

class FCameraBuildLog;
class FCameraRigParameterOverrideDetailRow;

class FCameraRigAssetReferenceDetailsCustomization : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:

	// IPropertyTypeCustomization interface.
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

	void OnCameraRigChanged();
	void OnCameraRigBuilt(UCameraRigAsset* CameraRig, FCameraBuildLog& BuildLog);

	void UpdateParameterOverrides(const UCameraRigAsset* CameraRigToUpdate, bool bRequestRefresh);

	void BuildParameterOverrideRows(IDetailChildrenBuilder& StructBuilder);

private:

	TSharedPtr<IPropertyHandle> CameraRigReferenceProperty;
	TSharedPtr<IPropertyUtilities> PropertyUtilities;

	TArray<TSharedPtr<FCameraRigParameterOverrideDetailRow>> ParameterOverrideRows;
};

}  // namespace UE::Cameras
