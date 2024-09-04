// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SObjectTreeGraphEditor.h"

#include "Algo/AnyOf.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Editor.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Editors/ObjectTreeGraphSchema.h"
#include "Editors/SObjectTreeGraphTitleBar.h"
#include "Editors/SObjectTreeGraphToolbox.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDetailsView.h"
#include "SNodePanel.h"
#include "ScopedTransaction.h"
#include "SGraphPanel.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "SObjectTreeGraphEditor"

void SObjectTreeGraphEditor::Construct(const FArguments& InArgs)
{
	DetailsView = InArgs._DetailsView;

	TSharedPtr<SWidget> GraphTitleBar = InArgs._GraphTitleBar;
	if (!GraphTitleBar.IsValid())
	{
		TSharedRef<SWidget> DefaultTitleBar = SNew(SObjectTreeGraphTitleBar)
			.Graph(InArgs._GraphToEdit)
			.TitleText(InArgs._GraphTitle);
		GraphTitleBar = DefaultTitleBar.ToSharedPtr();
	}

	SGraphEditor::FGraphEditorEvents GraphEditorEvents;
	GraphEditorEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &SObjectTreeGraphEditor::OnGraphSelectionChanged);
	GraphEditorEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &SObjectTreeGraphEditor::OnNodeTextCommitted);
	GraphEditorEvents.OnDoubleClicked = SGraphEditor::FOnDoubleClicked::CreateSP(this, &SObjectTreeGraphEditor::OnDoubleClicked);
	GraphEditorEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &SObjectTreeGraphEditor::OnNodeDoubleClicked);

	InitializeBuiltInCommands();

	TSharedPtr<FUICommandList> AdditionalCommands = BuiltInCommands;
	if (InArgs._AdditionalCommands)
	{
		AdditionalCommands = MakeShared<FUICommandList>();
		AdditionalCommands->Append(BuiltInCommands.ToSharedRef());
		AdditionalCommands->Append(InArgs._AdditionalCommands.ToSharedRef());
	}

	GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(AdditionalCommands)
		.Appearance(InArgs._Appearance)
		.TitleBar(GraphTitleBar)
		.GraphToEdit(InArgs._GraphToEdit)
		.GraphEvents(GraphEditorEvents)
		.AssetEditorToolkit(InArgs._AssetEditorToolkit);

	ChildSlot
	[
		GraphEditor.ToSharedRef()
	];

	GEditor->RegisterForUndo(this);
}

SObjectTreeGraphEditor::~SObjectTreeGraphEditor()
{
	GEditor->UnregisterForUndo(this);
}

void SObjectTreeGraphEditor::InitializeBuiltInCommands()
{
	if (BuiltInCommands.IsValid())
	{
		return;
	}

	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();

	BuiltInCommands = MakeShared<FUICommandList>();

	// Generic commands.
	BuiltInCommands->MapAction(GenericCommands.SelectAll,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::SelectAllNodes),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanSelectAllNodes)
		);

	BuiltInCommands->MapAction(GenericCommands.Delete,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanDeleteSelectedNodes)
		);

	BuiltInCommands->MapAction(GenericCommands.Copy,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanCopySelectedNodes)
		);

	BuiltInCommands->MapAction(GenericCommands.Cut,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanCutSelectedNodes)
		);

	BuiltInCommands->MapAction(GenericCommands.Paste,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::PasteNodes),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanPasteNodes)
		);

	BuiltInCommands->MapAction(GenericCommands.Duplicate,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::DuplicateNodes),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanDuplicateNodes)
		);
	BuiltInCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::CanRenameNode)
	);

	// Alignment commands.
	BuiltInCommands->MapAction(GraphEditorCommands.AlignNodesTop,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnAlignTop)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.AlignNodesMiddle,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnAlignMiddle)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.AlignNodesBottom,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnAlignBottom)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.AlignNodesLeft,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnAlignLeft)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.AlignNodesCenter,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnAlignCenter)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.AlignNodesRight,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnAlignRight)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.StraightenConnections,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnStraightenConnections)
			);

	// Distribution commands.
	BuiltInCommands->MapAction(GraphEditorCommands.DistributeNodesHorizontally,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnDistributeNodesHorizontally)
			);

	BuiltInCommands->MapAction(GraphEditorCommands.DistributeNodesVertically,
			FExecuteAction::CreateSP(this, &SObjectTreeGraphEditor::OnDistributeNodesVertically)
			);
}

