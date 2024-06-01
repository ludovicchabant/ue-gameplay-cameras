// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraRigTransitionGraphSchema.generated.h"

struct FObjectTreeGraphConfig;

/**
 * Provides information about an object that has enter/exit transitions.
 */
struct FCameraRigTransitionOwnerInfo
{
	/** The class of the object. */
	UClass* TransitionOwnerClass = nullptr;
	/** The name of the transition graph. */
	FName GraphName;
	/** The name of the property that returns the array of enter transitions. */
	FName EnterTransitionsPropertyName;
	/** The name of the property that returns the array of exit transitions. */
	FName ExitTransitionsPropertyName;
};

/**
 * Schema class for camera transition graph.
 */
UCLASS()
class UCameraRigTransitionGraphSchema : public UObjectTreeGraphSchema
{
	GENERATED_BODY()

public:

	static FObjectTreeGraphConfig BuildGraphConfig(const FCameraRigTransitionOwnerInfo& OwnerInfo);

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

