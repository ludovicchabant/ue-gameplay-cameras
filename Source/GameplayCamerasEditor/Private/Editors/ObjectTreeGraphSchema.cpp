// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeGraphSchema.h"

#include "Core/ObjectTreeGraphRootObject.h"
#include "Editors/ObjectTreeConnectionDrawingPolicy.h"
#include "Editors/ObjectTreeGraph.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "Exporters/Exporter.h"
#include "Factories.h"
#include "GameplayCameras.h"
#include "IGameplayCamerasEditorModule.h"
#include "ScopedTransaction.h"
#include "Serialization/ArchiveUObject.h"
#include "UObject/FastReferenceCollector.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"
#include "UnrealExporter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ObjectTreeGraphSchema)

#define LOCTEXT_NAMESPACE "ObjectTreeGraphSchema"

const FName UObjectTreeGraphSchema::PC_Self("Self");
const FName UObjectTreeGraphSchema::PC_Property("Property");

const FName UObjectTreeGraphSchema::PSC_ObjectProperty("ObjectProperty");
const FName UObjectTreeGraphSchema::PSC_ArrayProperty("ArrayProperty");
const FName UObjectTreeGraphSchema::PSC_ArrayPropertyItem("ArrayPropertyItem");

namespace UE::ObjectTreeGraph
{

struct FPackageReferenceCollector : public FArchiveUObject
{
	FPackageReferenceCollector(UObject* InRootObject, TArray<UObject*>& InOutReferencedObjects)
		: FArchiveUObject()
		, RootObject(InRootObject)
		, PackageScope(InRootObject->GetOutermost())
		, ReferencedObjects(InOutReferencedObjects)
	{
		SetIsPersistent(true);
		SetIsSaving(true);
		SetFilterEditorOnly(false);

		ArIsObjectReferenceCollector = true;
		ArShouldSkipBulkData = true;
	}

	void StopAtObjectClasses(TArray<UClass*> InStopAtClasses)
	{
		StopAtClasses = TSet<UClass*>(InStopAtClasses);
	}

	void CollectReferences()
	{
		ObjectsToVisit.Reset();
		VisitedObjects.Reset();

		ObjectsToVisit.Add(RootObject);
		while (ObjectsToVisit.Num() > 0)
		{
			UObject* CurObj = ObjectsToVisit.Pop(EAllowShrinking::No);
			VisitedObjects.Add(CurObj);
			CurObj->Serialize(*this);
		}
	}

private:

	bool ShouldStopAt(UObject* Obj)
	{
		UClass* ObjClass = Obj->GetClass();
		for (UClass* StopAtClass : StopAtClasses)
		{
			if (ObjClass->IsChildOf(StopAtClass))
			{
				return true;
			}
		}
		return false;
	}

	virtual FArchive& operator<<(UObject*& ObjRef) override
	{
		if (ObjRef != nullptr && ObjRef->IsIn(PackageScope) && !ShouldStopAt(ObjRef))
		{
			if (!VisitedObjects.Contains(ObjRef))
			{
				ReferencedObjects.Add(ObjRef);
				ObjectsToVisit.Add(ObjRef);
			}
		}
		return *this;
	}

private:

	UObject* RootObject;
	UPackage* PackageScope;
	TSet<UClass*> StopAtClasses;

	TArray<UObject*> ObjectsToVisit;
	TSet<UObject*> VisitedObjects;

	TArray<UObject*>& ReferencedObjects;
};

class FObjectTextFactory : public FCustomizableTextObjectFactory
{
public:
	FObjectTextFactory()
		: FCustomizableTextObjectFactory(GWarn)
	{
	}

	// FCustomizableTextObjectFactory interface.
	virtual bool CanCreateClass(UClass* ObjectClass, bool& bOmitSubObjs) const override
	{
		return true;
	}

	virtual void ProcessConstructedObject(UObject* NewObject) override
	{
		check(NewObject);
		CreatedObjects.Add(NewObject);
	}

