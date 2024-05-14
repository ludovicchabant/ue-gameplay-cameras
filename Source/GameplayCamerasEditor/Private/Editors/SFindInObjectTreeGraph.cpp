// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SFindInObjectTreeGraph.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
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

FFindInObjectTreeGraphResult::FFindInObjectTreeGraphResult(TSharedPtr<FFindInObjectTreeGraphResult>& InParent, UEdGraphNode* InGraphNode)
	: Parent(InParent)
	, GraphNode(InGraphNode)
{
}

FFindInObjectTreeGraphResult::FFindInObjectTreeGraphResult(TSharedPtr<FFindInObjectTreeGraphResult>& InParent, UEdGraphPin* InGraphPin)
	: Parent(InParent)
	, GraphPin(InGraphPin)
{
}

TSharedRef<SWidget>	FFindInObjectTreeGraphResult::GetIcon() const
{
	FSlateColor IconColor = FSlateColor::UseForeground();
	const FSlateBrush* Brush = NULL;

	if (UEdGraphPin* ResolvedPin = GraphPin.Get())
	{
		if (ResolvedPin->PinType.IsArray())
		{
			Brush = FAppStyle::GetBrush(TEXT("GraphEditor.ArrayPinIcon"));
		}
		else if (ResolvedPin->PinType.bIsReference)
		{
			Brush = FAppStyle::GetBrush(TEXT("GraphEditor.RefPinIcon"));
		}
		else
		{
			Brush = FAppStyle::GetBrush(TEXT("GraphEditor.PinIcon"));
		}

		const UEdGraphSchema* Schema = ResolvedPin->GetSchema();
		IconColor = Schema->GetPinTypeColor(ResolvedPin->PinType);
	}
	else if (GraphNode.IsValid())
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
	if (GraphNode.IsValid())
	{
		return LOCTEXT("NodeCategory", "Node");
	}
	else if (GraphPin.Get())
	{
		return LOCTEXT("PinCategory", "Pin");
	}
	return FText::GetEmpty();
}

FText FFindInObjectTreeGraphResult::GetText() const
{
	if (UEdGraphNode* ResolvedNode = GraphNode.Get())
	{
		const FText NodeFullTitle = ResolvedNode->GetNodeTitle(ENodeTitleType::FullTitle);
		const FText NodeListViewTitle = ResolvedNode->GetNodeTitle(ENodeTitleType::ListView);
		if (NodeFullTitle.EqualToCaseIgnored(NodeListViewTitle))
		{
			return NodeFullTitle;
		}
		return FText::Format(LOCTEXT("NodeResultFmt", "{0} - {1}"), NodeListViewTitle, NodeFullTitle);
	}
	else if (UEdGraphPin* ResolvedPin = GraphPin.Get())
	{
		const UEdGraphSchema* GraphSchema = ResolvedPin->GetSchema();
		return GraphSchema->GetPinDisplayName(ResolvedPin);
	}
	return CustomText;
}

FText FFindInObjectTreeGraphResult::GetCommentText() const
{
	if (UEdGraphNode* ResolvedNode = GraphNode.Get())
	{
		return FText::FromString(ResolvedNode->NodeComment);
	}
	return FText::GetEmpty();
}

FReply FFindInObjectTreeGraphResult::OnClick(TSharedRef<SFindInObjectTreeGraph> FindInObjectTreeGraph)
{
	if (UEdGraphNode* ResolvedNode = GraphNode.Get())
	{
		FindInObjectTreeGraph->OnJumpToNodeRequested.ExecuteIfBound(ResolvedNode);
		return FReply::Handled();
	}
	else if (UEdGraphPin* ResolvedPin = GraphPin.Get())
	{
		if (FindInObjectTreeGraph->OnJumpToPinRequested.IsBound())
		{
			FindInObjectTreeGraph->OnJumpToPinRequested.Execute(ResolvedPin);
		}
		else
		{
			FindInObjectTreeGraph->OnJumpToNodeRequested.ExecuteIfBound(ResolvedPin->GetOwningNode());
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void SFindInObjectTreeGraph::Construct(const FArguments& InArgs)
{
	GraphsToSearch = InArgs._GraphsToSearch;

	OnJumpToNodeRequested = InArgs._OnJumpToNodeRequested;
	OnJumpToPinRequested = InArgs._OnJumpToPinRequested;

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
	if (Tokens.Num() > 0)
	{
		HighlightText = FText::FromString(SearchQuery);
		for (UEdGraph* Graph : GraphsToSearch)
		{
			MatchTokensInGraph(Graph, Tokens);
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

void SFindInObjectTreeGraph::MatchTokensInGraph(UEdGraph* Graph, TArrayView<FString> Tokens)
{
	FResultPtr GraphResult(new FFindInObjectTreeGraphResult(FText::FromName(Graph->GetFName())));

	const UEdGraphSchema* GraphSchema = Graph->GetSchema();

	for (UEdGraphNode* Node : Graph->Nodes)
	{
		const bool bNodeMatches = GraphNodeMatchesSearchTokens(GraphSchema, Node, Tokens);

		TArray<UEdGraphPin*> MatchingPins;
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (GraphPinMatchesSearchTokens(GraphSchema, Pin, Tokens))
			{
				MatchingPins.Add(Pin);
			}
		}

		if (bNodeMatches || MatchingPins.Num() > 0)
		{
			FResultPtr NodeResult(new FFindInObjectTreeGraphResult(GraphResult, Node));
			Results.Add(NodeResult);

			for (UEdGraphPin* Pin : MatchingPins)
			{
				FResultPtr PinResult(new FFindInObjectTreeGraphResult(NodeResult, Pin));
			}
		}
	}

	for (UEdGraph* SubGraph : Graph->SubGraphs)
	{
		MatchTokensInGraph(SubGraph, Tokens);
	}
}

bool SFindInObjectTreeGraph::GraphNodeMatchesSearchTokens(const UEdGraphSchema* GraphSchema, UEdGraphNode* GraphNode, TArrayView<FString> Tokens)
{
	const FString NodeFullTitle = GraphNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString();
	if (StringMatchesSearchTokens(NodeFullTitle, Tokens))
	{
		return true;
	}

	const FString NodeListViewTitle = GraphNode->GetNodeTitle(ENodeTitleType::ListView).ToString();
	if (StringMatchesSearchTokens(NodeListViewTitle, Tokens))
	{
		return true;
	}

	if (StringMatchesSearchTokens(GraphNode->NodeComment, Tokens))
	{
		return true;
	}

	return false;
}

bool SFindInObjectTreeGraph::GraphPinMatchesSearchTokens(const UEdGraphSchema* GraphSchema, UEdGraphPin* GraphPin, TArrayView<FString> Tokens)
{
	const FString PinDisplayName = GraphSchema->GetPinDisplayName(GraphPin).ToString();
	if (StringMatchesSearchTokens(PinDisplayName, Tokens))
	{
		return true;
	}

	return false;
}

bool SFindInObjectTreeGraph::StringMatchesSearchTokens(const FString& ComparisonString, TArrayView<FString> Tokens)
{
	if (ComparisonString.IsEmpty())
	{
		return false;
	}

	for (const FString& Token : Tokens)
	{
		if (!ComparisonString.Contains(Token))
		{
			return false;
		}
	}
	return true;
}

#undef LOCTEXT_NAMESPACE

