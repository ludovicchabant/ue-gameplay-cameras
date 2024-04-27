// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeGraphSchema.h"

#include "Editors/ObjectTreeConnectionDrawingPolicy.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "ScopedTransaction.h"
#include "UObject/UObjectIterator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ObjectTreeGraphSchema)

#define LOCTEXT_NAMESPACE "ObjectTreeGraphSchema"

const FName UObjectTreeGraphSchema::PC_Self("Self");
const FName UObjectTreeGraphSchema::PC_Property("Property");

const FName UObjectTreeGraphSchema::PSC_ObjectProperty("ObjectProperty");
const FName UObjectTreeGraphSchema::PSC_ArrayProperty("ArrayProperty");
const FName UObjectTreeGraphSchema::PSC_ArrayPropertyItem("ArrayPropertyItem");

UObjectTreeGraphSchema::UObjectTreeGraphSchema(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
}

void UObjectTreeGraphSchema::RebuildGraph(UObjectTreeGraph* InGraph, EObjectTreeGraphBuildSource InSource) const
{
	RemoveAllNodes(InGraph);
	CreateAllNodes(InGraph, InSource);
	InGraph->NotifyGraphChanged();
}

void UObjectTreeGraphSchema::RemoveAllNodes(UObjectTreeGraph* InGraph) const
{
	TArray<UEdGraphNode*> NodesToRemove(InGraph->Nodes);  // Copy all nodes to remove them.
	for (UEdGraphNode* NodeToRemove : NodesToRemove)
	{
		InGraph->RemoveNode(NodeToRemove);
	}
}

void UObjectTreeGraphSchema::CreateAllNodes(UObjectTreeGraph* InGraph, EObjectTreeGraphBuildSource InSource) const
{
	UObject* RootObject = InGraph->GetRootObject();
	if (!ensure(RootObject))
	{
		return;
	}

	TArray<UObject*> AllObjects;
	UPackage* Package = RootObject->GetOutermost();
	GetObjectsWithPackage(Package, AllObjects);

	ensure(AllObjects.Contains(RootObject));

	FCreatedNodes CreatedNodes;
	for (UObject* Object : AllObjects)
	{
		if (UObjectTreeGraphNode* GraphNode = CreateObjectNode(InGraph, Object))
		{
			CreatedNodes.CreatedNodes.Add(Object, GraphNode);
		}
	}

	InGraph->RootObjectNode = nullptr;
	UObjectTreeGraphNode** CreatedRootObjectNode = CreatedNodes.CreatedNodes.Find(RootObject);
	if (ensure(CreatedRootObjectNode))
	{
		InGraph->RootObjectNode = *CreatedRootObjectNode;
	}

	for (TPair<UObject*, UObjectTreeGraphNode*> Pair : CreatedNodes.CreatedNodes)
	{
		CreateConnections(Pair.Value, CreatedNodes);
	}

	OnCreateAllNodes(InGraph, CreatedNodes);
}

void UObjectTreeGraphSchema::CreateConnections(UObjectTreeGraphNode* InGraphNode, const FCreatedNodes& InCreatedNodes) const
{
	UObject* Object = InGraphNode->GetObject();
	UClass* ObjectClass = Object->GetClass();

	TArray<FProperty*> ConnectableProperties;
	InGraphNode->GetAllConnectableProperties(ConnectableProperties);

	for (FProperty* ConnectableProperty : ConnectableProperties)
	{
		if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(ConnectableProperty))
		{
			UEdGraphPin* Pin = InGraphNode->GetPinForProperty(ObjectProperty);
			if (!ensure(Pin))
			{
				continue;
			}

			TObjectPtr<UObject> OutConnectedObject;
			ObjectProperty->GetValue_InContainer(Object, &OutConnectedObject);
			if (!OutConnectedObject)
			{
				continue;
			}

			UObjectTreeGraphNode* const* ConnectedNode = InCreatedNodes.CreatedNodes.Find(OutConnectedObject);
			if (ensure(ConnectedNode))
			{
				UEdGraphPin* ConnectedPin = (*ConnectedNode)->GetSelfPin();
				Pin->MakeLinkTo(ConnectedPin);
			}
		}
		else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(ConnectableProperty))
		{
			FObjectProperty* InnerProperty = CastFieldChecked<FObjectProperty>(ArrayProperty->Inner);
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Object));

			const int32 ArrayNum = ArrayHelper.Num();
			for (int32 Index = 0; Index < ArrayNum; ++Index)
			{
				UEdGraphPin* Pin = InGraphNode->GetPinForPropertyNewItem(ArrayProperty, true);
				if (!ensure(Pin))
				{
					continue;
				}

				UObject* ConnectedObject = InnerProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(Index));
				if (!ConnectedObject)
				{
					continue;
				}

				UObjectTreeGraphNode* const* ConnectedNode = InCreatedNodes.CreatedNodes.Find(ConnectedObject);
				if (ensure(ConnectedNode))
				{
					UEdGraphPin* ConnectedPin = (*ConnectedNode)->GetSelfPin();
					Pin->MakeLinkTo(ConnectedPin);
				}
			}
		}
	}
}

