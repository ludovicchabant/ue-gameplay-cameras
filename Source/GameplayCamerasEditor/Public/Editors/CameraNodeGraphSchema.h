// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Compat/EditorCompat.h"
#include "Core/CameraObjectInterface.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/CameraNodeGraphPinColors.h"
#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraNodeGraphSchema.generated.h"

class UCameraNode;
class UCameraObjectInterfaceParameterBase;
class UCameraObjectInterfaceParameterGraphNode;
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
	static const FName PC_CameraVariableReference;	// A variable reference pin.
	static const FName PC_CameraContextData;		// A context data pin.

	UCameraNodeGraphSchema(const FObjectInitializer& ObjInit);

	UCameraObjectInterfaceParameterGraphNode* CreateInterfaceParameterNode(UEdGraph* InGraph, UCameraObjectInterfaceParameterBase* InterfaceParameter) const;

protected:

	// UEdGraphSchema interface.
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool OnTryCreateCustomConnection(UEdGraphPin* A, UEdGraphPin* B) const;
	virtual bool OnBreakCustomPinLinks(UEdGraphPin& TargetPin) const;
	virtual bool OnBreakSingleCustomPinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool SafeDeleteNodeFromGraph(UEdGraph* Graph, UEdGraphNode* Node) const override;

	// UObjectTreeGraphSchema interface.
	virtual void CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const override;
	virtual void OnCreateAllNodes(UObjectTreeGraph* InGraph, const FCreatedNodes& InCreatedNodes) const override;

protected:

	void BuildBaseGraphConfig(FObjectTreeGraphConfig& OutGraphConfig) const;

private:

	UEdGraphPin* FindPin(UEdGraphNode* InNode, const FName& InPinName, const FName& InPinCategoryName) const;

	UE::Cameras::FCameraNodeGraphPinColors PinColors;
};

/**
 * Graph editor action for adding a new camera rig parameter node.
 */
USTRUCT()
struct FCameraNodeGraphSchemaAction_NewInterfaceParameterNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:

	/** The new parameter's definition. */
	UPROPERTY()
	FCameraObjectInterfaceParameterDefinition ParameterDefinition;

public:

	FCameraNodeGraphSchemaAction_NewInterfaceParameterNode();
	FCameraNodeGraphSchemaAction_NewInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping = 0, FText InKeywords = FText());

public:

	// FEdGraphSchemaAction interface.
	static FName StaticGetTypeId() { static FName Type("FCameraNodeGraphSchemaAction_NewInterfaceParameterNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode = true) override;
};

USTRUCT()
struct FCameraNodeGraphSchemaAction_AddInterfaceParameterNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UCameraObjectInterfaceParameterBase> InterfaceParameter;

public:
	
	FCameraNodeGraphSchemaAction_AddInterfaceParameterNode();
	FCameraNodeGraphSchemaAction_AddInterfaceParameterNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping = 0, FText InKeywords = FText());

public:

	// FEdGraphSchemaAction interface.
	static FName StaticGetTypeId() { static FName Type("FCameraNodeGraphSchemaAction_AddInterfaceParameterNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, FPerformGraphActionLocation Location, bool bSelectNewNode = true) override;
};