	TArray<UObject*> CreatedObjects;
};

}  // namespace UE::ObjectTreeGraph

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
	if (!RootObject)
	{
		return;
	}

	const FObjectTreeGraphConfig& GraphConfig = InGraph->GetConfig();

	TSet<UObject*> AllObjects;
	// Gather up all the objects we need for the graph. Start by all objects that are referenced (directly or
	// indirectly) by the root object. Our custom reference collector will not collect references that go outside
	// of the root object's package.
	if (GraphConfig.bAutoCollectInitialObjects)
	{
		using namespace UE::ObjectTreeGraph;

		// Make sure the root object itself is in there.
		AllObjects.Add(RootObject);

		TArray<UObject*> ReferencedObjects;
		FPackageReferenceCollector Collector(RootObject, ReferencedObjects);
		Collector.StopAtObjectClasses(GraphConfig.StopAutoCollectAtObjectClasses);
		Collector.CollectReferences();
		AllObjects.Append(ReferencedObjects);
	}
	// Add any other custom objects the root object may want.
	if (IObjectTreeGraphRootObject* RootObjectInterface = Cast<IObjectTreeGraphRootObject>(RootObject))
	{
		RootObjectInterface->GetConnectableObjects(GraphConfig.GraphName, AllObjects);
	}
	
	// Create all the nodes.
	FCreatedNodes CreatedNodes;
	for (UObject* Object : AllObjects)
	{
		if (UObjectTreeGraphNode* GraphNode = CreateObjectNode(InGraph, Object))
		{
			CreatedNodes.CreatedNodes.Add(Object, GraphNode);
		}
	}

	// Grab the graph node for the root object.
	InGraph->RootObjectNode = nullptr;
	if (!AllObjects.IsEmpty())
	{
		UObjectTreeGraphNode** CreatedRootObjectNode = CreatedNodes.CreatedNodes.Find(RootObject);
		if (ensure(CreatedRootObjectNode))
		{
			InGraph->RootObjectNode = *CreatedRootObjectNode;
		}
	}

	// Create all the connections.
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
				if (Pin->Direction == EGPD_Input)
				{
					(*ConnectedNode)->OverrideSelfPinDirection(EGPD_Output);
				}
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
					if (Pin->Direction == EGPD_Input)
					{
						(*ConnectedNode)->OverrideSelfPinDirection(EGPD_Output);
					}
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

	return OnCreateObjectNode(InGraph, InObject);
}

UObjectTreeGraphNode* UObjectTreeGraphSchema::OnCreateObjectNode(UObjectTreeGraph* InGraph, UObject* InObject) const
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

void UObjectTreeGraphSchema::AddConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InNewNode) const
{
	UObjectTreeGraphNode* RootObjectNode = InGraph->GetRootObjectNode();
	IObjectTreeGraphRootObject* RootObjectInterface = Cast<IObjectTreeGraphRootObject>(RootObjectNode->GetObject());
	if (RootObjectInterface)
	{
		const FName GraphName = InGraph->GetConfig().GraphName;
		RootObjectInterface->AddConnectableObject(GraphName, InNewNode->GetObject());
	}

	OnAddConnectableObject(InGraph, InNewNode);
}

void UObjectTreeGraphSchema::OnAddConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InNewNode) const
{
}

void UObjectTreeGraphSchema::RemoveConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InRemovedNode) const
{
	const FName GraphName = InGraph->GetConfig().GraphName;
	IObjectTreeGraphRootObject* RootObjectInterface = Cast<IObjectTreeGraphRootObject>(InGraph->GetRootObject());
	if (RootObjectInterface)
	{
		RootObjectInterface->RemoveConnectableObject(GraphName, InRemovedNode->GetObject());
	}

	OnRemoveConnectableObject(InGraph, InRemovedNode);
}

