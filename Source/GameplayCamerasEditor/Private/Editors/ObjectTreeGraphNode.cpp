// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeGraphNode.h"

#include "Core/ObjectTreeGraphObject.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphSchema.h"
#include "Editors/SObjectTreeGraphNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "ObjectTreeGraphNode"

UObjectTreeGraphNode::UObjectTreeGraphNode(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	bCanRenameNode = true;
}

void UObjectTreeGraphNode::Initialize(UObject* InObject)
{
	ensure(InObject);

	WeakObject = InObject;

	const FNodeContext NodeContext = GetNodeContext();
	IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(InObject);
	if (GraphObject && GraphObject->HasSupportFlags(NodeContext.GraphConfig.GraphName, EObjectTreeGraphObjectSupportFlags::CommentText))
	{
		NodeComment = GraphObject->GetGraphNodeCommentText(NodeContext.GraphConfig.GraphName);
	}
}

FText UObjectTreeGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UObject* Object = WeakObject.Get();
	if (Object)
	{
		const FNodeContext NodeContext = GetNodeContext();
		return NodeContext.GraphConfig.GetDisplayNameText(Object);
	}
	return FText::GetEmpty();
}

TSharedPtr<SGraphNode> UObjectTreeGraphNode::CreateVisualWidget()
{
	return SNew(SObjectTreeGraphNode).GraphNode(this);
}

FLinearColor UObjectTreeGraphNode::GetNodeTitleColor() const
{
	const FNodeContext NodeContext = GetNodeContext();
	return NodeContext.ObjectClassConfig.NodeTitleColor().Get(NodeContext.GraphConfig.DefaultGraphNodeTitleColor);
}

FLinearColor UObjectTreeGraphNode::GetNodeBodyTintColor() const
{
	const FNodeContext NodeContext = GetNodeContext();
	return NodeContext.ObjectClassConfig.NodeBodyTintColor().Get(NodeContext.GraphConfig.DefaultGraphNodeBodyTintColor);
}

FText UObjectTreeGraphNode::GetTooltipText() const
{
	UObject* Object = WeakObject.Get();
	if (Object)
	{
		return Object->GetClass()->GetToolTipText();
	}
	return FText::GetEmpty();
}

void UObjectTreeGraphNode::AllocateDefaultPins()
{
	UObject* Object = WeakObject.Get();
	if (!ensure(Object))
	{
		return;
	}

	const FNodeContext NodeContext = GetNodeContext();
	const FObjectTreeGraphConfig& OuterGraphConfig = NodeContext.GraphConfig;
	const FObjectTreeGraphClassConfig& ObjectClassConfig = NodeContext.ObjectClassConfig;

	if (ObjectClassConfig.HasSelfPin())
	{
		FEdGraphPinType SelfPinType;
		SelfPinType.PinCategory = UObjectTreeGraphSchema::PC_Self;
		const FName& SelfPinName = ObjectClassConfig.SelfPinName();
		UEdGraphPin* SelfPin = CreatePin(OuterGraphConfig.GetSelfPinDirection(NodeContext.ObjectClass), SelfPinType, SelfPinName);
		SelfPin->PinFriendlyName = ObjectClassConfig.SelfPinFriendlyName();
	}

	for (TFieldIterator<FProperty> PropertyIt(NodeContext.ObjectClass); PropertyIt; ++PropertyIt)
	{
		const FName PropertyName = PropertyIt->GetFName();

		const EEdGraphPinDirection PinDirection = OuterGraphConfig.GetPropertyPinDirection(NodeContext.ObjectClass, PropertyName);

		FEdGraphPinType ChildPinType;
		ChildPinType.PinCategory = UObjectTreeGraphSchema::PC_Property;

		if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(*PropertyIt))
		{
			if (!OuterGraphConfig.IsConnectable(ObjectProperty))
			{
				continue;
			}

			ChildPinType.PinSubCategory = UObjectTreeGraphSchema::PSC_ObjectProperty;
			UEdGraphPin* PropertyPin = CreatePin(PinDirection, ChildPinType, PropertyName);

			PropertyPin->PinFriendlyName = FText::FromName(PropertyName);
			PropertyPin->PinToolTip = ObjectProperty->PropertyClass->GetName();
		}
		else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(*PropertyIt))
		{
			if (!OuterGraphConfig.IsConnectable(ArrayProperty))
			{
				continue;
			}

			FObjectProperty* InnerProperty = CastFieldChecked<FObjectProperty>(ArrayProperty->Inner);

			ChildPinType.PinSubCategory = UObjectTreeGraphSchema::PSC_ArrayProperty;
			ChildPinType.ContainerType = EPinContainerType::Array;
			UEdGraphPin* ArrayPin = CreatePin(PinDirection, ChildPinType, PropertyName);

			ArrayPin->PinFriendlyName = FText::FromName(PropertyName);
			ArrayPin->PinToolTip = InnerProperty->PropertyClass->GetName();
			ArrayPin->bHidden = true;  // Always hidden, we only ever show the sub-pins.

			CreateNewItemPin(ArrayPin);
		}
	}
}

void UObjectTreeGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	UEdGraphPin* SelfPin = GetSelfPin();
	if (FromPin && SelfPin)
	{
		const UObjectTreeGraphSchema* GraphSchema = CastChecked<UObjectTreeGraphSchema>(GetSchema());
		GraphSchema->TryCreateConnection(FromPin, SelfPin);
	}

	Super::AutowireNewNode(FromPin);
}

void UObjectTreeGraphNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
}

void UObjectTreeGraphNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();
}

void UObjectTreeGraphNode::OnPinRemoved(UEdGraphPin* InRemovedPin)
{
	Super::OnPinRemoved(InRemovedPin);
	RefreshArrayPropertyPinNames();
}

void UObjectTreeGraphNode::CreateNewItemPin(FArrayProperty& InArrayProperty)
{
	UEdGraphPin** ParentArrayPinPtr = Pins.FindByPredicate([&InArrayProperty](UEdGraphPin* Item)
			{
				return Item->GetFName() == InArrayProperty.GetFName();
			});
	if (ensure(ParentArrayPinPtr))
	{
		CreateNewItemPin(*ParentArrayPinPtr);
	}
}

void UObjectTreeGraphNode::CreateNewItemPin(UEdGraphPin* InParentArrayPin)
{
	const FNodeContext NodeContext = GetNodeContext();

	const FName PropertyName = InParentArrayPin->GetFName();
	const int32 NewIndex = InParentArrayPin->SubPins.Num();

	FEdGraphPinType ChildPinType;
	ChildPinType.PinCategory = UObjectTreeGraphSchema::PC_Property;
	ChildPinType.PinSubCategory = UObjectTreeGraphSchema::PSC_ArrayPropertyItem;

	const EEdGraphPinDirection PinDirection = NodeContext.GraphConfig.GetPropertyPinDirection(NodeContext.ObjectClass, PropertyName);

	InParentArrayPin->Modify();

	FName ChildPinName = PropertyName;
	ChildPinName.SetNumber(NewIndex);
	UEdGraphPin* ChildPin = CreatePin(PinDirection, ChildPinType, ChildPinName);
	ChildPin->PinFriendlyName = FText::Format(LOCTEXT("ArrayPinFriendlyNameFmt", "{0} {1}"), FText::FromName(PropertyName), NewIndex);

	ChildPin->ParentPin = InParentArrayPin;
	InParentArrayPin->SubPins.Add(ChildPin);

	// Always re-insert the child pin so that all child pins are just after
	// the parent array pin.
	const int32 ParentPinIndex = Pins.Find(InParentArrayPin);
	if (ensure(ParentPinIndex >= 0))
	{
		const int32 ChildPinIndex = ParentPinIndex + InParentArrayPin->SubPins.Num();
		Pins.Pop(EAllowShrinking::No);
		Pins.Insert(ChildPin, ChildPinIndex);
	}
}

void UObjectTreeGraphNode::RemoveItemPin(UEdGraphPin* InItemPin)
{
	if (ensure(InItemPin && 
				InItemPin->ParentPin &&
				InItemPin->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property &&
				InItemPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayPropertyItem))
	{
		InItemPin->ParentPin->Modify();

		// Don't call RemovePin() because that also removes the parent pin.
		// We just want to remove the child pin.
		const int32 NumPinRemoved = Pins.Remove(InItemPin);
		ensure(NumPinRemoved == 1);
		const int32 NumSubPinRemoved = InItemPin->ParentPin->SubPins.Remove(InItemPin);
		ensure(NumSubPinRemoved == 1);

		OnPinRemoved(InItemPin);

		InItemPin->MarkAsGarbage();
	}
}

