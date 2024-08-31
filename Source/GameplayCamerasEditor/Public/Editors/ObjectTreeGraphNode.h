// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "EdGraph/EdGraphNode.h"
#include "UObject/UObjectGlobals.h"

#include "ObjectTreeGraphNode.generated.h"

class FObjectProperty;
class UEdGraphPin;
class UObjectTreeGraph;
struct FObjectTreeGraphConfig;
struct FObjectTreeGraphClassConfig;

/**
 * A graph node that represents an object inside an object tree graph.
 */
UCLASS()
class UObjectTreeGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:

	/** Creates a new graph node. */
	UObjectTreeGraphNode(const FObjectInitializer& ObjInit);

	/** Initializes this graph node for the given object. */
	void Initialize(UObject* InObject);

	/** Gets the underlying object represented by this graph node. */
	UObject* GetObject() const { return WeakObject.Get(); }
	/** Gets whether we have a valid underlying object, and that it's a type of ObjectClass. */
	template<typename ObjectClass> bool IsObjectA() const;
	/** Gets the underlying object as a point to the given sub-class. */
	template<typename ObjectClass> ObjectClass* CastObject() const;

	/** Gets all connectable properties on the underlying object. */
	void GetAllConnectableProperties(TArray<FProperty*>& OutProperties) const;

	/** Finds the self pin that represents the underlying object itself. */
	UEdGraphPin* GetSelfPin() const;
	/** Changes the direction of the self pin. */
	void OverrideSelfPinDirection(EEdGraphPinDirection Direction);

	/** Finds the pin for the given object property. */
	UEdGraphPin* GetPinForProperty(FObjectProperty* InProperty) const;
	/** Finds the pin for the given item in an array property. */
	UEdGraphPin* GetPinForProperty(FArrayProperty* InProperty, int32 Index) const;
	/** Finds the extra free pin used to add new items in an array property. */
	UEdGraphPin* GetPinForPropertyNewItem(FArrayProperty* InProperty, bool bCreateNew);
	/** Gets the underlying property represented by the given pin. */
	FProperty* GetPropertyForPin(const UEdGraphPin* InPin) const;
	/** Gets the type of object that can connect to the given pin. */
	UClass* GetConnectedObjectClassForPin(const UEdGraphPin* InPin) const;
	/** Gets the index of the given pin's underlying value inside an array property. */
	int32 GetIndexOfArrayPin(const UEdGraphPin* InPin) const;

public:

	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FLinearColor GetNodeBodyTintColor() const override;
	virtual FText GetTooltipText() const override;
	virtual void AllocateDefaultPins() override;
	virtual void PostPlacedNewNode() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void NodeConnectionListChanged() override;
	virtual void OnPinRemoved(UEdGraphPin* InRemovedPin) override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual bool GetCanRenameNode() const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual bool CanDuplicateNode() const override;
	virtual bool CanUserDeleteNode() const override;
	virtual bool SupportsCommentBubble() const override;
	virtual void OnUpdateCommentText(const FString& NewComment) override;

	// UObjectTreeGraphNode interface.
	virtual void OnGraphNodeMoved(bool bMarkDirty);
	virtual void OnDoubleClicked() {}

public:

	// Internal API.
	void CreateNewItemPin(FArrayProperty& InArrayProperty);
	void CreateNewItemPin(UEdGraphPin* InParentArrayPin);
	void RemoveItemPin(UEdGraphPin* InItemPin);
	void RefreshArrayPropertyPinNames();

protected:

	struct FNodeContext
	{
		UClass* ObjectClass;
		UObjectTreeGraph* Graph;
		const FObjectTreeGraphConfig& GraphConfig;
		const FObjectTreeGraphClassConfig& ObjectClassConfig;
	};

	FNodeContext GetNodeContext() const;
	const FObjectTreeGraphClassConfig& GetObjectClassConfig() const;

private:

	UPROPERTY()
	TWeakObjectPtr<UObject> WeakObject;

	UPROPERTY()
	TEnumAsByte<EEdGraphPinDirection> SelfPinDirectionOverride;

	UPROPERTY()
	bool bOverrideSelfPinDirection;
};

template<typename ObjectClass>
bool UObjectTreeGraphNode::IsObjectA() const
{
	if (UObject* Object = WeakObject.Get())
	{
		return Object->IsA<ObjectClass>();
	}
	return false;
}

template<typename ObjectClass> 
ObjectClass* UObjectTreeGraphNode::CastObject() const
{
	if (UObject* Object = WeakObject.Get())
	{
		return Cast<ObjectClass>(Object);
	}
	return nullptr;
}

