// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SFindInObjectTreeGraph.h"

#include "Core/ObjectTreeGraphObject.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphSearch.h"
#include "Framework/Application/SlateApplication.h"
#include "Types/SlateEnums.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "SFindInObjectTreeGraph"

FFindInObjectTreeGraphResult::FFindInObjectTreeGraphResult(const FText& InCustomText)
	: CustomText(InCustomText)
{
}

FFindInObjectTreeGraphResult::FFindInObjectTreeGraphResult(
		TSharedPtr<FFindInObjectTreeGraphResult>& InParent, 
		const FFindInObjectTreeGraphSource& InSource, 
		UObject* InObject)
	: Parent(InParent)
	, WeakObject(InObject)
	, Source(InSource)
{
}

FFindInObjectTreeGraphResult::FFindInObjectTreeGraphResult(
		TSharedPtr<FFindInObjectTreeGraphResult>& InParent, 
		const FFindInObjectTreeGraphSource& InSource, 
		UObject* InObject, 
		FName InPropertyName)
	: Parent(InParent)
	, WeakObject(InObject)
	, PropertyName(InPropertyName)
	, Source(InSource)
{
}

TSharedRef<SWidget>	FFindInObjectTreeGraphResult::GetIcon() const
{
	FSlateColor IconColor = FSlateColor::UseForeground();
	const FSlateBrush* Brush = NULL;

	UObject* Object = WeakObject.Get();

	if (Object && !PropertyName.IsNone())
	{
		UClass* ObjectClass = Object->GetClass();
		FProperty* Property = ObjectClass->FindPropertyByName(PropertyName);
		if (Property->IsA<FArrayProperty>())
		{
			Brush = FAppStyle::GetBrush(TEXT("GraphEditor.ArrayPinIcon"));
		}
		else if (Property->IsA<FObjectProperty>())
		{
			Brush = FAppStyle::GetBrush(TEXT("GraphEditor.RefPinIcon"));
		}
		else
		{
			Brush = FAppStyle::GetBrush(TEXT("GraphEditor.PinIcon"));
		}
	}
	else if (Object)
	{
		Brush = FAppStyle::GetBrush(TEXT("GraphEditor.NodeGlyph"));
	}

	return SNew(SImage)
		.Image(Brush)
		.ColorAndOpacity(IconColor)
		.ToolTipText(GetCategory());
}

FText FFindInObjectTreeGraphResult::GetCategory() const
{
	if (WeakObject.IsValid())
	{
		if (PropertyName.IsNone())
		{
			return LOCTEXT("NodeCategory", "Node");
		}
		else
		{
			return LOCTEXT("PinCategory", "Pin");
		}
	}
	return FText::GetEmpty();
}

FText FFindInObjectTreeGraphResult::GetText() const
{
	UObject* Object = WeakObject.Get();
	if (Object)
	{
		if (PropertyName.IsNone())
		{
			const FText DisplayNameText = Source.GraphConfig->GetDisplayNameText(Object);
			return DisplayNameText;
		}
		else
		{
			UClass* ObjectClass = Object->GetClass();
			FProperty* Property = ObjectClass->FindPropertyByName(PropertyName);
			return Property->GetDisplayNameText();
		}
	}
	return CustomText;
}

FText FFindInObjectTreeGraphResult::GetCommentText() const
{
	if (IObjectTreeGraphObject* ObjectInterface = Cast<IObjectTreeGraphObject>(WeakObject.Get()))
	{
		return FText::FromString(ObjectInterface->GetGraphNodeCommentText(Source.GraphConfig->GraphName));
	}
	return FText::GetEmpty();
}

FReply FFindInObjectTreeGraphResult::OnClick(TSharedRef<SFindInObjectTreeGraph> FindInObjectTreeGraph)
{
	if (UObject* Object = WeakObject.Get())
	{
		FindInObjectTreeGraph->OnJumpToObjectRequested.ExecuteIfBound(Object, PropertyName);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void SFindInObjectTreeGraph::Construct(const FArguments& InArgs)
{
	OnGetRootObjectsToSearch = InArgs._OnGetRootObjectsToSearch;
	OnJumpToObjectRequested = InArgs._OnJumpToObjectRequested;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(LOCTEXT("SearchHint", "Search"))
				.OnTextChanged(this, &SFindInObjectTreeGraph::OnSearchTextChanged)
				.OnTextCommitted(this, &SFindInObjectTreeGraph::OnSearchTextCommitted)
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.Padding(0, 4, 0, 0)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Menu.Background"))
			[
				SAssignNew(ResultTreeView, SResultTreeView)
				.TreeItemsSource(&Results)
				.SelectionMode(ESelectionMode::Multi)
				.OnGenerateRow(this, &SFindInObjectTreeGraph::OnResultTreeViewGenerateRow)
				.OnGetChildren(this, &SFindInObjectTreeGraph::OnResultTreeViewGetChildren)
				.OnSelectionChanged(this, &SFindInObjectTreeGraph::OnResultTreeViewSelectionChanged)
				.OnMouseButtonDoubleClick(this, &SFindInObjectTreeGraph::OnResultTreeViewMouseButtonDoubleClick)
			]
		]
	];
}

void SFindInObjectTreeGraph::FocusSearchEditBox()
{
	FSlateApplication::Get().SetKeyboardFocus(SearchBox, EFocusCause::SetDirectly);
}

void SFindInObjectTreeGraph::OnSearchTextChanged(const FText& Text)
{
	SearchQuery = Text.ToString();
}

