// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigAssetEditor.h"

#include "Core/CameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigTransition.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraNodeGraphSchema.h"
#include "Editors/CameraRigTransitionGraphSchema.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Editors/SObjectTreeGraphEditor.h"
#include "ObjectEditorUtils.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "SCameraRigAssetEditor"

namespace UE::Cameras
{

void SCameraRigAssetEditor::Construct(const FArguments& InArgs)
{
	CameraRigAsset = InArgs._CameraRigAsset;
	DetailsView = InArgs._DetailsView;
	AssetEditorToolkit = InArgs._AssetEditorToolkit;

	CurrentMode = ECameraRigAssetEditorMode::NodeGraph;

	CreateGraphEditors();

	ChildSlot
	[
		SAssignNew(BoxPanel, SBox)
		[
			NodeGraphEditor.ToSharedRef()
		]
	];
}

SCameraRigAssetEditor::~SCameraRigAssetEditor()
{
	if (!GExitPurge)
	{
		DiscardGraphEditors();
	}
}

void SCameraRigAssetEditor::SetCameraRigAsset(UCameraRigAsset* InCameraRig)
{
	if (CameraRigAsset != InCameraRig)
	{
		DiscardGraphEditors();

		CameraRigAsset = InCameraRig;

		CreateGraphEditors();

		SetEditorModeImpl(CurrentMode, true);
	}
}

void SCameraRigAssetEditor::CreateGraphEditors()
{
	CreateNodeGraphEditor();
	CreateTransitionGraphEditor();
}

void SCameraRigAssetEditor::CreateNodeGraphEditor()
{
	UClass* SchemaClass = UCameraNodeGraphSchema::StaticClass();
	UCameraNodeGraphSchema* DefaultSchemaObject = Cast<UCameraNodeGraphSchema>(SchemaClass->GetDefaultObject());
	FObjectTreeGraphConfig GraphConfig = DefaultSchemaObject->BuildGraphConfig();

	NodeGraph = NewObject<UObjectTreeGraph>(GetTransientPackage(), NAME_None, RF_Transactional | RF_Standalone);
	NodeGraph->Schema = SchemaClass;
	NodeGraph->Reset(CameraRigAsset, GraphConfig);

	NodeGraphChangedHandle = NodeGraph->AddOnGraphChangedHandler(
			FOnGraphChanged::FDelegate::CreateSP(this, &SCameraRigAssetEditor::OnGraphChanged));

	FGraphAppearanceInfo Appearance;
	Appearance.CornerText = LOCTEXT("CameraRigGraphText", "CAMERA NODES");

	NodeGraphEditor = SNew(SObjectTreeGraphEditor)
		.Appearance(Appearance)
		.DetailsView(DetailsView)
		.GraphTitle(this, &SCameraRigAssetEditor::GetCameraRigAssetName, NodeGraph.Get())
		.IsEnabled(this, &SCameraRigAssetEditor::IsGraphEditorEnabled)
		.GraphToEdit(NodeGraph)
		.AssetEditorToolkit(AssetEditorToolkit);
}

void SCameraRigAssetEditor::CreateTransitionGraphEditor()
{
	UClass* SchemaClass = UCameraRigTransitionGraphSchema::StaticClass();
	UCameraRigTransitionGraphSchema* DefaultSchemaObject = Cast<UCameraRigTransitionGraphSchema>(SchemaClass->GetDefaultObject());
	FObjectTreeGraphConfig GraphConfig = DefaultSchemaObject->BuildGraphConfig();

	TransitionGraph = NewObject<UObjectTreeGraph>(GetTransientPackage(), NAME_None, RF_Transactional | RF_Standalone);
	TransitionGraph->Schema = SchemaClass;
	TransitionGraph->Reset(CameraRigAsset, GraphConfig);

	TransitionGraphChangedHandle = TransitionGraph->AddOnGraphChangedHandler(
			FOnGraphChanged::FDelegate::CreateSP(this, &SCameraRigAssetEditor::OnGraphChanged));

	FGraphAppearanceInfo Appearance;
	Appearance.CornerText = LOCTEXT("TransitionGraphText", "TRANSITIONS");

	TransitionGraphEditor = SNew(SObjectTreeGraphEditor)
		.Appearance(Appearance)
		.DetailsView(DetailsView)
		.GraphTitle(this, &SCameraRigAssetEditor::GetCameraRigAssetName, TransitionGraph.Get())
		.IsEnabled(this, &SCameraRigAssetEditor::IsGraphEditorEnabled)
		.GraphToEdit(TransitionGraph)
		.AssetEditorToolkit(AssetEditorToolkit);
}

void SCameraRigAssetEditor::DiscardGraphEditors()
{
	TArray<TTuple<UObjectTreeGraph*, FDelegateHandle>> Graphs 
	{ 
		{ NodeGraph, NodeGraphChangedHandle },
		{ TransitionGraph, TransitionGraphChangedHandle } 
	};
	for (auto& Pair : Graphs)
	{
		UObjectTreeGraph* Graph(Pair.Key);
		FDelegateHandle GraphChangedHandle(Pair.Value);
		if (Graph)
		{
			Graph->RemoveFromRoot();

			if (GraphChangedHandle.IsValid())
			{
				Graph->RemoveOnGraphChangedHandler(GraphChangedHandle);
			}
		}
	}

	NodeGraphChangedHandle.Reset();
	TransitionGraphChangedHandle.Reset();

	// WARNING: the graph editors (and their graphs) are still in use as widgets 
	//			in the layout until they are replaced!
}

ECameraRigAssetEditorMode SCameraRigAssetEditor::GetEditorMode() const
{
	return CurrentMode;
}

bool SCameraRigAssetEditor::IsEditorMode(ECameraRigAssetEditorMode InMode) const
{
	return CurrentMode == InMode;
}

void SCameraRigAssetEditor::SetEditorMode(ECameraRigAssetEditorMode InMode)
{
	SetEditorModeImpl(InMode, false);
}

void SCameraRigAssetEditor::SetEditorModeImpl(ECameraRigAssetEditorMode InMode, bool bForceSet)
{
	if (bForceSet || InMode != CurrentMode)
	{
		TSharedPtr<SObjectTreeGraphEditor> CurrentGraphEditor;
		switch(InMode)
		{
			case ECameraRigAssetEditorMode::NodeGraph:
			default:
				CurrentGraphEditor = NodeGraphEditor;
				break;
			case ECameraRigAssetEditorMode::TransitionGraph:
				CurrentGraphEditor = TransitionGraphEditor;
				break;
		}

		BoxPanel->SetContent(CurrentGraphEditor.ToSharedRef());
		CurrentGraphEditor->ResyncDetailsView();
		CurrentMode = InMode;
	}
}

void SCameraRigAssetEditor::GetGraphs(TArray<UEdGraph*>& OutGraphs) const
{
	OutGraphs.Add(NodeGraph);
	OutGraphs.Add(TransitionGraph);
}

UEdGraph* SCameraRigAssetEditor::GetFocusedGraph() const
{
	switch (CurrentMode)
	{
		case ECameraRigAssetEditorMode::NodeGraph:
			return NodeGraph;
		case ECameraRigAssetEditorMode::TransitionGraph:
			return TransitionGraph;
		default:
			ensure(false);
			return nullptr;
	}
}

const FObjectTreeGraphConfig& SCameraRigAssetEditor::GetFocusedGraphConfig() const
{
	static const FObjectTreeGraphConfig DefaultConfig;

	switch (CurrentMode)
	{
		case ECameraRigAssetEditorMode::NodeGraph:
			return NodeGraph->GetConfig();
		case ECameraRigAssetEditorMode::TransitionGraph:
			return TransitionGraph->GetConfig();
		default:
			ensure(false);
			return DefaultConfig;
	}
}

void SCameraRigAssetEditor::FocusHome()
{
	UObjectTreeGraph* Graph = nullptr;
	TSharedPtr<SObjectTreeGraphEditor> GraphEditor = nullptr;

	switch (CurrentMode)
	{
		case ECameraRigAssetEditorMode::NodeGraph:
			Graph = NodeGraph;
			GraphEditor = NodeGraphEditor;
			break;
		case ECameraRigAssetEditorMode::TransitionGraph:
			Graph = TransitionGraph;
			GraphEditor = TransitionGraphEditor;
	}

	if (Graph && GraphEditor)
	{
		FindAndJumpToObjectNode(CameraRigAsset);
	}
}

bool SCameraRigAssetEditor::FindAndJumpToObjectNode(UObject* InObject)
{
	if (UObjectTreeGraphNode* NodeGraphObjectNode = NodeGraph->FindObjectNode(InObject))
	{
		SetEditorMode(ECameraRigAssetEditorMode::NodeGraph);
		NodeGraphEditor->JumpToNode(NodeGraphObjectNode);
		return true;
	}
	if (UObjectTreeGraphNode* TransitionGraphObjectNode = TransitionGraph->FindObjectNode(InObject))
	{
		SetEditorMode(ECameraRigAssetEditorMode::TransitionGraph);
		TransitionGraphEditor->JumpToNode(TransitionGraphObjectNode);
		return true;
	}
	return false;
}

FText SCameraRigAssetEditor::GetCameraRigAssetName(UObjectTreeGraph* ForGraph) const
{
	if (CameraRigAsset && ForGraph)
	{
		const FObjectTreeGraphConfig& GraphConfig = TransitionGraph->GetConfig();
		return GraphConfig.GetDisplayNameText(CameraRigAsset);
	}
	return LOCTEXT("NoCameraRig", "No Camera Rig");
}

bool SCameraRigAssetEditor::IsGraphEditorEnabled() const
{
	return CameraRigAsset != nullptr;
}

void SCameraRigAssetEditor::OnGraphChanged(const FEdGraphEditAction& InEditAction)
{
	OnAnyGraphChanged.Broadcast(InEditAction);
}

FDelegateHandle SCameraRigAssetEditor::AddOnAnyGraphChanged(FOnGraphChanged::FDelegate InAddDelegate)
{
	return OnAnyGraphChanged.Add(InAddDelegate);
}

void SCameraRigAssetEditor::RemoveOnAnyGraphChanged(FDelegateHandle InDelegateHandle)
{
	if (InDelegateHandle.IsValid())
	{
		OnAnyGraphChanged.Remove(InDelegateHandle);
	}
}

void SCameraRigAssetEditor::RemoveOnAnyGraphChanged(const void* InUserObject)
{
	OnAnyGraphChanged.RemoveAll(InUserObject);
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

