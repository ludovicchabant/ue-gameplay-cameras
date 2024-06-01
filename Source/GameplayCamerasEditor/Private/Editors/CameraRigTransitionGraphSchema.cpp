// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigTransitionGraphSchema.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraRigAsset.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"

#include "ScopedTransaction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransitionGraphSchema)

#define LOCTEXT_NAMESPACE "CameraRigTransitionGraphSchema"

FObjectTreeGraphConfig UCameraRigTransitionGraphSchema::BuildGraphConfig(const FCameraRigTransitionOwnerInfo& OwnerInfo)
{
	using namespace UE::Cameras;

	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;
	GraphConfig.GraphName = OwnerInfo.GraphName;
	GraphConfig.ConnectableObjectClasses.Add(OwnerInfo.TransitionOwnerClass);
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigTransition::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigTransitionCondition::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.StopAutoCollectAtObjectClasses.Add(UCameraRigAsset::StaticClass());
	GraphConfig.GraphDisplayInfo.PlainName = FText::FromName(OwnerInfo.GraphName);
	GraphConfig.GraphDisplayInfo.DisplayName = GraphConfig.GraphDisplayInfo.PlainName;
	GraphConfig.ObjectClassConfigs.Emplace(OwnerInfo.TransitionOwnerClass)
		.HasSelfPin(false)
		.OnlyAsRoot()
		.SetPropertyPinDirection(OwnerInfo.EnterTransitionsPropertyName, EGPD_Input)
		.SetPropertyPinDirection(OwnerInfo.ExitTransitionsPropertyName, EGPD_Output)
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigTransition::StaticClass())
		.SetPropertyPinDirection(GET_MEMBER_NAME_STRING_CHECKED(UCameraRigTransition, Conditions), EGPD_Input)
		.NodeTitleColor(Settings->CameraRigTransitionTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigTransitionCondition::StaticClass())
		.SelfPinDirection(EGPD_Output)
		.DefaultPropertyPinDirection(EGPD_Input)
		.StripDisplayNameSuffix(TEXT("Transition Condition"))
		.NodeTitleColor(Settings->CameraRigTransitionConditionTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UBlendCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"));
	GraphConfig.OnFormatObjectDisplayName = FOnFormatObjectDisplayName::CreateLambda(
			[](const UObject* Object, FText& InOutDisplayNameText)
			{
				if (const UCameraRigAsset* CameraRigAsset = Cast<UCameraRigAsset>(Object))
				{
					InOutDisplayNameText = FText::FromString(CameraRigAsset->GetDisplayName());
				}
			});

	return GraphConfig;
}

void UCameraRigTransitionGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
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
		TSharedRef<FCameraRigTransitionGraphSchemaAction_NewTransitionNode> EnterAction = MakeShared<FCameraRigTransitionGraphSchemaAction_NewTransitionNode>(
				LOCTEXT("TransitionsCategory", "Transitions"),
				LOCTEXT("EnterTransition", "Enter Transition"),
				LOCTEXT("EnterTransitionToolTip", "Creates a new enter transition"));
		EnterAction->TransitionType = FCameraRigTransitionGraphSchemaAction_NewTransitionNode::ETransitionType::Enter;
		ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(EnterAction.ToSharedPtr()));
	}
	
	if (bCanCreateExitTransition)
	{
		TSharedRef<FCameraRigTransitionGraphSchemaAction_NewTransitionNode> ExitAction = MakeShared<FCameraRigTransitionGraphSchemaAction_NewTransitionNode>(
				LOCTEXT("TransitionsCategory", "Transitions"),
				LOCTEXT("ExitTransition", "Exit Transition"),
				LOCTEXT("ExitTransitionToolTip", "Creates a new exit transition"),
				0);
		ExitAction->TransitionType = FCameraRigTransitionGraphSchemaAction_NewTransitionNode::ETransitionType::Exit;
		ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(ExitAction.ToSharedPtr()));
	}

	Super::GetGraphContextActions(ContextMenuBuilder);
}

void UCameraRigTransitionGraphSchema::FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const
{
	InOutClasses.Remove(UCameraRigTransition::StaticClass());
}

FCameraRigTransitionGraphSchemaAction_NewTransitionNode::FCameraRigTransitionGraphSchemaAction_NewTransitionNode()
{
	ObjectClass = UCameraRigTransition::StaticClass();
}

FCameraRigTransitionGraphSchemaAction_NewTransitionNode::FCameraRigTransitionGraphSchemaAction_NewTransitionNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
	: FObjectGraphSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InKeywords)
{
	ObjectClass = UCameraRigTransition::StaticClass();
}

void FCameraRigTransitionGraphSchemaAction_NewTransitionNode::AutoSetupNewNode(UObjectTreeGraphNode* NewNode, UEdGraphPin* FromPin)
{
	if (TransitionType == ETransitionType::Enter)
	{
		NewNode->OverrideSelfPinDirection(EGPD_Output);
	}

	FObjectGraphSchemaAction_NewNode::AutoSetupNewNode(NewNode, FromPin);
}

#undef LOCTEXT_NAMESPACE

