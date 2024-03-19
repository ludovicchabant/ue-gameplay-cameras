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

enum class EObjectTreeGraphBuildSource
{
	RootObjectPackage
};

UCLASS()
class UObjectTreeGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	UObjectTreeGraph(const FObjectInitializer& ObjInit);

	void Initialize(TObjectPtr<UObject> InRootObject, const FObjectTreeGraphConfig& InConfig);

	UObject* GetRootObject() const { return WeakRootObject.Get(); }
	UObjectTreeGraphNode* GetRootObjectNode() const { return RootObjectNode; }
	const FObjectTreeGraphConfig& GetConfig() const;

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

