// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EditorUndoClient.h"
#include "Templates/SharedPointer.h"
#include "UObject/ObjectPtr.h"

class SBox;
class SWidget;
class UCameraRigAsset;
class UCameraRigInterfaceParameterBase;

namespace UE::Cameras
{

class SCameraRigInterfaceParametersPanel;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraRigInterfaceParameterEvent, UCameraRigInterfaceParameterBase*);

/**
 * Utility toolkit for the "interface parameters" panel of the camera rig editor.
 */
class FCameraRigInterfaceParametersToolkit 
	: public TSharedFromThis<FCameraRigInterfaceParametersToolkit>
	, public FEditorUndoClient
{
public:

	FCameraRigInterfaceParametersToolkit();
	~FCameraRigInterfaceParametersToolkit();

	/** Gets the camera rig asset to edit. */
	UCameraRigAsset* GetCameraRigAsset() const { return CameraRigAsset; }
	/** Sets the camera rig asset to edit. This re-creates the panel widget. */
	void SetCameraRigAsset(UCameraRigAsset* InCameraRigAsset);

	/** Gets the panel widget. */
	TSharedPtr<SWidget> GetInterfaceParametersPanel() const;

	/** Delegate invoked when a parmeter is selected in the panel. */
	FOnCameraRigInterfaceParameterEvent& OnInterfaceParameterSelected() { return OnInterfaceParameterSelectedDelegate; }

protected:

	// FEditorUndoClient interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

private:

	TObjectPtr<UCameraRigAsset> CameraRigAsset;

	FOnCameraRigInterfaceParameterEvent OnInterfaceParameterSelectedDelegate;

	TSharedPtr<SBox> PanelContainer;
	TSharedPtr<SCameraRigInterfaceParametersPanel> Panel;
};

}  // namespace UE::Cameras

