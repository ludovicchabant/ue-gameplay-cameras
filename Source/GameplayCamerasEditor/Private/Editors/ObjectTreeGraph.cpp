// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/ObjectTreeGraph.h"

#include "Editors/ObjectTreeGraphNode.h"
#include "Editors/ObjectTreeGraphSchema.h"
#include "Misc/CoreMiscDefines.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ObjectTreeGraph)

UObjectTreeGraph::UObjectTreeGraph(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	Schema = UObjectTreeGraphSchema::StaticClass();
}

void UObjectTreeGraph::Initialize(TObjectPtr<UObject> InRootObject, const FObjectTreeGraphConfig& InConfig)
{
	WeakRootObject = InRootObject;

	Config = InConfig;
	ensureMsgf(Config.ConnectableObjectClasses.Num() > 0, TEXT("No connectable object classes specified... no graph can be created!"));
	if (!Config.DefaultGraphNodeClass)
	{
		Config.DefaultGraphNodeClass = UObjectTreeGraphNode::StaticClass();
	}
}

const FObjectTreeGraphConfig& UObjectTreeGraph::GetConfig() const
{
	return Config;
}

void UObjectTreeGraph::RebuildGraph(EObjectTreeGraphBuildSource InSource)
{
	RemoveAllNodes();
	CreateAllNodes(InSource);
	NotifyGraphChanged();
}

void UObjectTreeGraph::RemoveAllNodes()
{
	TArray<UEdGraphNode*> NodesToRemove(Nodes);  // Copy all nodes to remove them.
	for (UEdGraphNode* NodeToRemove : NodesToRemove)
	{
		RemoveNode(NodeToRemove);
	}
}

void UObjectTreeGraph::CreateAllNodes(EObjectTreeGraphBuildSource InSource)
{
	const UObjectTreeGraphSchema* GraphSchema = Cast<UObjectTreeGraphSchema>(GetSchema());

	UObject* RootObject = WeakRootObject.Get();
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
		if (UObjectTreeGraphNode* GraphNode = GraphSchema->CreateObjectNode(this, Object))
		{
			CreatedNodes.CreatedNodes.Add(Object, GraphNode);
		}
	}

	RootObjectNode = nullptr;
	UObjectTreeGraphNode** CreatedRootObjectNode = CreatedNodes.CreatedNodes.Find(RootObject);
	if (ensure(CreatedRootObjectNode))
	{
		RootObjectNode = *CreatedRootObjectNode;
	}

	for (TPair<UObject*, UObjectTreeGraphNode*> Pair : CreatedNodes.CreatedNodes)
	{
		CreateConnections(Pair.Value, CreatedNodes);
	}
}

void UObjectTreeGraph::CreateConnections(UObjectTreeGraphNode* InGraphNode, const FCreatedNodes& InCreatedNodes)
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

