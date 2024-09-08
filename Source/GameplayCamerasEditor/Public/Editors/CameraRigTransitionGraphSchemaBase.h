// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraRigTransitionGraphSchemaBase.generated.h"

struct FObjectTreeGraphConfig;

/**
 * The list of transition-specific actions that can be performed on a transition graph.
 */
enum class ETransitionGraphContextActions
{
	None = 0,
	CreateEnterTransition = 1 << 0,
	CreateExitTransition = 1 << 1
};
ENUM_CLASS_FLAGS(ETransitionGraphContextActions);

/**
 * Base schema class for camera transition graph.
 */
UCLASS()
class UCameraRigTransitionGraphSchemaBase : public UObjectTreeGraphSchema
{
	GENERATED_BODY()

public:

	FObjectTreeGraphConfig BuildGraphConfig() const;

protected:

	// UEdGraphSchema interface.
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	// UObjectTreeGraphSchema interface.
	virtual void CollectAllObjects(UObjectTreeGraph* InGraph, TSet<UObject*>& OutAllObjects) const override;
	virtual void FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const override;

protected:

	// UCameraRigTransitionGraphSchemaBase interface.
	virtual void OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const {}
	virtual ETransitionGraphContextActions GetTransitionGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const { return ETransitionGraphContextActions::None; }
};

/**
 * Graph action to create a new transition node.
 *
 * We need a custom action for this because we need to switch the "self" pin according to whether
 * we want an enter or exit transition.
 */
USTRUCT()
struct FCameraRigTransitionGraphSchemaAction_NewTransitionNode : public FObjectGraphSchemaAction_NewNode
{
	GENERATED_BODY()

public:

	enum class ETransitionType
	{
		Enter,
		Exit
	};

	/** The transition type to create. */
	ETransitionType TransitionType;

public:

	FCameraRigTransitionGraphSchemaAction_NewTransitionNode();
	FCameraRigTransitionGraphSchemaAction_NewTransitionNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping = 0, FText InKeywords = FText());

public:

	// FEdGraphSchemaAction interface.
	static FName StaticGetTypeId() { static FName Type("FCameraRigTransitionGraphSchemaAction_NewTransitionNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); } 

protected:

	virtual void AutoSetupNewNode(UObjectTreeGraphNode* NewNode, UEdGraphPin* FromPin) override;
};

