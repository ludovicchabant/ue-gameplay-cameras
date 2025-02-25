// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraphSchema_K2.h"
#include "EditorUndoClient.h"
#include "Templates/SharedPointer.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigInterfaceParametersToolkit.generated.h"

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

UCLASS(MinimalAPI, Hidden)
class UEdGraphSchema_CameraNodeK2 : public UEdGraphSchema_K2
{
	GENERATED_BODY()

public:

	virtual bool SupportsPinTypeContainer(TWeakPtr<const FEdGraphSchemaAction> SchemaAction, const FEdGraphPinType& PinType, const EPinContainerType& ContainerType) const
	{
		if (ContainerType == EPinContainerType::None || ContainerType == EPinContainerType::Array)
		{
			return Super::SupportsPinTypeContainer(SchemaAction, PinType, ContainerType);
		}
		return false;
	}
};