void UObjectTreeGraphSchema::OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const
{
}

UObjectTreeGraphNode* UObjectTreeGraphSchema::CreateObjectNode(UObjectTreeGraph* InGraph, UObject* InObject) const
{
	if (!InObject)
	{
		return nullptr;
	}

	if (!InGraph->GetConfig().IsConnectable(InObject->GetClass()))
	{
		return nullptr;
	}

	return CreateObjectNodeImpl(InGraph, InObject);
}

UObjectTreeGraphNode* UObjectTreeGraphSchema::CreateObjectNodeImpl(UObjectTreeGraph* InGraph, UObject* InObject) const
{
	const FObjectTreeGraphConfig& Config = InGraph->GetConfig();
	const FObjectTreeGraphClassConfig& ClassConfig = Config.GetObjectClassConfig(InObject->GetClass());

	TSubclassOf<UObjectTreeGraphNode> GraphNodeClass = ClassConfig.GraphNodeClass();
	if (!GraphNodeClass.Get())
	{
		GraphNodeClass = Config.DefaultGraphNodeClass;
	}

	FGraphNodeCreator<UObjectTreeGraphNode> GraphNodeCreator(*InGraph);
	UObjectTreeGraphNode* NewNode = GraphNodeCreator.CreateNode(false, GraphNodeClass);
	NewNode->Initialize(InObject);
	GraphNodeCreator.Finalize();
	return NewNode;
}

void UObjectTreeGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const UObjectTreeGraph* Graph = CastChecked<UObjectTreeGraph>(ContextMenuBuilder.CurrentGraph);
	const FObjectTreeGraphConfig& GraphConfig = Graph->GetConfig();

	// Find the common class restriction for all the dragged pins. We will only show actions that
	// are compatible with them.
	UClass* DraggedPinClass = nullptr;
	bool bShouldShowNewObjectActions = true;
	if (const UEdGraphPin* DraggedPin = ContextMenuBuilder.FromPin)
	{
		UObjectTreeGraphNode* OwningNode = Cast<UObjectTreeGraphNode>(DraggedPin->GetOwningNode());
		if (OwningNode)
		{
			if (DraggedPin->PinType.PinCategory == UObjectTreeGraphSchema::PC_Self)
			{
				DraggedPinClass = OwningNode->GetObject()->GetClass();
			}
			else if (DraggedPin->PinType.PinCategory == UObjectTreeGraphSchema::PC_Property)
			{
				DraggedPinClass = OwningNode->GetConnectedObjectClassForPin(DraggedPin);
			}
			else
			{
				// Dragged an unknown pin...
				bShouldShowNewObjectActions = false;
			}
		}
		else
		{
			// Dragged a pin from an unknown node...
				bShouldShowNewObjectActions = false;
		}
	}
	if (!bShouldShowNewObjectActions)
	{
		// Don't show anything.
		return;
	}

	// Find all the object classes we can create from those pins, for the given graph.
	TArray<UClass*> PossibleObjectClasses;
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		if (ClassIt->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			continue;
		}
		if (ClassIt->HasAnyClassFlags(CLASS_Hidden | CLASS_NotPlaceable))
		{
			continue;
		}

		if (!GraphConfig.IsConnectable(*ClassIt))
		{
			continue;
		}

		const FObjectTreeGraphClassConfig& ClassConfig = GraphConfig.GetObjectClassConfig(*ClassIt);
		if (!ClassConfig.CanCreateNew())
		{
			continue;
		}

		if (DraggedPinClass && !ClassIt->IsChildOf(DraggedPinClass))
		{
			continue;
		}

		PossibleObjectClasses.Add(*ClassIt);
	}

	FilterGraphContextPlaceableClasses(PossibleObjectClasses);

	for (UClass* PossibleObjectClass : PossibleObjectClasses)
	{
		if (!PossibleObjectClass)
		{
			continue;
		}

		const FString* CategoryName = nullptr;
		for (UClass* CurClass = PossibleObjectClass; CurClass; CurClass = CurClass->GetSuperClass())
		{
			CategoryName = CurClass->FindMetaData(TEXT("ObjectTreeGraphCategory"));
			if (CategoryName)
			{
				break;
			}
		}

		checkSlow(PossibleObjectClass);
		TSharedRef<FObjectGraphSchemaAction_NewNode> Action = MakeShared<FObjectGraphSchemaAction_NewNode>(
				CategoryName ? FText::FromString(*CategoryName) : FText::GetEmpty(),
				PossibleObjectClass->GetDisplayNameText(), 
				PossibleObjectClass->GetDisplayNameText());
		Action->ObjectClass = PossibleObjectClass;
		ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(Action.ToSharedPtr()));
	}

	// Don't call the base class, we want to control exactly what can be created.
}

