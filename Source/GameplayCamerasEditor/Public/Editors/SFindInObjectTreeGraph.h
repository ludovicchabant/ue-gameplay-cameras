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

class SFindInObjectTreeGraph;
class SSearchBox;
class UEdGraph;
class UEdGraphNode;
class UEdGraphSchema;
struct FObjectTreeGraphConfig;

/**
 * Struct describing one source of possible results for a search.
 */
struct FFindInObjectTreeGraphSource
{
	/** The root object a result was found in. */
	UObject* RootObject = nullptr;
	/** The config for the object tree graph a result was found in. */
	const FObjectTreeGraphConfig* GraphConfig = nullptr;
};

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
	/** The object that this result refers to. */
	TWeakObjectPtr<> WeakObject;
	/** The property name that this result refers to. */
	FName PropertyName;

public:

	/** Creates a new result with a custom text. */
	FFindInObjectTreeGraphResult(const FText& InCustomText);
	/** Creates a new result referring to an object, under a parent result. */
	FFindInObjectTreeGraphResult(
			TSharedPtr<FFindInObjectTreeGraphResult>& InParent, 
			const FFindInObjectTreeGraphSource& InSource, 
			UObject* InObject);
	/** Creates a new result referring to an object's property, under a parent result. */
	FFindInObjectTreeGraphResult(
			TSharedPtr<FFindInObjectTreeGraphResult>& InParent, 
			const FFindInObjectTreeGraphSource& InSource, 
			UObject* InObject, 
			FName InPropertyName);

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

private:

	FFindInObjectTreeGraphSource Source;
};

/**
 * A search panel to find things in one or more object tree graphs.
 */
class SFindInObjectTreeGraph : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_OneParam(FOnGetRootObjectsToSearch, TArray<FFindInObjectTreeGraphSource>&);
	DECLARE_DELEGATE_TwoParams(FOnJumpToObjectRequested, UObject*, FName);

	SLATE_BEGIN_ARGS(SFindInObjectTreeGraph)
	{}
		/** The callback to get the graphs to search. */
		SLATE_EVENT(FOnGetRootObjectsToSearch, OnGetRootObjectsToSearch)
		/** The callback to invoke when a search result wants to focus an object node or one of its pins. */
		SLATE_EVENT(FOnJumpToObjectRequested, OnJumpToObjectRequested)
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

protected:

	FOnGetRootObjectsToSearch OnGetRootObjectsToSearch;
	FOnJumpToObjectRequested OnJumpToObjectRequested;

	TSharedPtr<SSearchBox> SearchBox;
	TSharedPtr<SResultTreeView> ResultTreeView;

	FString SearchQuery;
	TArray<FResultPtr> Results;

	FText HighlightText;

	friend struct FFindInObjectTreeGraphResult;
};

