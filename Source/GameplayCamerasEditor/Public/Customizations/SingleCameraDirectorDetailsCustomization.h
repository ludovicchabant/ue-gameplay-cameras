// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "UObject/WeakObjectPtr.h"

class IPropertyHandle;
class SComboButton;
class SWidget;
class UCameraRigAsset;
class USingleCameraDirector;

namespace UE::Cameras
{

/**
 * Details customization for the single-rig camera director.
 *
 * It shows a custom camera rig picker that only chooses among the rigs of the parent camera asset.
 */
class FSingleCameraDirectorDetailsCustomization : public IDetailCustomization
{
public:

	static TSharedRef<IDetailCustomization> MakeInstance();

public:

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	TSharedRef<SWidget> OnBuildCameraRigPicker();
	FText OnGetComboButtonText() const;
	void OnCameraRigSelected(UCameraRigAsset* CameraRig);

private:

	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<IPropertyHandle> CameraRigPropertyHandle;
	TWeakObjectPtr<USingleCameraDirector> WeakSelectedDirector;
};

}  // namespace UE::Cameras