void SFindInObjectTreeGraph::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		StartSearch();
	}
}

TSharedRef<ITableRow> SFindInObjectTreeGraph::OnResultTreeViewGenerateRow(FResultPtr InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	const FText CommentText = InItem->GetCommentText();

	return SNew(STableRow<FResultPtr>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				InItem->GetIcon()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(2, 0)
			[
				SNew(STextBlock)
				.Text(InItem->GetText())
				.HighlightText(HighlightText)
				.ToolTipText(FText::Format(LOCTEXT("ResultToolTipFmt", "{0} : {1}"), InItem->GetCategory(), InItem->GetText()))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.Padding(2, 0)
			[
				SNew(STextBlock)
				.Text(CommentText.IsEmpty() 
						? FText::GetEmpty() 
						: FText::Format(LOCTEXT("NodeCommentFmt", "Node Comment: {0}"), CommentText))
				.HighlightText(HighlightText)
			]
		];
}

void SFindInObjectTreeGraph::OnResultTreeViewGetChildren(FResultPtr InItem, TArray<FResultPtr>& OutChildren)
{
	OutChildren += InItem->Children;
}

void SFindInObjectTreeGraph::OnResultTreeViewSelectionChanged(FResultPtr Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid())
	{
		Item->OnClick(SharedThis(this));
	}
}

void SFindInObjectTreeGraph::OnResultTreeViewMouseButtonDoubleClick(FResultPtr Item)
{
	if (Item.IsValid())
	{
		Item->OnClick(SharedThis(this));
	}
}

void SFindInObjectTreeGraph::StartSearch()
{
	TArray<FString> Tokens;
	if (SearchQuery.Contains("\"") && SearchQuery.ParseIntoArray(Tokens, TEXT("\""), true) > 0)
	{
		for (FString& Token : Tokens)
		{
			Token = Token.TrimQuotes();
		}
	}
	else
	{
		SearchQuery.ParseIntoArray(Tokens, TEXT(" "), true);
	}
	Tokens.RemoveAll([](const FString& Item) { return Item.IsEmpty(); });

	Results.Empty();
	HighlightText = FText::GetEmpty();
	TArray<FObjectTreeGraphSearchResult> SearchResults;
	if (Tokens.Num() > 0)
	{
		HighlightText = FText::FromString(SearchQuery);

		TArray<FFindInObjectTreeGraphSource> Sources;
		OnGetRootObjectsToSearch.ExecuteIfBound(Sources);

		FObjectTreeGraphSearch Searcher;
		for (const FFindInObjectTreeGraphSource& Source : Sources)
		{
			Searcher.AddRootObject(Source.RootObject, Source.GraphConfig);
		}

		Searcher.Search(Tokens, SearchResults);
	}

	// Convert simple flat search results to hierarchical results, where property results
	// are always under an object result, object results are always under a graph result, 
	// and so on. We do this by creating blank results if we need to, although it makes
	// the assumption that the flat results come ordered (e.g. an object result won't be
	// found after a property result for that object).
	{
		TMap<UObject*, FResultPtr> RootObjectToWidgetResult;
		TMap<UObject*, FResultPtr> ObjectToWidgetResult;
		for (const FObjectTreeGraphSearchResult& SearchResult : SearchResults)
		{
			FFindInObjectTreeGraphSource CurSource{ SearchResult.RootObject, SearchResult.GraphConfig };

			FResultPtr GraphResult;
			if (SearchResult.RootObject)
			{
				GraphResult = RootObjectToWidgetResult.FindRef(SearchResult.RootObject);
				if (!GraphResult)
				{
					const FText RootObjectDisplayText = SearchResult.GraphConfig->GetDisplayNameText(SearchResult.RootObject);
					const FText& GraphDisplayName = SearchResult.GraphConfig->GraphDisplayInfo.DisplayName;
					const FText GraphResultText = FText::Format(
							LOCTEXT("GraphResultFmt", "{0}: {1}"), { RootObjectDisplayText, GraphDisplayName });
					GraphResult = MakeShared<FFindInObjectTreeGraphResult>(GraphResultText);
					RootObjectToWidgetResult.Add(SearchResult.RootObject, GraphResult);
					Results.Add(GraphResult);
				}
			}

			FResultPtr ObjectResult;
			if (SearchResult.Object)
			{
				ObjectResult = ObjectToWidgetResult.FindRef(SearchResult.Object);
				if (!ObjectResult)
				{
					ensure(GraphResult);
					ObjectResult = MakeShared<FFindInObjectTreeGraphResult>(GraphResult, CurSource, SearchResult.Object);
					ObjectToWidgetResult.Add(SearchResult.Object, ObjectResult);
					GraphResult->Children.Add(ObjectResult);
				}
			}

			if (!SearchResult.PropertyName.IsNone())
			{
				ensure(ObjectResult);
				FResultPtr PropertyResult = MakeShared<FFindInObjectTreeGraphResult>(
						ObjectResult, CurSource, SearchResult.Object, SearchResult.PropertyName);
				ObjectResult->Children.Add(PropertyResult);
			}
		}
	}
	
	if (Results.IsEmpty())
	{
		Results.Add(MakeShared<FFindInObjectTreeGraphResult>(LOCTEXT("NoResults", "No results found")));
	}

	ResultTreeView->RequestTreeRefresh();
	for (FResultPtr Result : Results)
	{
		ResultTreeView->SetItemExpansion(Result, true);
	}
}

#undef LOCTEXT_NAMESPACE