void SObjectTreeGraphEditor::JumpToNode(UEdGraphNode* InNode)
{
	GraphEditor->JumpToNode(InNode);
}

void SObjectTreeGraphEditor::ResyncDetailsView()
{
	OnGraphSelectionChanged(GraphEditor->GetSelectedNodes());
}

FReply SObjectTreeGraphEditor::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FObjectTreeClassDragDropOp> ObjectClassOp = DragDropEvent.GetOperationAs<FObjectTreeClassDragDropOp>();
	if (ObjectClassOp)
	{
		TArray<UClass*> PlaceableClasses = FilterPlaceableObjectClasses(ObjectClassOp->GetObjectClasses());
		if (PlaceableClasses.Num() == ObjectClassOp->GetObjectClasses().Num())
		{
			const FSlateBrush* OKIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"));
			ObjectClassOp->SetToolTip(
					FText::Format(
						LOCTEXT("OnDragOver_Success", "Create {0} node(s) from the dragged object classes"),
						ObjectClassOp->GetObjectClasses().Num()),
					OKIcon);
		}
		else if (PlaceableClasses.Num() > 0)
		{
			const FSlateBrush* WarnIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OKWarn"));
			ObjectClassOp->SetToolTip(
					FText::Format(
						LOCTEXT("OnDragOver_Warning", "Create {0} node(s) from the dragged object classes, ignoring {1} that can't be created in this graph"),
						PlaceableClasses.Num(), (ObjectClassOp->GetObjectClasses().Num() - PlaceableClasses.Num())),
					WarnIcon);
		}
		else
		{
			const FSlateBrush* ErrorIcon = FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
			ObjectClassOp->SetToolTip(
					LOCTEXT("OnDragOver_Error", "The dragged object classes can't be created in this graph"),
					ErrorIcon);
		}

		return FReply::Handled();
	}

	return SCompoundWidget::OnDragOver(MyGeometry, DragDropEvent);
}

FReply SObjectTreeGraphEditor::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FObjectTreeClassDragDropOp> ObjectClassOp = DragDropEvent.GetOperationAs<FObjectTreeClassDragDropOp>();
	if (ObjectClassOp)
	{
		const FScopedTransaction Transaction(LOCTEXT("DropObjectClasses", "Drop New Nodes"));

		TArray<UClass*> PlaceableClasses = FilterPlaceableObjectClasses(ObjectClassOp->GetObjectClasses());
		UEdGraph* Graph = GraphEditor->GetCurrentGraph();

		GraphEditor->ClearSelectionSet();

		SGraphPanel* GraphPanel = GraphEditor->GetGraphPanel();
		FVector2D NewLocation = GraphPanel->PanelCoordToGraphCoord(MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()));

		for (UClass* PlaceableClass : PlaceableClasses)
		{
			FObjectGraphSchemaAction_NewNode Action;
			Action.ObjectClass = PlaceableClass;
			UEdGraphNode* NewNode = Action.PerformAction(Graph, nullptr, NewLocation, false);
			GraphEditor->SetNodeSelection(NewNode, true);

			NewLocation += FVector2D(20, 20);
		}
	}

	return SCompoundWidget::OnDrop(MyGeometry, DragDropEvent);
}

TArray<UClass*> SObjectTreeGraphEditor::FilterPlaceableObjectClasses(TArrayView<UClass* const> InObjectClasses)
{
	UObjectTreeGraph* Graph = CastChecked<UObjectTreeGraph>(GraphEditor->GetCurrentGraph());
	const FObjectTreeGraphConfig& GraphConfig = Graph->GetConfig();
	TArray<UClass*> PlaceableClasses = InObjectClasses.FilterByPredicate(
			[&GraphConfig](UClass* ObjectClass)
			{
				return GraphConfig.IsConnectable(ObjectClass);
			});
	return PlaceableClasses;
}

void SObjectTreeGraphEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		GraphEditor->ClearSelectionSet();

		GraphEditor->NotifyGraphChanged();

		FSlateApplication::Get().DismissAllMenus();
	}
}

void SObjectTreeGraphEditor::PostRedo(bool bSuccess)
{
	PostUndo(bSuccess);
}

void SObjectTreeGraphEditor::OnGraphSelectionChanged(const FGraphPanelSelectionSet& SelectionSet)
{
	if (DetailsView)
	{
		TArray<UObject*> SelectedObjects;
		for (UObject* Selection : SelectionSet)
		{
			if (UObjectTreeGraphNode* GraphNode = Cast<UObjectTreeGraphNode>(Selection))
			{
				SelectedObjects.Add(GraphNode->GetObject());
			}
		}

		DetailsView->SetObjects(SelectedObjects);
	}
}

