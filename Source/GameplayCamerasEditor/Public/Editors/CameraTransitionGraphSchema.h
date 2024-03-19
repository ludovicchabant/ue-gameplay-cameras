// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editors/ObjectTreeGraphSchema.h"

#include "CameraTransitionGraphSchema.generated.h"

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

