// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigTransitionGraphSchemaBase.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraRigTransition.h"
#include "EdGraph/EdGraphPin.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransitionGraphSchemaBase)

#define LOCTEXT_NAMESPACE "CameraRigTransitionGraphSchemaBase"

FObjectTreeGraphConfig UCameraRigTransitionGraphSchemaBase::BuildGraphConfig() const
{
	using namespace UE::Cameras;

	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	FObjectTreeGraphConfig GraphConfig;
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigTransition::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UCameraRigTransitionCondition::StaticClass());
	GraphConfig.ConnectableObjectClasses.Add(UBlendCameraNode::StaticClass());
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigTransition::StaticClass())
		.NodeTitleColor(Settings->CameraRigTransitionTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UCameraRigTransitionCondition::StaticClass())
		.StripDisplayNameSuffix(TEXT("Transition Condition"))
		.NodeTitleColor(Settings->CameraRigTransitionConditionTitleColor);
	GraphConfig.ObjectClassConfigs.Emplace(UBlendCameraNode::StaticClass())
		.StripDisplayNameSuffix(TEXT("Camera Node"))
		.CreateCategoryMetaData(TEXT("CameraNodeCategories"));

	OnBuildGraphConfig(GraphConfig);

	return GraphConfig;
}

void UCameraRigTransitionGraphSchemaBase::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	ETransitionGraphContextActions PossibleActions = GetTransitionGraphContextActions(ContextMenuBuilder);

	if (EnumHasAnyFlags(PossibleActions, ETransitionGraphContextActions::CreateEnterTransition))
	{
		TSharedRef<FCameraRigTransitionGraphSchemaAction_NewTransitionNode> EnterAction = MakeShared<FCameraRigTransitionGraphSchemaAction_NewTransitionNode>(
				LOCTEXT("TransitionsCategory", "Transitions"),
				LOCTEXT("EnterTransition", "Enter Transition"),
				LOCTEXT("EnterTransitionToolTip", "Creates a new enter transition"));
		EnterAction->TransitionType = FCameraRigTransitionGraphSchemaAction_NewTransitionNode::ETransitionType::Enter;
		ContextMenuBuilder.AddAction(StaticCastSharedPtr<FEdGraphSchemaAction>(EnterAction.ToSharedPtr()));
	}
	
	if (EnumHasAnyFlags(PossibleActions, ETransitionGraphContextActions::CreateExitTransition))
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

void UCameraRigTransitionGraphSchemaBase::FilterGraphContextPlaceableClasses(TArray<UClass*>& InOutClasses) const
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