void UObjectTreeGraphSchema::OnRemoveConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InRemovedNode) const
{
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

	const FText MiscellaneousCategoryText = LOCTEXT("MiscellaneousCategory", "Miscellaneous");

	for (UClass* PossibleObjectClass : PossibleObjectClasses)
	{
		if (!PossibleObjectClass)
		{
			continue;
		}

		const FText DisplayName = GraphConfig.GetDisplayNameText(PossibleObjectClass);

		TArray<FString> CategoryNames;
		const FName CreateCategoryMetaData = GraphConfig.GetObjectClassConfig(PossibleObjectClass).CreateCategoryMetaData();
		for (UClass* CurClass = PossibleObjectClass; CurClass; CurClass = CurClass->GetSuperClass())
		{
			const FString* CategoryNamesMetaData = CurClass->FindMetaData(CreateCategoryMetaData);
			if (CategoryNamesMetaData)
			{
				CategoryNamesMetaData->ParseIntoArray(CategoryNames, TEXT(","), true);
				break;
			}
		}
		if (CategoryNames.IsEmpty())
		{
			CategoryNames.Add(FString());
		}

		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("Name"), DisplayName);
		const FText ToolTipText = FText::Format(LOCTEXT("NewNodeToolTip", "Adds a {Name} node here"), Arguments);

		checkSlow(PossibleObjectClass);
		for (const FString& CategoryName : CategoryNames)
		{
			FText CategoryText = MiscellaneousCategoryText;
			int32 Grouping = -1;
			if (!CategoryName.IsEmpty())
			{
				CategoryText = FText::FromString(CategoryName);
				Grouping = CategoryName == TEXT("Common") ? 1 : 0;
			}

			FText KeywordsText(FText::FromString(PossibleObjectClass->GetMetaData(TEXT("Keywords"))));

			TSharedRef<FObjectGraphSchemaAction_NewNode> Action = MakeShared<FObjectGraphSchemaAction_NewNode>(
					CategoryText, DisplayName, ToolTipText, Grouping, KeywordsText);
			Action->ObjectClass = PossibleObjectClass;
			ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(Action.ToSharedPtr()));
		}
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

	if (!ObjectA->CanEditChange(PropertyA))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Property cannot be changed"));
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
		ObjectA->PreEditChange(PropertyA);

		ObjectA->Modify();

		ObjectPropertyA->SetValue_InContainer(ObjectA, TObjectPtr<UObject>(ObjectB));

		FPropertyChangedEvent PropertyChangedEvent(PropertyA, EPropertyChangeType::ValueSet);
		ObjectA->PostEditChangeProperty(PropertyChangedEvent);
	}
	else if (FArrayProperty* ArrayPropertyA = CastField<FArrayProperty>(PropertyA))
	{
		ObjectA->PreEditChange(PropertyA);

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

		FPropertyChangedEvent PropertyChangedEvent(PropertyA);
		PropertyChangedEvent.ChangeType = bAddNewItemPin ? EPropertyChangeType::ArrayAdd : EPropertyChangeType::ValueSet;
		ObjectA->PostEditChangeProperty(PropertyChangedEvent);
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
		Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
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
		OwningObject->PreEditChange(Property);

		OwningObject->Modify();

		ObjectProperty->ClearValue_InContainer(OwningObject);

		FPropertyChangedEvent PropertyChangedEvent(Property, EPropertyChangeType::ValueSet);
		OwningObject->PostEditChangeProperty(PropertyChangedEvent);
	}
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		OwningObject->PreEditChange(Property);

		OwningObject->Modify();

		int32 Index = PropertyOwningNode->GetIndexOfArrayPin(PropertyPin);
		ensure(Index != INDEX_NONE);

		FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(OwningObject));
		ArrayHelper.RemoveValues(Index);

		bRemovePropertyPin = true;

		FPropertyChangedEvent PropertyChangedEvent(Property, EPropertyChangeType::ArrayRemove);
		OwningObject->PostEditChangeProperty(PropertyChangedEvent);
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
		Super::BreakSinglePinLink(SourcePin, TargetPin);
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

bool UObjectTreeGraphSchema::SafeDeleteNodeFromGraph(UEdGraph* Graph, UEdGraphNode* Node) const
{
	if (!Graph || !Node)
	{
		return false;
	}
	
	const FScopedTransaction Transaction(LOCTEXT("DeleteNode", "Delete Node"));

	BreakNodeLinks(*Node);

	UObjectTreeGraph* ObjectTreeGraph = CastChecked<UObjectTreeGraph>(Graph);
	OnDeleteNodeFromGraph(ObjectTreeGraph, Node);
	Node->DestroyNode();

	return true;
}

void UObjectTreeGraphSchema::OnDeleteNodeFromGraph(UObjectTreeGraph* Graph, UEdGraphNode* Node) const
{
	UObjectTreeGraphNode* ObjectNode = Cast<UObjectTreeGraphNode>(Node);
	if (ObjectNode)
	{
		RemoveConnectableObject(Graph, ObjectNode);
	}
}

void UObjectTreeGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& OutDisplayInfo) const
{
	const UObjectTreeGraph* ObjectTreeGraph = CastChecked<const UObjectTreeGraph>(&Graph);
	const FObjectTreeGraphConfig& GraphConfig = ObjectTreeGraph->GetConfig();

	OutDisplayInfo = GraphConfig.GraphDisplayInfo;

	if (OutDisplayInfo.PlainName.IsEmpty())
	{
		OutDisplayInfo.PlainName = FText::FromString(Graph.GetName());
	}
	if (OutDisplayInfo.DisplayName.IsEmpty())
	{
		OutDisplayInfo.DisplayName = OutDisplayInfo.PlainName;
	}

	if (GraphConfig.OnGetGraphDisplayInfo.IsBound())
	{
		GraphConfig.OnGetGraphDisplayInfo.Execute(ObjectTreeGraph, OutDisplayInfo);
	}
}

