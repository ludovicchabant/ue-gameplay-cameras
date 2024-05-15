// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraTransitionGraphSchema.h"

#include "Core/CameraRigAsset.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/ObjectTreeGraphNode.h"

#include "ScopedTransaction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraTransitionGraphSchema)

#define LOCTEXT_NAMESPACE "CameraTransitionGraphSchema"

void UCameraTransitionGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Start with being able to create both enter and exit transitions.
	bool bCanCreateEnterTransition = true;
	bool bCanCreateExitTransition = true;
	if (const UEdGraphPin* DraggedPin = ContextMenuBuilder.FromPin)
	{
		// If we are creating a node from dragging a pin into an empty space, figure out which transition
		// we can create based on the direction of the dragged pin.
		bCanCreateEnterTransition = false;
		bCanCreateExitTransition = false;

		UObjectTreeGraphNode* OwningNode = Cast<UObjectTreeGraphNode>(DraggedPin->GetOwningNode());
		if (OwningNode)
		{
			FProperty* DraggedPinProperty = OwningNode->GetPropertyForPin(DraggedPin);
			if (DraggedPinProperty && DraggedPinProperty->GetOwnerClass()->IsChildOf<UCameraRigAsset>())
			{
				bCanCreateEnterTransition = (DraggedPinProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCameraRigAsset, EnterTransitions));
				bCanCreateExitTransition = (DraggedPinProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCameraRigAsset, ExitTransitions));
			}
		}
	}

	if (bCanCreateEnterTransition)
	{
		TSharedRef<FCameraTransitionGraphSchemaAction_NewTransitionNode> EnterAction = MakeShared<FCameraTransitionGraphSchemaAction_NewTransitionNode>(
				LOCTEXT("TransitionsCategory", "Transitions"),
				LOCTEXT("EnterTransition", "Enter Transition"),
				LOCTEXT("EnterTransitionToolTip", "Creates a new enter transition"));
		EnterAction->TransitionType = FCameraTransitionGraphSchemaAction_NewTransitionNode::ETransitionType::Enter;
		ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(EnterAction.ToSharedPtr()));
	}
	
	if (bCanCreateExitTransition)
	{
		TSharedRef<FCameraTransitionGraphSchemaAction_NewTransitionNode> ExitAction = MakeShared<FCameraTransitionGraphSchemaAction_NewTransitionNode>(
				LOCTEXT("TransitionsCategory", "Transitions"),
				LOCTEXT("ExitTransition", "Exit Transition"),
				LOCTEXT("ExitTransitionToolTip", "Creates a new exit transition"),
				0);
		ExitAction->TransitionType = FCameraTransitionGraphSchemaAction_NewTransitionNode::ETransitionType::Exit;
		ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(ExitAction.ToSharedPtr()));
	}

	Super::GetGraphContextActions(ContextMenuBuilder);
}

void UCameraTransitionGraphSchema::FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const
{
	InOutClasses.Remove(UCameraRigTransition::StaticClass());
}

FCameraTransitionGraphSchemaAction_NewTransitionNode::FCameraTransitionGraphSchemaAction_NewTransitionNode()
{
	ObjectClass = UCameraRigTransition::StaticClass();
}

FCameraTransitionGraphSchemaAction_NewTransitionNode::FCameraTransitionGraphSchemaAction_NewTransitionNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FObjectGraphSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
	ObjectClass = UCameraRigTransition::StaticClass();
}

void FCameraTransitionGraphSchemaAction_NewTransitionNode::AutoSetupNewNode(UObjectTreeGraphNode* NewNode, UEdGraphPin* FromPin)
{
	if (TransitionType == ETransitionType::Enter)
	{
		NewNode->OverrideSelfPinDirection(EGPD_Output);
	}

	FObjectGraphSchemaAction_NewNode::AutoSetupNewNode(NewNode, FromPin);
}

#undef LOCTEXT_NAMESPACE

