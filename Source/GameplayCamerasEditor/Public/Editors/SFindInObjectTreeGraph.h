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

struct FFindInObjectTreeGraphResult
{
public:

	TWeakPtr<FFindInObjectTreeGraphResult> Parent;
	TArray<TSharedPtr<FFindInObjectTreeGraphResult>> Children;

	FText CustomText;
	TWeakObjectPtr<UEdGraphNode> GraphNode;
	FEdGraphPinReference GraphPin;

public:

	FFindInObjectTreeGraphResult(const FText& InCustomText);
	FFindInObjectTreeGraphResult(TSharedPtr<FFindInObjectTreeGraphResult>& InParent, UEdGraphNode* InGraphNode);
	FFindInObjectTreeGraphResult(TSharedPtr<FFindInObjectTreeGraphResult>& InParent, UEdGraphPin* InGraphPin);

	TSharedRef<SWidget>	GetIcon() const;
	FText GetCategory() const;
	FText GetText() const;
	FText GetCommentText() const;

	FReply OnClick(TSharedRef<SFindInObjectTreeGraph> FindInObjectTreeGraph);
};

class SFindInObjectTreeGraph : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_OneParam(FOnJumpToNodeRequested, UEdGraphNode*);
	DECLARE_DELEGATE_OneParam(FOnJumpToPinRequested, UEdGraphPin*);

	SLATE_BEGIN_ARGS(SFindInObjectTreeGraph)
	{}
		SLATE_ARGUMENT(TArray<UEdGraph*>, GraphsToSearch)
		SLATE_ARGUMENT(FOnJumpToNodeRequested, OnJumpToNodeRequested)
		SLATE_ARGUMENT(FOnJumpToPinRequested, OnJumpToPinRequested)
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