void UObjectTreeGraphNode::RefreshArrayPropertyPinNames()
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && 
				Pin->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property &&
				Pin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayProperty)
		{
			const FName PropertyName = Pin->GetFName();
			for (int32 PinIndex = 0; PinIndex < Pin->SubPins.Num(); ++PinIndex)
			{
				UEdGraphPin* ChildPin = Pin->SubPins[PinIndex];
				ChildPin->PinName.SetNumber(PinIndex);
				ChildPin->PinFriendlyName = FText::Format(LOCTEXT("ArrayPinFriendlyNameFmt", "{0} {1}"), FText::FromName(PropertyName), PinIndex);
			}
		}
	}
}

void UObjectTreeGraphNode::GetAllConnectableProperties(TArray<FProperty*>& OutProperties) const
{
	UObject* Object = WeakObject.Get();
	if (!ensure(Object))
	{
		return;
	}

	UClass* ObjectClass = Object->GetClass();
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin 
				&& Pin->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property
				&& (Pin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ObjectProperty || Pin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayProperty))
		{
			FProperty* Property = ObjectClass->FindPropertyByName(Pin->GetFName());
			if (ensure(Property))
			{
				OutProperties.Add(Property);
			}
		}
	}
}

UEdGraphPin* UObjectTreeGraphNode::GetSelfPin() const
{
	UEdGraphPin* const* SelfPinPtr = Pins.FindByPredicate(
			[](UEdGraphPin* Item) { return Item->PinType.PinCategory == UObjectTreeGraphSchema::PC_Self; });
	if (SelfPinPtr)
	{
		return *SelfPinPtr;
	}
	return nullptr;
}

void UObjectTreeGraphNode::OverrideSelfPinDirection(EEdGraphPinDirection Direction)
{
	Modify();

	bOverrideSelfPinDirection = true;
	SelfPinDirectionOverride = Direction;

	if (UEdGraphPin* SelfPin = GetSelfPin())
	{
		SelfPin->Direction = Direction;
		GetGraph()->NotifyNodeChanged(this);
	}
}

UEdGraphPin* UObjectTreeGraphNode::GetPinForProperty(FObjectProperty* InProperty) const
{
	UEdGraphPin* const* FoundItem = Pins.FindByPredicate(
			[InProperty](UEdGraphPin* Item)
			{ 
				return Item->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property &&
					Item->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ObjectProperty &&
					Item->GetFName() == InProperty->GetFName(); 
			});
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

UEdGraphPin* UObjectTreeGraphNode::GetPinForProperty(FArrayProperty* InProperty, int32 Index) const
{
	UEdGraphPin* const* FoundItem = Pins.FindByPredicate(
			[InProperty](UEdGraphPin* Item)
			{ 
				return Item->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property &&
					Item->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayProperty &&
					Item->GetFName() == InProperty->GetFName(); 
			});
	if (FoundItem && ensure((*FoundItem)->SubPins.IsValidIndex(Index)))
	{
		return (*FoundItem)->SubPins[Index];
	}
	return nullptr;
}

UEdGraphPin* UObjectTreeGraphNode::GetPinForPropertyNewItem(FArrayProperty* InProperty, bool bCreateNew)
{
	UEdGraphPin* const* FoundItem = Pins.FindByPredicate(
			[InProperty](UEdGraphPin* Item)
			{ 
				return Item->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property &&
					Item->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayProperty &&
					Item->GetFName() == InProperty->GetFName(); 
			});
	if (FoundItem)
	{
		ensure((*FoundItem)->SubPins.Num() > 0 && (*FoundItem)->SubPins.Last()->LinkedTo.IsEmpty());
		UEdGraphPin* NewItemPin = (*FoundItem)->SubPins.Last();
		if (bCreateNew)
		{
			CreateNewItemPin(*FoundItem);
		}
		return NewItemPin;
	}
	return nullptr;
}

FProperty* UObjectTreeGraphNode::GetPropertyForPin(const UEdGraphPin* InPin) const
{
	UObject* Object = WeakObject.Get();
	if (!ensure(Object))
	{
		return nullptr;
	}
	if (InPin->PinType.PinCategory != UObjectTreeGraphSchema::PC_Property)
	{
		return nullptr;
	}

	UClass* ObjectClass = Object->GetClass();

	if (InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ObjectProperty)
	{
		return ObjectClass->FindPropertyByName(InPin->GetFName());
	}
	else if (InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayProperty)
	{
		return ObjectClass->FindPropertyByName(InPin->GetFName());
	}
	else if (InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayPropertyItem)
	{
		UEdGraphPin* ParentArrayPin = InPin->ParentPin;
		check(ParentArrayPin);
		return ObjectClass->FindPropertyByName(ParentArrayPin->GetFName());
	}

	return nullptr;
}

UClass* UObjectTreeGraphNode::GetConnectedObjectClassForPin(const UEdGraphPin* InPin) const
{
	UObject* Object = WeakObject.Get();
	if (!ensure(Object))
	{
		return nullptr;
	}
	if (InPin->PinType.PinCategory != UObjectTreeGraphSchema::PC_Property)
	{
		return nullptr;
	}

	UClass* ObjectClass = Object->GetClass();

	if (InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ObjectProperty)
	{
		FObjectProperty* ObjectProperty = CastFieldChecked<FObjectProperty>(ObjectClass->FindPropertyByName(InPin->GetFName()));
		return ObjectProperty->PropertyClass;
	}
	else if (InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayProperty)
	{
		FArrayProperty* ArrayProperty = CastFieldChecked<FArrayProperty>(ObjectClass->FindPropertyByName(InPin->GetFName()));
		FObjectProperty* InnerProperty = CastFieldChecked<FObjectProperty>(ArrayProperty->Inner);
		return InnerProperty->PropertyClass;
	}
	else if (InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayPropertyItem)
	{
		UEdGraphPin* ParentArrayPin = InPin->ParentPin;
		check(ParentArrayPin);
		FArrayProperty* ArrayProperty = CastFieldChecked<FArrayProperty>(ObjectClass->FindPropertyByName(ParentArrayPin->GetFName()));
		FObjectProperty* InnerProperty = CastFieldChecked<FObjectProperty>(ArrayProperty->Inner);
		return InnerProperty->PropertyClass;
	}

	return nullptr;
}

int32 UObjectTreeGraphNode::GetIndexOfArrayPin(const UEdGraphPin* InPin) const
{
	if (!ensure(InPin &&
				InPin->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property &&
				InPin->PinType.PinSubCategory == UObjectTreeGraphSchema::PSC_ArrayPropertyItem))
	{
		return INDEX_NONE;
	}

	UEdGraphPin* ParentArrayPin = InPin->ParentPin;
	check(ParentArrayPin);
	return ParentArrayPin->SubPins.Find(const_cast<UEdGraphPin*>(InPin));
}

void UObjectTreeGraphNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(GetObject());
	if (GraphObject)
	{
		const FNodeContext NodeContext = GetNodeContext();
		GraphObject->GetGraphNodePosition(NodeContext.GraphConfig.GraphName, NodePosX, NodePosY);
	}
}

