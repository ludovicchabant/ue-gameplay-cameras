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

UCLASS()
class UObjectTreeGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:

	UObjectTreeGraphNode(const FObjectInitializer& ObjInit);

	void Initialize(UObject* InObject);

	UObject* GetObject() const { return Object.Get(); }
	void GetAllConnectableProperties(TArray<FProperty*>& OutProperties) const;

	UEdGraphPin* GetSelfPin() const;
	void OverrideSelfPinDirection(EEdGraphPinDirection Direction);

	UEdGraphPin* GetPinForProperty(FObjectProperty* InProperty) const;
	UEdGraphPin* GetPinForProperty(FArrayProperty* InProperty, int32 Index) const;
	UEdGraphPin* GetPinForPropertyNewItem(FArrayProperty* InProperty, bool bCreateNew);
	FProperty* GetPropertyForPin(const UEdGraphPin* InPin) const;
	UClass* GetConnectedObjectClassForPin(const UEdGraphPin* InPin) const;
	int32 GetIndexOfArrayPin(const UEdGraphPin* InPin) const;

	// UEdGraphNode interface.
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FLinearColor GetNodeBodyTintColor() const override;
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
	virtual void OnGraphNodeMoved();
	virtual void OnDoubleClicked() {}

public:

	// Internal API.
	void PostDuplicateObject(const TMap<UEdGraphNode*, UEdGraphNode*>& NodeMap);
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
	TObjectPtr<UObject> Object;

	UPROPERTY()
	TEnumAsByte<EEdGraphPinDirection> SelfPinDirectionOverride;

	UPROPERTY()
	bool bOverrideSelfPinDirection;
};