void UObjectTreeGraphSchema::FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const
{
}

void UObjectTreeGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);
}

FName UObjectTreeGraphSchema::GetParentContextMenuName() const
{
	// Return NAME_None if we don't want the default menu entries.
	return Super::GetParentContextMenuName();
}

FLinearColor UObjectTreeGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

FConnectionDrawingPolicy* UObjectTreeGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, UEdGraph* InGraph) const
{
	return new FObjectTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}

bool UObjectTreeGraphSchema::ShouldAlwaysPurgeOnModification() const
{
	return false;
}

FPinConnectionResponse UObjectTreeGraphSchema::CanCreateNewNodes(UEdGraphPin* InSourcePin) const
{
	return Super::CanCreateNewNodes(InSourcePin);
}

const FPinConnectionResponse UObjectTreeGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	UObjectTreeGraphNode* NodeA = Cast<UObjectTreeGraphNode>(A->GetOwningNode());
	UObjectTreeGraphNode* NodeB = Cast<UObjectTreeGraphNode>(B->GetOwningNode());
	if (!NodeA || !NodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Unsupported node types"));
	}

	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Incompatible pins"));
	}
	
	// Try to always reason back to A being the property pin, and B being the self pin of the
	// object we want to set on the property.
	if (A->PinType.PinCategory == PC_Self)
	{
		Swap(A, B);
		Swap(NodeA, NodeB);
	}

	const bool bIsPropertyToSelf = (A->PinType.PinCategory == PC_Property && B->PinType.PinCategory == PC_Self);
	if (!bIsPropertyToSelf)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Connection must be between a property pin and a self pin"));
	}

	UObject* ObjectA = NodeA->GetObject();
	UObject* ObjectB = NodeB->GetObject();
	UClass* ObjectClassB = ObjectB->GetClass();

	FProperty* PropertyA = NodeA->GetPropertyForPin(A);
	if (!PropertyA)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Unsupported source pin"));
	}

	if (FObjectProperty* ObjectPropertyA = CastField<FObjectProperty>(PropertyA))
	{
		if (ObjectClassB->IsChildOf(ObjectPropertyA->PropertyClass))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible pin types"));
		}
		else
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Incompatible pin types"));
		}
	}
	else if (FArrayProperty* ArrayPropertyA = CastField<FArrayProperty>(PropertyA))
	{
		FObjectProperty* InnerPropertyA = CastFieldChecked<FObjectProperty>(ArrayPropertyA->Inner);
		if (ObjectClassB->IsChildOf(InnerPropertyA->PropertyClass))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT("Compatible array pin types"));
		}
		else
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Incompatible array pin types"));
		}
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Unsupported source pin type"));
	}
}

