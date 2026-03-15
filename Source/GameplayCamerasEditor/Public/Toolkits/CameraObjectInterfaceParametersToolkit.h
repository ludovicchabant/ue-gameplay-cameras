// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraphSchema_K2.h"
#include "EditorUndoClient.h"
#include "Templates/SharedPointer.h"
#include "UObject/ObjectPtr.h"

class FUICommandList;
class SBox;
class SWidget;
class UBaseCameraObject;
class SCameraNodeGraphEditor;
class UCameraObjectInterfaceParameterBase;

namespace UE::Cameras
{

class SCameraObjectInterfaceParametersPanel;

DECLARE_DELEGATE_RetVal(TSharedPtr<SCameraNodeGraphEditor>, FOnGetFocusedCameraNodeGraphEditor);
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

	/** Initialize this toolkit with a way to get the current graph editor. */
	void Initialize(FOnGetFocusedCameraNodeGraphEditor&& InDelegate);

	/** Gets the camera object asset to edit. */
	UBaseCameraObject* GetCameraObject() const { return CameraObject; }
	/** Sets the camera object to edit. This re-creates the panel widget. */
	void SetCameraObject(UBaseCameraObject* InCameraObject);

	/** Gets the panel widget. */
	TSharedPtr<SWidget> GetInterfaceParametersPanel() const;

public:

	/** Binds specific commands to another toolkit. */
	void BindCommands(TSharedRef<FUICommandList> CommandList);

	/** Rename the selected parameter in the focused panel. */
	void RenameSelectedParameter();

	/** Delete the selected parameter in the focused panel. */
	void DeleteSelectedParameter();

	/** Selects the parameter that corresponds to the current graph editor selection. */
	void SelectParameterFromGraphEditorSelection();

public:

	/** Delegate invoked when a parmeter is selected in the panel. */
	FOnCameraObjectInterfaceParameterEvent& OnInterfaceParameterSelected() { return OnInterfaceParameterSelectedDelegate; }

	/** Delegate invoked when the user requests finding getter nodes in the graph. */
	FOnCameraObjectInterfaceParameterEvent& OnSearchInterfaceParameterNodes() { return OnSearchInterfaceParameterNodesDelegate; }

protected:

	// FEditorUndoClient interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

private:

	FOnGetFocusedCameraNodeGraphEditor GetFocusedCameraNodeGraphEditor;

	TObjectPtr<UBaseCameraObject> CameraObject;

	FOnCameraObjectInterfaceParameterEvent OnInterfaceParameterSelectedDelegate;
	FOnCameraObjectInterfaceParameterEvent OnSearchInterfaceParameterNodesDelegate;

	TSharedPtr<SBox> PanelContainer;
	TSharedPtr<SCameraObjectInterfaceParametersPanel> Panel;
};

}  // namespace UE::Cameras

