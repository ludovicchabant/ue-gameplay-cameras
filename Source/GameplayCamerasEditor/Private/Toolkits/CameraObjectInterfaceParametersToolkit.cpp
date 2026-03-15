// Copyright Epic Games, Inc. All Rights Reserved.

#include "Toolkits/CameraObjectInterfaceParametersToolkit.h"

#include "Commands/CameraObjectInterfaceParametersEditorCommands.h"
#include "Core/BaseCameraObject.h"
#include "EdGraph/EdGraphNode.h"
#include "Editor.h"
#include "Editors/CameraObjectInterfaceParameterGraphNode.h"
#include "Editors/SCameraNodeGraphEditor.h"
#include "Editors/SCameraObjectInterfaceParametersPanel.h"
#include "SPinTypeSelector.h"
#include "ToolMenus.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "CameraObjectInterfaceParametersToolkit"

namespace UE::Cameras
{

FCameraObjectInterfaceParametersToolkit::FCameraObjectInterfaceParametersToolkit()
{
	SAssignNew(PanelContainer, SBox);

	if (GEditor)
	{
		GEditor->RegisterForUndo(this);
	}
}

FCameraObjectInterfaceParametersToolkit::~FCameraObjectInterfaceParametersToolkit()
{
	if (GEditor)
	{
		GEditor->UnregisterForUndo(this);
	}
}

void FCameraObjectInterfaceParametersToolkit::Initialize(FOnGetFocusedCameraNodeGraphEditor&& InDelegate)
{
	GetFocusedCameraNodeGraphEditor = MoveTemp(InDelegate);
}

void FCameraObjectInterfaceParametersToolkit::SetCameraObject(UBaseCameraObject* InCameraObject)
{
	if (CameraObject != InCameraObject)
	{
		PanelContainer->SetContent(SNullWidget::NullWidget);

		CameraObject = InCameraObject;

		if (CameraObject)
		{
			Panel = SNew(SCameraObjectInterfaceParametersPanel, this);
			PanelContainer->SetContent(Panel.ToSharedRef());
		}
	}
}

TSharedPtr<SWidget> FCameraObjectInterfaceParametersToolkit::GetInterfaceParametersPanel() const
{
	return PanelContainer;
}

void FCameraObjectInterfaceParametersToolkit::PostUndo(bool bSuccess)
{
	Panel->RequestListRefresh();
}

void FCameraObjectInterfaceParametersToolkit::PostRedo(bool bSuccess)
{
	Panel->RequestListRefresh();
}

void FCameraObjectInterfaceParametersToolkit::BindCommands(TSharedRef<FUICommandList> CommandList)
{
	const FCameraObjectInterfaceParametersEditorCommands& Commands = FCameraObjectInterfaceParametersEditorCommands::Get();

	CommandList->MapAction(
			Commands.RenameInterfaceParameter,
			FExecuteAction::CreateSP(this, &FCameraObjectInterfaceParametersToolkit::RenameSelectedParameter));

	CommandList->MapAction(
			Commands.DeleteInterfaceParameter,
			FExecuteAction::CreateSP(this, &FCameraObjectInterfaceParametersToolkit::DeleteSelectedParameter));

	if (GetFocusedCameraNodeGraphEditor.IsBound())
	{
		if (TSharedPtr<SCameraNodeGraphEditor> GraphEditor = GetFocusedCameraNodeGraphEditor.Execute())
		{
			if (TSharedPtr<FUICommandList> GraphCommandList = GraphEditor->GetCommandList())
			{
				GraphCommandList->MapAction(
						Commands.GoToInterfaceParameter,
						FExecuteAction::CreateSP(this, &FCameraObjectInterfaceParametersToolkit::SelectParameterFromGraphEditorSelection));
			}
		}
	}
}

void FCameraObjectInterfaceParametersToolkit::RenameSelectedParameter()
{
	Panel->RenameSelectedParameter();
}

void FCameraObjectInterfaceParametersToolkit::DeleteSelectedParameter()
{
	Panel->DeleteSelectedParameter();
}

void FCameraObjectInterfaceParametersToolkit::SelectParameterFromGraphEditorSelection()
{
	if (!GetFocusedCameraNodeGraphEditor.IsBound())
	{
		return;
	}

	TSharedPtr<SCameraNodeGraphEditor> GraphEditor = GetFocusedCameraNodeGraphEditor.Execute();
	if (!GraphEditor)
	{
		return;
	}

	const UCameraObjectInterfaceParameterGraphNode* SelectedNode = Cast<UCameraObjectInterfaceParameterGraphNode>(
			GraphEditor->GetGraphEditor()->GetSingleSelectedNode());
	if (!SelectedNode)
	{
		return;
	}

	Panel->SelectParameter(SelectedNode->GetInterfaceParameter());
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