bool UObjectTreeGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	const bool bModified = UEdGraphSchema::TryCreateConnection(A, B);
	if (!bModified)
	{
		return false;
	}

	const bool bHandled = OnCreateConnection(A, B);
	if (bHandled)
	{
		return true;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateConnection", "Create Connection"));

	UObjectTreeGraphNode* NodeA = Cast<UObjectTreeGraphNode>(A->GetOwningNode());
	UObjectTreeGraphNode* NodeB = Cast<UObjectTreeGraphNode>(B->GetOwningNode());

	// Try to always reason back to A being the property pin, and B being the self pin of the
	// object we want to set on the property.
	if (A->PinType.PinCategory == PC_Self)
	{
		Swap(A, B);
		Swap(NodeA, NodeB);
	}
	// We know we are in the right configuration now because UEdGraphSchema::TryCreateConnection
	// already called CanCreateConnection, which we implemented above as checking that A and B
	// are a property/self pin pair, one way or the other.

	UObject* ObjectA = NodeA->GetObject();
	UObject* ObjectB = NodeB->GetObject();

	FProperty* PropertyA = NodeA->GetPropertyForPin(A);

	if (FObjectProperty* ObjectPropertyA = CastField<FObjectProperty>(PropertyA))
	{
		ObjectA->Modify();

		ObjectPropertyA->SetValue_InContainer(ObjectA, TObjectPtr<UObject>(ObjectB));
	}
	else if (FArrayProperty* ArrayPropertyA = CastField<FArrayProperty>(PropertyA))
	{
		ObjectA->Modify();

		const int32 Index = NodeA->GetIndexOfArrayPin(A);
		ensure(Index != INDEX_NONE);

		FScriptArrayHelper ArrayHelper(ArrayPropertyA, ArrayPropertyA->ContainerPtrToValuePtr<void>(ObjectA));
		const bool bAddNewItemPin = ArrayHelper.ExpandForIndex(Index);

		FObjectProperty* InnerProperty = CastFieldChecked<FObjectProperty>(ArrayPropertyA->Inner);
		InnerProperty->SetObjectPropertyValue(ArrayHelper.GetRawPtr(Index), ObjectB);

		if (bAddNewItemPin)
		{
			NodeA->CreateNewItemPin(*ArrayPropertyA);
			NodeA->GetGraph()->NotifyNodeChanged(NodeA);
		}
	}

	return true;
}

bool UObjectTreeGraphSchema::OnCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	return false;
}

void UObjectTreeGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	if (TargetPin.LinkedTo.IsEmpty())
	{
		Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
		return;
	}

	const bool bHandled = OnBreakPinLinks(TargetPin, bSendsNodeNotification);
	if (bHandled)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("BreakPinLinks", "Break Pin Links"));

	// TargetPin could be a self pin or a property pin, we need to handle both cases and directions.
	UEdGraphPin* PropertyPin = &TargetPin;
	UObjectTreeGraphNode* PropertyOwningNode = Cast<UObjectTreeGraphNode>(PropertyPin->GetOwningNode());
	if (PropertyPin->PinType.PinCategory == PC_Self)
	{
		PropertyPin = TargetPin.LinkedTo[0];
		PropertyOwningNode = Cast<UObjectTreeGraphNode>(PropertyPin->GetOwningNode());
	}

	bool bRemovePropertyPin = false;
	UObject* OwningObject = PropertyOwningNode->GetObject();
	FProperty* Property = PropertyOwningNode->GetPropertyForPin(PropertyPin);

	if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		OwningObject->Modify();

		ObjectProperty->ClearValue_InContainer(OwningObject);
	}
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		OwningObject->Modify();

		int32 Index = PropertyOwningNode->GetIndexOfArrayPin(PropertyPin);
		ensure(Index != INDEX_NONE);

		FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(OwningObject));
		ArrayHelper.RemoveValues(Index);

		bRemovePropertyPin = true;
	}

	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);

	if (bRemovePropertyPin)
	{
		PropertyOwningNode->RemoveItemPin(PropertyPin);
		PropertyOwningNode->GetGraph()->NotifyNodeChanged(PropertyOwningNode);
	}
}

bool UObjectTreeGraphSchema::OnBreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	return false;
}

void UObjectTreeGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const bool bHandled = OnBreakSinglePinLink(SourcePin, TargetPin);
	if (bHandled)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("BreakSinglePinLink", "Break Pin Link"));

	// SourcePin could be the self-pin, and TargetPin the property pin, if the directions
	// are reversed for that type of object/graph.
	UEdGraphPin* PropertyPin = SourcePin;
	UObjectTreeGraphNode* PropertyOwningNode = Cast<UObjectTreeGraphNode>(SourcePin->GetOwningNode());
	if (SourcePin->PinType.PinCategory == PC_Self)
	{
		PropertyPin = SourcePin->LinkedTo[0];
		PropertyOwningNode = Cast<UObjectTreeGraphNode>(PropertyPin->GetOwningNode());
	}

	bool bRemovePropertyPin = false;
	UObject* OwningObject = PropertyOwningNode->GetObject();
	FProperty* Property = PropertyOwningNode->GetPropertyForPin(PropertyPin);

	if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		OwningObject->Modify();

		ObjectProperty->ClearValue_InContainer(OwningObject);
	}
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		OwningObject->Modify();

		int32 Index = PropertyOwningNode->GetIndexOfArrayPin(PropertyPin);
		ensure(Index != INDEX_NONE);

		FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(OwningObject));
		ArrayHelper.RemoveValues(Index);

		bRemovePropertyPin = true;
	}

	Super::BreakSinglePinLink(SourcePin, TargetPin);

	if (bRemovePropertyPin)
	{
		PropertyOwningNode->RemoveItemPin(PropertyPin);
		PropertyOwningNode->GetGraph()->NotifyNodeChanged(PropertyOwningNode);
	}
}