void SObjectTreeGraphEditor::OnNodeTextCommitted(const FText& InText, ETextCommit::Type InCommitType, UEdGraphNode* InEditedNode)
{
	if (InEditedNode)
	{
		FString NewName = InText.ToString().TrimStartAndEnd();
		if (NewName.IsEmpty())
		{
			return;
		}

		if (NewName.Len() >= NAME_SIZE)
		{
			NewName = NewName.Left(NAME_SIZE - 1);
		}

		const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));

		InEditedNode->Modify();
		InEditedNode->OnRenameNode(NewName);

		GraphEditor->GetCurrentGraph()->NotifyNodeChanged(InEditedNode);
	}
}

void SObjectTreeGraphEditor::OnNodeDoubleClicked(UEdGraphNode* InClickedNode)
{
	UObjectTreeGraphNode* SelectedNode = Cast<UObjectTreeGraphNode>(InClickedNode);
	if (SelectedNode)
	{
		SelectedNode->OnDoubleClicked();
	}
}

void SObjectTreeGraphEditor::OnDoubleClicked()
{
}

FString SObjectTreeGraphEditor::ExportNodesToText(const FGraphPanelSelectionSet& Nodes, bool bOnlyCanDuplicateNodes, bool bOnlyCanDeleteNodes)
{
	UEdGraph* CurrentGraph = GraphEditor->GetCurrentGraph();
	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(CurrentGraph->GetSchema());

	return Schema->ExportNodesToText(Nodes, bOnlyCanDuplicateNodes, bOnlyCanDeleteNodes);
}

void SObjectTreeGraphEditor::ImportNodesFromText(const FVector2D& Location, const FString& TextToImport)
{
	// Start a transaction and flag things as modified.
	const FScopedTransaction Transaction(LOCTEXT("PasteNodes", "Paste Nodes"));

	UObjectTreeGraph* Graph = CastChecked<UObjectTreeGraph>(GraphEditor->GetCurrentGraph());
	Graph->Modify();

	UPackage* ObjectPackage = Graph->GetRootObject()->GetOutermost();
	ObjectPackage->Modify();

	// Import the nodes.
	TArray<UEdGraphNode*> PastedNodes;
	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(Graph->GetSchema());
	Schema->ImportNodesFromText(Graph, TextToImport, PastedNodes);

	// Compute the center of the pasted nodes.
	FVector2D PastedNodesClusterCenter(FVector2D::ZeroVector);
	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		PastedNodesClusterCenter.X += PastedNode->NodePosX;
		PastedNodesClusterCenter.Y += PastedNode->NodePosY;
	}
	if (PastedNodes.Num() > 0)
	{
		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		PastedNodesClusterCenter.X *= InvNumNodes;
		PastedNodesClusterCenter.Y *= InvNumNodes;
	}

	// Move all pasted nodes to the new location, and select them.
	GraphEditor->ClearSelectionSet();

	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		PastedNode->NodePosX = (PastedNode->NodePosX - PastedNodesClusterCenter.X) + Location.X ;
		PastedNode->NodePosY = (PastedNode->NodePosY - PastedNodesClusterCenter.Y) + Location.Y ;

		PastedNode->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Notify object nodes of having been moved so that we save the new position
		// in the underlying data.
		if (UObjectTreeGraphNode* PastedObjectNode = Cast<UObjectTreeGraphNode>(PastedNode))
		{
			PastedObjectNode->OnGraphNodeMoved(false);
		}

		GraphEditor->SetNodeSelection(PastedNode, true);
	}

	// Update the UI.
	GraphEditor->NotifyGraphChanged();
}

bool SObjectTreeGraphEditor::CanImportNodesFromText(const FString& TextToImport)
{
	UObjectTreeGraph* CurrentGraph = CastChecked<UObjectTreeGraph>(GraphEditor->GetCurrentGraph());
	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(CurrentGraph->GetSchema());

	return Schema->CanImportNodesFromText(CurrentGraph, TextToImport);
}

