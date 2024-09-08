// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editors/CameraSharedTransitionGraphSchema.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigAsset.h"
#include "Editors/ObjectTreeGraphConfig.h"
#include "Editors/ObjectTreeGraphNode.h"
#include "GameplayCamerasEditorSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraSharedTransitionGraphSchema)

#define LOCTEXT_NAMESPACE "CameraSharedTransitionGraphSchema"

void UCameraSharedTransitionGraphSchema::OnBuildGraphConfig(FObjectTreeGraphConfig& InOutGraphConfig) const
{
	const UGameplayCamerasEditorSettings* Settings = GetDefault<UGameplayCamerasEditorSettings>();

	InOutGraphConfig.GraphName = UCameraAsset::SharedTransitionsGraphName;
	InOutGraphConfig.ConnectableObjectClasses.Add(UCameraAsset::StaticClass());
	InOutGraphConfig.GraphDisplayInfo.PlainName = LOCTEXT("NodeGraphPlainName", "SharedTransitions");
	InOutGraphConfig.GraphDisplayInfo.DisplayName = LOCTEXT("NodeGraphDisplayName", "Shared Transitions");
	InOutGraphConfig.ObjectClassConfigs.Emplace(UCameraAsset::StaticClass())
		.HasSelfPin(false)
		.OnlyAsRoot()
		.NodeTitleUsesObjectName(true)
		.NodeTitleColor(Settings->CameraAssetTitleColor);
}

ETransitionGraphContextActions UCameraSharedTransitionGraphSchema::GetTransitionGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
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
			if (DraggedPinProperty && DraggedPinProperty->GetOwnerClass()->IsChildOf<UCameraAsset>())
			{
				if (DraggedPinProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCameraAsset, EnterTransitions))
				{
					PossibleActions |= ETransitionGraphContextActions::CreateEnterTransition;
				}
				if (DraggedPinProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UCameraAsset, ExitTransitions))
				{
					PossibleActions |= ETransitionGraphContextActions::CreateExitTransition;
				}
			}
		}
	}

	return PossibleActions;
}

#undef LOCTEXT_NAMESPACE

