// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "EdGraph/EdGraphSchema.h"

#include "ObjectTreeGraphSchema.generated.h"

class UEdGraph;
class UObjectTreeGraph;
class UObjectTreeGraphNode;
struct FObjectTreeGraphClassConfig;

/**
 * Schema class for an object tree graph.
 */
UCLASS()
class UObjectTreeGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:

	// Pin categories.
	static const FName PC_Self;			// A "self" pin.
	static const FName PC_Property;		// A property pin.

	// Pin sub-categories.
	static const FName PSC_ObjectProperty;		// A normal object property pin.
	static const FName PSC_ArrayProperty;		// An array property pin (generally hidden).
	static const FName PSC_ArrayPropertyItem;	// A pin for an item inside an array property.

public:

	/** Creates a new schema. */
	UObjectTreeGraphSchema(const FObjectInitializer& ObjInit);

	/** Creates an object graph node for the given object. */
	UObjectTreeGraphNode* CreateObjectNode(UObjectTreeGraph* InGraph, UObject* InObject) const;

public:

	// UEdGraphSchema interface.
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual FName GetParentContextMenuName() const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, UEdGraph* InGraph) const override;
	virtual bool ShouldAlwaysPurgeOnModification() const override;
	virtual FPinConnectionResponse CanCreateNewNodes(UEdGraphPin* InSourcePin) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	virtual bool SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const override;

	// UObjectTreeGraphSchema interface.
	virtual void FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const;
	virtual void ProcessDuplicatedNodes(UObjectTreeGraph* InGraph, const TMap<UEdGraphNode*, UEdGraphNode*>& NodeMap) const;

protected:

	virtual UObjectTreeGraphNode* CreateObjectNodeImpl(UObjectTreeGraph* InGraph, UObject* InObject) const;

protected:

	const FObjectTreeGraphClassConfig& GetObjectClassConfig(const UObjectTreeGraphNode* InNode) const;
	const FObjectTreeGraphClassConfig& GetObjectClassConfig(const UObjectTreeGraph* InGraph, UClass* InObjectClass) const;
};

/**
 * Graph action to create a new object (and corresponding graph node) of a given class.
 */
USTRUCT()
struct FObjectGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:

	/** The outer for the new object. Defaults to the root object's package. */
	UPROPERTY()
	TObjectPtr<UObject> ObjectOuter;

	/** The class of the new object. */
	UPROPERTY()
	TObjectPtr<UClass> ObjectClass;

public:

	FObjectGraphSchemaAction_NewNode();
	FObjectGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping = 0, FText InKeywords = FText());

public:

	// FEdGraphSchemaAction interface.
	static FName StaticGetTypeId() { static FName Type("FObjectGraphSchemaAction_NewNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); } 
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

protected:

	virtual UObject* CreateObject();
	virtual void AutoSetupNewNode(UObjectTreeGraphNode* NewNode, UEdGraphPin* FromPin);
};