void SObjectTreeGraphEditor::DeleteNodes(TArrayView<UObjectTreeGraphNode*> NodesToDelete)
{
	UEdGraph* CurrentGraph = GraphEditor->GetCurrentGraph();
	const UEdGraphSchema* Schema = CurrentGraph->GetSchema();

	const FScopedTransaction Transaction(LOCTEXT("DeleteNode", "Delete Node(s)"));

	for (UObjectTreeGraphNode* Node : NodesToDelete)
	{
		if (Node)
		{
			Schema->SafeDeleteNodeFromGraph(CurrentGraph, Node);

			Node->DestroyNode();
		}
	}
}

void SObjectTreeGraphEditor::SelectAllNodes()
{
	GraphEditor->SelectAllNodes();
}

bool SObjectTreeGraphEditor::CanSelectAllNodes()
{
	return true;
}

void SObjectTreeGraphEditor::DeleteSelectedNodes()
{
	TArray<UObjectTreeGraphNode*> NodesToDelete;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* GraphNode = Cast<UEdGraphNode>(*NodeIt);
		if (GraphNode && GraphNode->CanUserDeleteNode())
		{
			NodesToDelete.Add(Cast<UObjectTreeGraphNode>(*NodeIt));
		}
	}
	
	DeleteNodes(NodesToDelete);

	// Remove deleted nodes from the details view.
	GraphEditor->ClearSelectionSet();
}

bool SObjectTreeGraphEditor::CanDeleteSelectedNodes()
{
	bool bDeletableNodeExists = false;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* GraphNode = Cast<UEdGraphNode>(*NodeIt);
		if (GraphNode && GraphNode->CanUserDeleteNode())
		{
			bDeletableNodeExists = true;
		}
	}

	return SelectedNodes.Num() > 0 && bDeletableNodeExists;
}

void SObjectTreeGraphEditor::CopySelectedNodes()
{
	const FString Buffer = ExportNodesToText(GraphEditor->GetSelectedNodes(), true, false);
	FPlatformApplicationMisc::ClipboardCopy(*Buffer);
}

bool SObjectTreeGraphEditor::CanCopySelectedNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt);
		if (Node != nullptr && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void SObjectTreeGraphEditor::CutSelectedNodes()
{
	const FString Buffer = ExportNodesToText(GraphEditor->GetSelectedNodes(), true, true);
	FPlatformApplicationMisc::ClipboardCopy(*Buffer);

	DeleteSelectedNodes();
}

bool SObjectTreeGraphEditor::CanCutSelectedNodes()
{
	return CanCopySelectedNodes() && CanDeleteSelectedNodes();
}

void SObjectTreeGraphEditor::PasteNodes()
{
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);
	
	ImportNodesFromText(GraphEditor->GetPasteLocation(), TextToImport);
}

bool SObjectTreeGraphEditor::CanPasteNodes()
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return CanImportNodesFromText(ClipboardContent);
}

void SObjectTreeGraphEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool SObjectTreeGraphEditor::CanDuplicateNodes()
{
	return CanCopySelectedNodes();
}

void SObjectTreeGraphEditor::OnRenameNode()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
		if (SelectedNode != nullptr && SelectedNode->GetCanRenameNode())
		{
			const bool bRequestRename = true;
			GraphEditor->IsNodeTitleVisible(SelectedNode, bRequestRename);
			break;
		}
	}
}

bool SObjectTreeGraphEditor::CanRenameNode()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt);
		if (Node != nullptr && Node->GetCanRenameNode())
		{
			return true;
		}
	}

	return false;
}

void SObjectTreeGraphEditor::OnAlignTop()
{
	GraphEditor->OnAlignTop();
}

void SObjectTreeGraphEditor::OnAlignMiddle()
{
	GraphEditor->OnAlignMiddle();
}

void SObjectTreeGraphEditor::OnAlignBottom()
{
	GraphEditor->OnAlignBottom();
}

void SObjectTreeGraphEditor::OnAlignLeft()
{
	GraphEditor->OnAlignLeft();
}

void SObjectTreeGraphEditor::OnAlignCenter()
{
	GraphEditor->OnAlignCenter();
}

void SObjectTreeGraphEditor::OnAlignRight()
{
	GraphEditor->OnAlignRight();
}

void SObjectTreeGraphEditor::OnStraightenConnections()
{
	GraphEditor->OnStraightenConnections();
}

void SObjectTreeGraphEditor::OnDistributeNodesHorizontally()
{
	GraphEditor->OnDistributeNodesH();
}

void SObjectTreeGraphEditor::OnDistributeNodesVertically()
{
	GraphEditor->OnDistributeNodesV();
}

#undef LOCTEXT_NAMESPACE