bool UObjectTreeGraphSchema::OnBreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	return false;
}

bool UObjectTreeGraphSchema::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Super::SupportsDropPinOnNode(InTargetNode, InSourcePinType, InSourcePinDirection, OutErrorMessage);
}

void UObjectTreeGraphSchema::ProcessDuplicatedNodes(UObjectTreeGraph* InGraph, const TMap<UEdGraphNode*, UEdGraphNode*>& NodeMap) const
{
	UPackage* RootObjectPackage = InGraph->GetRootObject()->GetOutermost();

	// Create object duplicates for all the new nodes.
	for (TPair<UEdGraphNode*, UEdGraphNode*> Pair : NodeMap)
	{
		const UObjectTreeGraphNode* OldNode = Cast<UObjectTreeGraphNode>(Pair.Key);
		UObjectTreeGraphNode* NewNode = Cast<UObjectTreeGraphNode>(Pair.Value);

		if (OldNode && NewNode)
		{
			ensure(OldNode->GetObject() == NewNode->GetObject());

			UObject* NewNodeObject = DuplicateObject<UObject>(OldNode->GetObject(), RootObjectPackage);
			NewNode->Initialize(NewNodeObject);
		}
	}

	// Patch-up property references on the new nodes' objects.
	for (TPair<UEdGraphNode*, UEdGraphNode*> Pair : NodeMap)
	{
		UObjectTreeGraphNode* NewNode = Cast<UObjectTreeGraphNode>(Pair.Value);
		if (NewNode)
		{
			NewNode->PostDuplicateObject(NodeMap);
		}
	}

	// Notify for everything.
	for (TPair<UEdGraphNode*, UEdGraphNode*> Pair : NodeMap)
	{
		UObjectTreeGraphNode* NewNode = Cast<UObjectTreeGraphNode>(Pair.Value);
		InGraph->NotifyNodeChanged(NewNode);
	}
	InGraph->NotifyGraphChanged();
}

const FObjectTreeGraphClassConfig& UObjectTreeGraphSchema::GetObjectClassConfig(const UObjectTreeGraphNode* InNode) const
{
	const UObjectTreeGraph* Graph = CastChecked<UObjectTreeGraph>(InNode->GetGraph());
	return GetObjectClassConfig(Graph, InNode->GetObject()->GetClass());
}

const FObjectTreeGraphClassConfig& UObjectTreeGraphSchema::GetObjectClassConfig(const UObjectTreeGraph* InGraph, UClass* InObjectClass) const
{
	return InGraph->GetConfig().GetObjectClassConfig(InObjectClass);
}

FObjectGraphSchemaAction_NewNode::FObjectGraphSchemaAction_NewNode()
{
}

FObjectGraphSchemaAction_NewNode::FObjectGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
}

UEdGraphNode* FObjectGraphSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UObjectTreeGraph* ObjectTreeGraph = Cast<UObjectTreeGraph>(ParentGraph);
	if (!ensure(ObjectTreeGraph))
	{
		return nullptr;
	}

	if (!ensure(ObjectClass))
	{
		return nullptr;
	}

	if (!ObjectOuter)
	{
		if (ensure(ObjectTreeGraph))
		{
			ObjectOuter = ObjectTreeGraph->GetRootObject();
		}
	}

	if (!ensure(ObjectOuter))
	{
		return nullptr;
	}

	const FScopedTransaction Transaction(LOCTEXT("CreateNewNodeAction", "Create New Node"));
	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(ParentGraph->GetSchema());

	UObject* NewObject = CreateObject();

	if (NewObject)
	{
		UObjectTreeGraphNode* NewGraphNode = Schema->CreateObjectNode(ObjectTreeGraph, NewObject);

		NewGraphNode->NodePosX = Location.X;
		NewGraphNode->NodePosY = Location.Y;
		NewGraphNode->OnGraphNodeMoved();

		AutoSetupNewNode(NewGraphNode, FromPin);

		return NewGraphNode;
	}
	
	return nullptr;
}

UObject* FObjectGraphSchemaAction_NewNode::CreateObject()
{
	return NewObject<UObject>(ObjectOuter, ObjectClass, NAME_None, RF_Transactional);
}

void FObjectGraphSchemaAction_NewNode::AutoSetupNewNode(UObjectTreeGraphNode* NewNode, UEdGraphPin* FromPin)
{
	NewNode->AutowireNewNode(FromPin);
}

#undef LOCTEXT_NAMESPACE

