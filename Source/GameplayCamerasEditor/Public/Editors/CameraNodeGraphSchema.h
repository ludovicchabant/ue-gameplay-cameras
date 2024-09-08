// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraNodeGraphSchema.generated.h"

class UCameraNode;
struct FObjectTreeGraphConfig;

/**
 * Schema class for camera node graph.
 */
UCLASS()
class UCameraNodeGraphSchema : public UObjectTreeGraphSchema
{
	GENERATED_BODY()

public:

	static const FName PC_CameraParameter;			// A camera parameter pin.

	FObjectTreeGraphConfig BuildGraphConfig() const;

protected:

	// UEdGraphSchema interface.
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	// UObjectTreeGraphSchema interface.
	virtual void CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const override;
	virtual void OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const override;
	virtual void OnAddConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InNewNode) const override;
	virtual void OnRemoveConnectableObject(UObjectTreeGraph* InGraph, UObjectTreeGraphNode* InRemovedNode) const override;
	virtual bool OnApplyConnection(UEdGraphPin* A, UEdGraphPin* B, FDelayedPinActions& Actions) const override;
	virtual bool OnApplyDisconnection(UEdGraphPin* TargetPin, FDelayedPinActions& Actions, bool bIsReconnecting) const override;
	virtual bool OnApplyDisconnection(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin, FDelayedPinActions& Actions) const override;
};

/**
 * Graph editor action for adding a new camera rig parameter node.
 */
USTRUCT()
struct FCameraNodeGraphSchemaAction_NewInterfaceParameterNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:

	/** The camera node being driven by the camera rig parameter. */
	UPROPERTY()
	TObjectPtr<UCameraNode> Target;

	/** The property on the target camera node being driven by the camera rig parameter. */
	UPROPERTY()
	FName TargetPropertyName;

public:

	FCameraNodeGraphSchemaAction_NewInterfaceParameterNode();
	FCameraNodeGraphSchemaAction_NewInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping = 0, FText InKeywords = FText());

public:

	// FEdGraphSchemaAction interface.
	static FName StaticGetTypeId() { static FName Type("FCameraNodeGraphSchemaAction_NewInterfaceParameterNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); } 
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