FString UObjectTreeGraphSchema::ExportNodesToText(const FGraphPanelSelectionSet& Nodes, bool bOnlyCanDuplicateNodes, bool bOnlyCanDeleteNodes) const
{
	// Gather up the nodes we need to copy from.
	TSet<UObject*> ObjectsToExport;
	TSet<UObject*> OtherNodesToExport;

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(Nodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt);
		if (Node && 
				(!bOnlyCanDuplicateNodes || Node->CanDuplicateNode()) &&
				(!bOnlyCanDeleteNodes || Node->CanUserDeleteNode()))
		{
			Node->PrepareForCopying();

			if (UObjectTreeGraphNode* ObjectTreeNode = Cast<UObjectTreeGraphNode>(Node))
			{
				ObjectsToExport.Add(ObjectTreeNode->GetObject());
			}
			else
			{
				OtherNodesToExport.Add(Node);
			}
		}
	}

	if (ObjectsToExport.IsEmpty() && OtherNodesToExport.IsEmpty())
	{
		return FString();
	}

	// Clear the mark state for saving.
	UnMarkAllObjects(EObjectMark(OBJECTMARK_TagExp | OBJECTMARK_TagImp));

	FStringOutputDevice Archive;
	const FExportObjectInnerContext Context;

	UObject* LastOuter = nullptr;
	for (UObject* ObjectToExport : ObjectsToExport)
	{
		// The nodes should all be from the same scope.
		UObject* ThisOuter = ObjectToExport->GetOuter();
		if (LastOuter != nullptr && ThisOuter != LastOuter)
		{
			UE_LOG(LogCameraSystemEditor, Warning,
					TEXT("Cannot copy objects from different outers. Only copying from %s"), *LastOuter->GetName());
			continue;
		}
		LastOuter = ThisOuter;

		UExporter::ExportToOutputDevice(
				&Context,
				ObjectToExport, 
				nullptr, // no exporter
				Archive, 
				TEXT("copy"), // file type
				0, // indent
				PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited, // port flags
				false, // selected only
				ThisOuter // export root scope
				);
	}

	if (!OtherNodesToExport.IsEmpty())
	{
		CopyNonObjectNodes(OtherNodesToExport.Array(), Archive);
	}

	return Archive;
}

void UObjectTreeGraphSchema::CopyNonObjectNodes(TArrayView<UObject*> InObjects, FStringOutputDevice& OutDevice) const
{
}

void UObjectTreeGraphSchema::ImportNodesFromText(UObjectTreeGraph* InGraph, const FString& TextToImport, TArray<UEdGraphNode*>& OutPastedNodes) const
{
	using namespace UE::ObjectTreeGraph;

	TArray<UObject*> ImportedObjects;

	// Import the given text as new objects.
	UPackage* TempPackage = NewObject<UPackage>(nullptr, TEXT("/Engine/GameplayCamerasEditor/Transient"), RF_Transient);
	TempPackage->AddToRoot();
	{
		FObjectTextFactory Factory;
		Factory.ProcessBuffer(TempPackage, RF_Transactional, TextToImport);
		ImportedObjects = Factory.CreatedObjects;
	}
	TempPackage->RemoveFromRoot();

	// Finish setting up the new objects: clear the transient flag from the transient package we used above,
	// and move the objects under the our graph root.
	UObject* GraphRootObject = InGraph->GetRootObject();
	if (ensure(GraphRootObject))
	{
		for (UObject* Object : ImportedObjects)
		{
			Object->ClearFlags(RF_Transient);
			Object->Rename(nullptr, GraphRootObject);
		}
	}

	// Create nodes for all the imported objects, and add them to the root object if it supports the root interface.
	FCreatedNodes CreatedNodes;
	for (UObject* Object : ImportedObjects)
	{
		if (UObjectTreeGraphNode* GraphNode = CreateObjectNode(InGraph, Object))
		{
			CreatedNodes.CreatedNodes.Add(Object, GraphNode);

			AddConnectableObject(InGraph, GraphNode);
		}
	}

	// Create all the connections.
	for (TPair<UObject*, UObjectTreeGraphNode*> Pair : CreatedNodes.CreatedNodes)
	{
		CreateConnections(Pair.Value, CreatedNodes);
	}

	OnCreateAllNodes(InGraph, CreatedNodes);

	for (const TTuple<UObject*, UObjectTreeGraphNode*>& Pair : CreatedNodes.CreatedNodes)
	{
		OutPastedNodes.Add(Pair.Value);
	}
}

bool UObjectTreeGraphSchema::CanImportNodesFromText(UObjectTreeGraph* InGraph, const FString& TextToImport) const
{
	using namespace UE::ObjectTreeGraph;

	FObjectTextFactory Factory;
	return Factory.CanCreateObjectsFromText(TextToImport);
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

	const FScopedTransaction Transaction(FText::Format(LOCTEXT("CreateNewNodeAction", "Create {0} Node"), ObjectClass->GetDisplayNameText()));

	const UObjectTreeGraphSchema* Schema = CastChecked<UObjectTreeGraphSchema>(ParentGraph->GetSchema());

	UObject* NewObject = CreateObject();

	if (NewObject)
	{
		UObjectTreeGraphNode* NewGraphNode = Schema->CreateObjectNode(ObjectTreeGraph, NewObject);

		Schema->AddConnectableObject(ObjectTreeGraph, NewGraphNode);

		NewGraphNode->NodePosX = Location.X;
		NewGraphNode->NodePosY = Location.Y;
		NewGraphNode->OnGraphNodeMoved(false);

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

