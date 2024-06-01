// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "EdGraph/EdGraphPin.h"
#include "UObject/WeakObjectPtr.h"
#include "Templates/SharedPointerFwd.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STreeView.h"

class UEdGraph;
class UEdGraphNode;
class UEdGraphSchema;
class SSearchBox;

class SFindInObjectTreeGraph;

/** 
 * Structure for a search result inside an object tree graph.
 */
struct FFindInObjectTreeGraphResult
{
public:

	/** Parent result. */
	TWeakPtr<FFindInObjectTreeGraphResult> Parent;
	/** Children results. */
	TArray<TSharedPtr<FFindInObjectTreeGraphResult>> Children;

	/** Custom text for this result. */
	FText CustomText;
	/** The graph node that this result refers to. */
	TWeakObjectPtr<UEdGraphNode> GraphNode;
	/** The graph pin that this result refers to. */
	FEdGraphPinReference GraphPin;

public:

	/** Creates a new result with a custom text. */
	FFindInObjectTreeGraphResult(const FText& InCustomText);
	/** Creates a new result referring to a graph node, under a parent result. */
	FFindInObjectTreeGraphResult(TSharedPtr<FFindInObjectTreeGraphResult>& InParent, UEdGraphNode* InGraphNode);
	/** Creates a new result referring to a graph pin, under a parent result. */
	FFindInObjectTreeGraphResult(TSharedPtr<FFindInObjectTreeGraphResult>& InParent, UEdGraphPin* InGraphPin);

	/** Gets the icon for this result. */
	TSharedRef<SWidget>	GetIcon() const;
	/** Gets the category for this result. */
	FText GetCategory() const;
	/** Gets the display text for this result. */
	FText GetText() const;
	/** Gets the comment text for this result. */
	FText GetCommentText() const;

	/** Go to the graph node, pin, etc. */
	FReply OnClick(TSharedRef<SFindInObjectTreeGraph> FindInObjectTreeGraph);
};

/**
 * A search panel to find things in one or more object tree graphs.
 */
class SFindInObjectTreeGraph : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_OneParam(FOnJumpToNodeRequested, UEdGraphNode*);
	DECLARE_DELEGATE_OneParam(FOnJumpToPinRequested, UEdGraphPin*);

	SLATE_BEGIN_ARGS(SFindInObjectTreeGraph)
	{}
		/** The graphs to search. */
		SLATE_ARGUMENT(TArray<UEdGraph*>, GraphsToSearch)
		/** The callback to invoke when a search result wants to focus a node. */
		SLATE_EVENT(FOnJumpToNodeRequested, OnJumpToNodeRequested)
		/** The callback to invoke when a search result wants to focus a pin. */
		SLATE_EVENT(FOnJumpToPinRequested, OnJumpToPinRequested)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void FocusSearchEditBox();

protected:

	typedef TSharedPtr<FFindInObjectTreeGraphResult> FResultPtr;
	typedef STreeView<FResultPtr> SResultTreeView;

	void OnSearchTextChanged(const FText& Text);
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);

	TSharedRef<ITableRow> OnResultTreeViewGenerateRow(FResultPtr InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnResultTreeViewGetChildren(FResultPtr InItem, TArray<FResultPtr>& OutChildren);
	void OnResultTreeViewSelectionChanged(FResultPtr Item, ESelectInfo::Type SelectInfo);
	void OnResultTreeViewMouseButtonDoubleClick(FResultPtr Item);

protected:

	void StartSearch();
	void MatchTokensInGraph(UEdGraph* Graph, TArrayView<FString> Tokens);
	bool GraphNodeMatchesSearchTokens(const UEdGraphSchema* GraphSchema, UEdGraphNode* GraphNode, TArrayView<FString> Tokens);
	bool GraphPinMatchesSearchTokens(const UEdGraphSchema* GraphSchema, UEdGraphPin* GraphPin, TArrayView<FString> Tokens);
	bool StringMatchesSearchTokens(const FString& ComparisonString, TArrayView<FString> Tokens);

protected:

	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SResultTreeView> ResultTreeView;

	FString SearchQuery;
	TArray<FResultPtr> Results;

	FText HighlightText;

	TArray<UEdGraph*> GraphsToSearch;

	FOnJumpToNodeRequested OnJumpToNodeRequested;
	FOnJumpToPinRequested OnJumpToPinRequested;

	friend struct FFindInObjectTreeGraphResult;
};

