// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraTransitionGraphSchema.generated.h"

/**
 * Schema class for camera transition graph.
 */
UCLASS()
class UCameraTransitionGraphSchema : public UObjectTreeGraphSchema
{
	GENERATED_BODY()

protected:

	// UEdGraphSchema interface.
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	// UObjectTreeGraphSchema interface.
	virtual void FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const override;
};

/**
 * Graph action to create a new transition node.
 *
 * We need a custom action for this because we need to switch the "self" pin according to whether
 * we want an enter or exit transition.
 */
USTRUCT()
struct FCameraTransitionGraphSchemaAction_NewTransitionNode : public FObjectGraphSchemaAction_NewNode
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

	FCameraTransitionGraphSchemaAction_NewTransitionNode();
	FCameraTransitionGraphSchemaAction_NewTransitionNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping = 0, FText InKeywords = FText());

public:

	// FEdGraphSchemaAction interface.
	static FName StaticGetTypeId() { static FName Type("FCameraTransitionGraphSchemaAction_NewTransitionNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); } 

protected:

	virtual void AutoSetupNewNode(UObjectTreeGraphNode* NewNode, UEdGraphPin* FromPin) override;
};

