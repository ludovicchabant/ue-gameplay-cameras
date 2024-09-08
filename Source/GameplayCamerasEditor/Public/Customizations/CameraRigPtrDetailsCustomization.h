// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class SComboButton;
class SWidget;
class UCameraRigAsset;
struct FAssetData;

namespace UE::Cameras
{

struct FCameraRigPickerConfig;

class FCameraRigPtrDetailsCustomization : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

public:

	// IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

	enum class EPickerMode { PrefabCameraRigPicker, AnyCameraRigPicker, SelfCameraRigPicker, CameraDirectorRigPicker };

	EPickerMode DeterminePickerMode();

	FText OnGetComboText() const;
	FText OnGetComboToolTipText() const;
	TSharedRef<SWidget> OnBuildAnyCameraRigNamePicker();
	TSharedRef<SWidget> OnBuildSelfCameraRigNamePicker();
	TSharedRef<SWidget> OnBuildCameraDirectorRigNamePicker();
	TSharedRef<SWidget> BuildCameraRigNamePickerImpl(FCameraRigPickerConfig& PickerConfig);
	void OnPickerAssetSelected(UCameraRigAsset* SelectedItem);

private:

	TSharedPtr<IPropertyHandle> CameraRigPropertyHandle;
	TSharedPtr<SComboButton> CameraRigPickerButton;
};

}  // namespace UE::Cameras

