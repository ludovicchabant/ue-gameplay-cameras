// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "CoreTypes.h"
#include "EdGraph/EdGraph.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Templates/SubclassOf.h"
#include "UObject/WeakObjectPtrFwd.h"

#include "ObjectTreeGraph.generated.h"

class UObjectTreeGraphNode;
class UObjectTreeGraphSchema;

/** Specifies where to find graph node objects. */
enum class EObjectTreeGraphBuildSource
{
	/** Create graph nodes from all eligible objects found in the same package. */
	RootObjectPackage
};

/**
 * A node graph that represents a hierarchy of objects and their relationships.
 */
UCLASS()
class UObjectTreeGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	/** Creates a new graph. */
	UObjectTreeGraph(const FObjectInitializer& ObjInit);

	/** Initializes the graph given a root object and a graph configuration. */
	void Initialize(TObjectPtr<UObject> InRootObject, const FObjectTreeGraphConfig& InConfig);

	/** Gets the root object. */
	UObject* GetRootObject() const { return WeakRootObject.Get(); }
	/** Gets the root object's graph node. */
	UObjectTreeGraphNode* GetRootObjectNode() const { return RootObjectNode; }
	/** Gets the graph configuration. */
	const FObjectTreeGraphConfig& GetConfig() const;

	/** Rebuilds the node graph. */
	void RebuildGraph(EObjectTreeGraphBuildSource InSource);

private:

	struct FCreatedNodes
	{
		TMap<UObject*, UObjectTreeGraphNode*> CreatedNodes;
	};

	void RemoveAllNodes();
	void CreateAllNodes(EObjectTreeGraphBuildSource InSource);
	void CreateConnections(UObjectTreeGraphNode* InGraphNode, const FCreatedNodes& InCreatedNodes);

private:

	TWeakObjectPtr<> WeakRootObject;

	FObjectTreeGraphConfig Config;

	UPROPERTY()
	TObjectPtr<UObjectTreeGraphNode> RootObjectNode;
};

