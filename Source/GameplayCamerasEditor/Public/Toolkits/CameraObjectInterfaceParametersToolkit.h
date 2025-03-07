// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraphSchema_K2.h"
#include "EditorUndoClient.h"
#include "Templates/SharedPointer.h"
#include "UObject/ObjectPtr.h"

#include "CameraObjectInterfaceParametersToolkit.generated.h"

class SBox;
class SWidget;
class UBaseCameraObject;
class UCameraObjectInterfaceParameterBase;

namespace UE::Cameras
{

class SCameraObjectInterfaceParametersPanel;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraObjectInterfaceParameterEvent, UCameraObjectInterfaceParameterBase*);

/**
 * Utility toolkit for the "interface parameters" panel of any camera object editor.
 */
class FCameraObjectInterfaceParametersToolkit 
	: public TSharedFromThis<FCameraObjectInterfaceParametersToolkit>
	, public FEditorUndoClient
{
public:

	FCameraObjectInterfaceParametersToolkit();
	~FCameraObjectInterfaceParametersToolkit();

	/** Gets the camera object asset to edit. */
	UBaseCameraObject* GetCameraObject() const { return CameraObject; }
	/** Sets the camera object to edit. This re-creates the panel widget. */
	void SetCameraObject(UBaseCameraObject* InCameraObject);

	/** Gets the panel widget. */
	TSharedPtr<SWidget> GetInterfaceParametersPanel() const;

	/** Delegate invoked when a parmeter is selected in the panel. */
	FOnCameraObjectInterfaceParameterEvent& OnInterfaceParameterSelected() { return OnInterfaceParameterSelectedDelegate; }

protected:

	// FEditorUndoClient interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

private:

	TObjectPtr<UBaseCameraObject> CameraObject;

	FOnCameraObjectInterfaceParameterEvent OnInterfaceParameterSelectedDelegate;

	TSharedPtr<SBox> PanelContainer;
	TSharedPtr<SCameraObjectInterfaceParametersPanel> Panel;
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

