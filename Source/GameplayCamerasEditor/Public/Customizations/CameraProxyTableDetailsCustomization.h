// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class SComboButton;
class SWidget;
class UCameraRigAsset;
class UCameraRigProxyTable;
struct FCameraRigProxyTableEntry;

namespace UE::Cameras
{

/**
 * Details customization for an entry in a camera rig proxy table.
 *
 * It uses a custom camera rig picker that only chooses among the rigs of the parent camera asset.
 */
class FCameraProxyTableEntryDetailsCustomization : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:

	// IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

	TSharedRef<SWidget> OnBuildCameraRigPicker();
	FText OnGetComboButtonText() const;
	void OnCameraRigSelected(UCameraRigAsset* CameraRig);

private:

	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<IPropertyHandle> CameraRigPropertyHandle;
	TSharedPtr<IPropertyHandle> CameraRigProxyPropertyHandle;
	TArray<TWeakObjectPtr<UCameraRigProxyTable>> ProxyTables;
};

}  // namespace UE::Cameras