void UObjectTreeGraphNode::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	FToolMenuInsert MenuPosition = FToolMenuInsert(NAME_None, EToolMenuInsertType::First);

	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();
	const FGenericCommands& GenericCommands = FGenericCommands::Get();

	// Common actions.
	{
		FToolMenuSection& NodeSection = Menu->AddSection(
				"ObjectTreeGraphNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"), MenuPosition);

		NodeSection.AddMenuEntry(GraphEditorCommands.BreakNodeLinks);
	}

	// General actions.
	{
		FToolMenuSection& Section = Menu->AddSection(
				"ObjectTreeGraphNodeGenericActions", LOCTEXT("GenericActionsMenuHeader", "General"));

		Section.AddMenuEntry(GenericCommands.Delete);
		Section.AddMenuEntry(GenericCommands.Cut);
		Section.AddMenuEntry(GenericCommands.Copy);
		Section.AddMenuEntry(GenericCommands.Duplicate);
	}

	// Graph organization.
	{
		FToolMenuSection& Section = Menu->AddSection(
				"ObjectTreeGraphOrganizationActions", LOCTEXT("OrganizationActionsMenuHeader", "Organization"));

		Section.AddSubMenu(
				"Alignment",
				LOCTEXT("AlignmentHeader", "Alignment"),
				FText(),
				FNewToolMenuDelegate::CreateLambda([&GraphEditorCommands](UToolMenu* InMenu)
					{
						FToolMenuSection& SubMenuSection = InMenu->AddSection(
								"ObjectTreeGraphAlignmentActions", LOCTEXT("AlignmentHeader", "Alignment"));
						SubMenuSection.AddMenuEntry(GraphEditorCommands.AlignNodesTop);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.AlignNodesMiddle);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.AlignNodesBottom);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.AlignNodesLeft);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.AlignNodesCenter);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.AlignNodesRight);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.StraightenConnections);
					}));

		Section.AddSubMenu(
				"Distribution",
				LOCTEXT("DistributionHeader", "Distribution"),
				FText(),
				FNewToolMenuDelegate::CreateLambda([&GraphEditorCommands](UToolMenu* InMenu)
					{
						FToolMenuSection& SubMenuSection = InMenu->AddSection(
								"ObjectTreeGraphDistributionActions", LOCTEXT("DistributionHeader", "Distribution"));
						SubMenuSection.AddMenuEntry(GraphEditorCommands.DistributeNodesHorizontally);
						SubMenuSection.AddMenuEntry(GraphEditorCommands.DistributeNodesVertically);
					}));
	}
}

