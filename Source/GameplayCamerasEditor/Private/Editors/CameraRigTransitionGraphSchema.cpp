// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraRigTransitionGraphSchema.h"

#include "Core/CameraRigAsset.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigTransitionGraphSchema)

#define LOCTEXT_NAMESPACE "CameraRigTransitionGraphSchema"

void UCameraRigTransitionGraphSchema::OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	InOutGraphConfig.GraphName = UCameraRigAsset::TransitionsGraphName;
	InOutGraphConfig.ConnectableObjectClasses.Add(UCameraRigAsset::StaticClass());
	InOutGraphConfig.GraphDisplayInfo.PlainName = LOCTEXT("NodeGraphPlainName", "Transitions");
	InOutGraphConfig.GraphDisplayInfo.DisplayName = LOCTEXT("NodeGraphDisplayName", "Transitions");
	InOutGraphConfig.ObjectClassConfigs.Emplace(UCameraRigAsset::StaticClass())
		.HasSelfPin(false)
		.OnlyAsRoot()
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraRigAssetTitleColor);
}

ETransitionGraphContextActions UCameraRigTransitionGraphSchema::GetTransitionGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	ETransitionGraphContextActions PossibleActions = ETransitionGraphContextActions::None;

	if (const UEdGraphPin* DraggedPin = ContextMenuBuilder.FromPin)
	{
		// If we are creating a node from dragging a pin into an empty space, figure out which transition
		// we can create based on the direction of the dragged pin.
		UObjectTreeGraphNode* OwningNode = Cast<UObjectTreeGraphNode>(DraggedPin->GetOwningNode());
		if (OwningNode)
		{
			FProperty* DraggedPinProperty = OwningNode->GetPropertyForPin(DraggedPin);
			if (DraggedPinProperty && DraggedPinProperty->GetOwnerClass()->IsChildOf<UCameraRigAsset>())
			{
				if (DraggedPinProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCameraRigAsset, EnterTransitions))
				{
					PossibleActions |= ETransitionGraphContextActions::CreateEnterTransition;
				}
				if (DraggedPinProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCameraRigAsset, ExitTransitions))
				{
					PossibleActions |= ETransitionGraphContextActions::CreateExitTransition;
				}
			}
		}
	}

	return PossibleActions;
}

#undef LOCTEXT_NAMESPACE

