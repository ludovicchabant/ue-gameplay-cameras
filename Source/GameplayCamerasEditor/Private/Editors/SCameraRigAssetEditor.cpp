// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SCameraRigAssetEditor.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigTransition.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraNodeGraphNode.h"
#include "Editors/CameraNodeGraphSchema.h"
#include "Editors/CameraRigInterfaceParameterGraphNode.h"
#include "Editors/CameraTransitionGraphSchema.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Editors/SObjectTreeGraphEditor.h"
#include "GameplayCamerasEditorSettings.h"
#include "ObjectEditorUtils.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "SCameraRigAssetEditor"

namespace UE::Cameras
{

void SCameraRigAssetEditor::Construct(const FArguments& InArgs)
{
	CameraRigAsset = InArgs._CameraRigAsset;

	CurrentMode = ECameraRigAssetEditorMode::NodeGraph;

	CreateNodeGraphEditor(InArgs);
	CreateTransitionGraphEditor(InArgs);

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
		if (NodeGraph)
		{
			NodeGraph->RemoveFromRoot();
		}
		if (TransitionGraph)
		{
			TransitionGraph->RemoveFromRoot();
		}
	}
}

void SCameraRigAssetEditor::CreateNodeGraphEditor(const FArguments& InArgs)
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;
	GraphConfig.GraphName = UCameraRigAsset::NodeTreeGraphName;
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigAsset::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraNode::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigInterfaceParameter::StaticClass());
	GraphConfig.NonConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigAsset::StaticClass())
		.OnlyAsRoot()
		.HasSelfPin(false)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"))
		.GraphNodeClass(UCameraNodeGraphNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigInterfaceParameter::StaticClass())
		.SelfPinDirection(EGPD_Output)
		.SelfPinName(NAME_None)  // No self pin name, we just want the title
		.CanCreateNew(false)
		.GraphNodeClass(UCameraRigInterfaceParameterGraphNode::StaticClass());

	NodeGraph = NewObject<UObjectTreeGraph>(GetTransientPackage(), NAME_None, RF_Transactional);
	NodeGraph->Schema = UCameraNodeGraphSchema::StaticClass();
	NodeGraph->AddToRoot();
	NodeGraph->Initialize(CameraRigAsset, GraphConfig);
	NodeGraph->RebuildGraph(EObjectTreeGraphBuildSource::RootObjectPackage);

	FGraphAppearanceInfo Appearance;
	Appearance.CornerText = LOCTEXT("CameraRigGraphText", "CAMERA RIG");

	NodeGraphEditor = SNew(SObjectTreeGraphEditor)
		.Appearance(Appearance)
		.DetailsView(InArgs._DetailsView)
		.GraphTitle(this, &SCameraRigAssetEditor::GetCameraRigAssetName)
		.GraphToEdit(NodeGraph)
		.AssetEditorToolkit(InArgs._AssetEditorToolkit);
}

void SCameraRigAssetEditor::CreateTransitionGraphEditor(const FArguments& InArgs)
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;
	GraphConfig.GraphName = UCameraRigAsset::TransitionsGraphName;
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigAsset::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigTransition::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigTransitionCondition::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigAsset::StaticClass())
		.HasSelfPin(false)
		.OnlyAsRoot()
		.SetPropertyPinDirection(GET_MEMBER_NAME_STRING_CHECKED(UCameraRigAsset, EnterTransitions), EGPD_Input)
		.SetPropertyPinDirection(GET_MEMBER_NAME_STRING_CHECKED(UCameraRigAsset, ExitTransitions), EGPD_Output)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigTransition::StaticClass())
		.SetPropertyPinDirection(GET_MEMBER_NAME_STRING_CHECKED(UCameraRigTransition, Conditions), EGPD_Input)
		.NodeTitleColor(Settings->CameraRigTransitionTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigTransitionCondition::StaticClass())
		.SelfPinDirection(EGPD_Output)
		.DefaultPropertyPinDirection(EGPD_Input)
		.StripDisplayNameSuffix(TEXT("Transition Condition"))
		.NodeTitleColor(Settings->CameraRigTransitionConditionTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UBlendCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"));

	TransitionGraph = NewObject<UObjectTreeGraph>(GetTransientPackage(), NAME_None, RF_Transactional);
	TransitionGraph->Schema = UCameraTransitionGraphSchema::StaticClass();
	TransitionGraph->AddToRoot();
	TransitionGraph->Initialize(CameraRigAsset, GraphConfig);
	TransitionGraph->RebuildGraph(EObjectTreeGraphBuildSource::RootObjectPackage);

	FGraphAppearanceInfo Appearance;
	Appearance.CornerText = LOCTEXT("TransitionGraphText", "TRANSITIONS");

	TransitionGraphEditor = SNew(SObjectTreeGraphEditor)
		.Appearance(Appearance)
		.DetailsView(InArgs._DetailsView)
		.GraphTitle(this, &SCameraRigAssetEditor::GetCameraRigAssetName)
		.GraphToEdit(TransitionGraph)
		.AssetEditorToolkit(InArgs._AssetEditorToolkit);
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
	if (InMode != CurrentMode)
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
		if (UObjectTreeGraphNode* RootObjectNode = Graph->GetRootObjectNode())
		{
			JumpToNode(RootObjectNode);
		}
	}
}

void SCameraRigAssetEditor::JumpToNode(UEdGraphNode* InGraphNode)
{
	if (InGraphNode)
	{
		UEdGraph* Graph = InGraphNode->GetGraph();
		if (Graph == NodeGraph)
		{
			SetEditorMode(ECameraRigAssetEditorMode::NodeGraph);
			NodeGraphEditor->JumpToNode(InGraphNode);
		}
		else if (Graph == TransitionGraph)
		{
			SetEditorMode(ECameraRigAssetEditorMode::TransitionGraph);
			TransitionGraphEditor->JumpToNode(InGraphNode);
		}
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

FText SCameraRigAssetEditor::GetCameraRigAssetName() const
{
	return FText::FromString(CameraRigAsset->GetName());
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

