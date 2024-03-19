// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EditorUndoClient.h"
#include "GraphEditor.h"
#include "Widgets/SCompoundWidget.h"

class FAssetEditorToolkit;
class IDetailsView;
class UObjectTreeGraph;
class UObjectTreeGraphNode;

class SObjectTreeGraphEditor 
	: public SCompoundWidget
	, public FEditorUndoClient
{
public:

	SLATE_BEGIN_ARGS(SObjectTreeGraphEditor)
	{}
		SLATE_ARGUMENT(TSharedPtr<FUICommandList>, AdditionalCommands)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, GraphTitleBar)
		SLATE_ARGUMENT(TSharedPtr<IDetailsView>, DetailsView)
		SLATE_ARGUMENT(UObjectTreeGraph*, GraphToEdit)
		SLATE_ARGUMENT(TWeakPtr<FAssetEditorToolkit>, AssetEditorToolkit)
		SLATE_ATTRIBUTE(FGraphAppearanceInfo, Appearance)
		SLATE_ATTRIBUTE(FText, GraphTitle)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SObjectTreeGraphEditor();

	void JumpToNode(UEdGraphNode* InNode);
	void ResyncDetailsView();

protected:

	// SWidget interface.
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

	// FEditorUndoClient interface.
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

protected:

	void InitializeBuiltInCommands();

	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& SelectionSet);
	void OnNodeTextCommitted(const FText& InText, ETextCommit::Type InCommitType, UEdGraphNode* InEditedNode);
	void OnNodeDoubleClicked(UEdGraphNode* InClickedNode);
	void OnDoubleClicked();

	FString ExportNodesToText(const FGraphPanelSelectionSet& Nodes, bool bOnlyCanDuplicateNodes, bool bOnlyCanDeleteNodes);
	void ImportNodesFromText(const FVector2D& Location, const FString& TextToImport);
	void DeleteNodes(TArrayView<UObjectTreeGraphNode*> NodesToDelete);

	void SelectAllNodes();
	bool CanSelectAllNodes();

	void DeleteSelectedNodes();
	bool CanDeleteSelectedNodes();

	void CopySelectedNodes();
	bool CanCopySelectedNodes();

	void CutSelectedNodes();
	bool CanCutSelectedNodes();

	void PasteNodes();
	bool CanPasteNodes();

	void DuplicateNodes();
	bool CanDuplicateNodes();

	void OnRenameNode();
	bool CanRenameNode();

	void OnAlignTop();
	void OnAlignMiddle();
	void OnAlignBottom();
	void OnAlignLeft();
	void OnAlignCenter();
	void OnAlignRight();

	void OnStraightenConnections();
	void OnDistributeNodesHorizontally();
	void OnDistributeNodesVertically();

	TArray<UClass*> FilterPlaceableObjectClasses(TArrayView<UClass* const> InObjectClasses);

protected:

	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<FUICommandList> BuiltInCommands;
};