bool UObjectTreeGraphNode::GetCanRenameNode() const
{
	UObject* Object = WeakObject.Get();
	const FNodeContext NodeContext = GetNodeContext();
	IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(Object);
	return GraphObject && GraphObject->HasSupportFlags(NodeContext.GraphConfig.GraphName, EObjectTreeGraphObjectSupportFlags::CustomRename);
}

void UObjectTreeGraphNode::OnRenameNode(const FString& NewName)
{
	Super::OnRenameNode(NewName);

	UObject* Object = WeakObject.Get();
	if (IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(Object))
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameNode", "Rename Node"));

		const FNodeContext NodeContext = GetNodeContext();
		GraphObject->OnRenameGraphNode(NodeContext.GraphConfig.GraphName, NewName);
	}
}

bool UObjectTreeGraphNode::CanDuplicateNode() const
{
	const FObjectTreeGraphClassConfig& ObjectClassConfig = GetObjectClassConfig();
	if (!ObjectClassConfig.CanCreateNew())  // If it can't be created, it shouldn't be worked around by copy/pasting
	{
		return false;
	}

	return Super::CanUserDeleteNode();
}

bool UObjectTreeGraphNode::CanUserDeleteNode() const
{
	const FObjectTreeGraphClassConfig& ObjectClassConfig = GetObjectClassConfig();
	if (!ObjectClassConfig.CanDelete())
	{
		return false;
	}

	return Super::CanUserDeleteNode();
}

bool UObjectTreeGraphNode::SupportsCommentBubble() const
{
	UObject* Object = WeakObject.Get();
	const FNodeContext NodeContext = GetNodeContext();
	IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(Object);
	return GraphObject && GraphObject->HasSupportFlags(NodeContext.GraphConfig.GraphName, EObjectTreeGraphObjectSupportFlags::CommentText);
}

void UObjectTreeGraphNode::OnUpdateCommentText(const FString& NewComment)
{
	Super::OnUpdateCommentText(NewComment);

	UObject* Object = WeakObject.Get();
	IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(Object);
	if (GraphObject)
	{
		const FScopedTransaction Transaction(LOCTEXT("UpdateNodeComment", "Update Node Comment"));

		const FNodeContext NodeContext = GetNodeContext();
		GraphObject->OnUpdateGraphNodeCommentText(NodeContext.GraphConfig.GraphName, NewComment);
	}
}

void UObjectTreeGraphNode::OnGraphNodeMoved(bool bMarkDirty)
{
	UObject* Object = WeakObject.Get();
	IObjectTreeGraphObject* GraphObject = Cast<IObjectTreeGraphObject>(Object);
	if (GraphObject)
	{
		const FNodeContext NodeContext = GetNodeContext();
		GraphObject->OnGraphNodeMoved(NodeContext.GraphConfig.GraphName, NodePosX, NodePosY, bMarkDirty);
	}
}

UObjectTreeGraphNode::FNodeContext UObjectTreeGraphNode::GetNodeContext() const
{
	UObjectTreeGraph* OuterGraph = CastChecked<UObjectTreeGraph>(GetGraph());
	const FObjectTreeGraphConfig& OuterGraphConfig = OuterGraph->GetConfig();

	if (UObject* Object = WeakObject.Get())
	{
		UClass* ObjectClass = Object->GetClass();
		const FObjectTreeGraphClassConfig& ObjectClassConfig = OuterGraphConfig.GetObjectClassConfig(ObjectClass);

		return FNodeContext{ ObjectClass, OuterGraph, OuterGraphConfig, ObjectClassConfig };
	}
	else
	{
		const FObjectTreeGraphClassConfig& ObjectClassConfig = OuterGraphConfig.GetObjectClassConfig(nullptr);
		return FNodeContext{ nullptr, OuterGraph, OuterGraphConfig, ObjectClassConfig };
	}
}

const FObjectTreeGraphClassConfig& UObjectTreeGraphNode::GetObjectClassConfig() const
{
	return GetNodeContext().ObjectClassConfig;
}

#undef LOCTEXT_NAMESPACE

