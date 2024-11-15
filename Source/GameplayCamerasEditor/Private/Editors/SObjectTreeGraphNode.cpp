// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/SObjectTreeGraphNode.h"

#include "EdGraph/EdGraph.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Editors/ObjectTreeGraphSchema.h"
#include "GraphEditorSettings.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "SObjectTreeGraphNode"

void SObjectTreeGraphNode::Construct(const FArguments& InArgs)
{
	GraphNode = InArgs._GraphNode;
	ObjectGraphNode = InArgs._GraphNode;

	SetCursor(EMouseCursor::CardinalCross);

	UpdateGraphNode();
}

void SObjectTreeGraphNode::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	SGraphNode::MoveTo(NewPosition, NodeFilter, bMarkDirty);

	if (ObjectGraphNode)
	{
		ObjectGraphNode->OnGraphNodeMoved(bMarkDirty);
	}
}

void SObjectTreeGraphNode::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	TArray<FArrayProperty*> ArrayProperties;
	ObjectGraphNode->GetArrayProperties(ArrayProperties);
	for (FArrayProperty* ArrayProperty : ArrayProperties)
	{
		TSharedRef<SWidget> AddPinButton = MakeAddArrayPropertyPinButton(ArrayProperty);

		FMargin AddPinPadding = Settings->GetOutputPinPadding();
		AddPinPadding.Top += 6.0f;

		OutputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Right)
		.Padding(AddPinPadding)
		[
			AddPinButton
		];
	}
}

TSharedRef<SWidget> SObjectTreeGraphNode::MakeAddArrayPropertyPinButton(FArrayProperty* ArrayProperty)
{
	TSharedRef<SWidget> ButtonContent = SNew(SHorizontalBox)
	+SHorizontalBox::Slot()
	.AutoWidth()
	.HAlign(HAlign_Left)
	[
		SNew(STextBlock)
		.Text(FText::Format(
					LOCTEXT("AddPropertyPinButtonLabelFmt", "Add {0} pin"),
					FText::FromName(ArrayProperty->GetFName())))
		.ColorAndOpacity(FLinearColor::White)
	]
	+SHorizontalBox::Slot()
	.AutoWidth()
	. VAlign(VAlign_Center)
	. Padding(7, 0, 0, 0)
	[
		SNew(SImage)
		.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
	];

	TSharedRef<SButton> AddPinButton = SNew(SButton)
	.ContentPadding(0.0f)
	.ButtonStyle( FAppStyle::Get(), "NoBorder" )
	.OnClicked(this, &SObjectTreeGraphNode::OnAddArrayPropertyPin, ArrayProperty)
	.IsEnabled(this, &SGraphNode::IsNodeEditable)
	.ToolTipText(FText::Format(
				LOCTEXT("AddPropertyPinButtonTooltipFmt", "Adds a new pin for the '{0}' property on this node"),
				FText::FromName(ArrayProperty->GetFName())))
	[
		ButtonContent
	];

	AddPinButton->SetCursor( EMouseCursor::Hand );

	return AddPinButton;
}

FReply SObjectTreeGraphNode::OnAddArrayPropertyPin(FArrayProperty* ArrayProperty)
{
	if (ObjectGraphNode)
	{
		UEdGraph* Graph = ObjectGraphNode->GetGraph();
		const UObjectTreeGraphSchema* Schema = Cast<const UObjectTreeGraphSchema>(Graph->GetSchema());

		UEdGraphPin* ArrayPin = ObjectGraphNode->GetPinForProperty(ArrayProperty);
		Schema->InsertArrayItemPin(ArrayPin, INDEX_NONE);

		UpdateGraphNode();
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

