// Copyright Epic Games, Inc. All Rights Reserved.

#include "Services/CameraActionService.h"

#include "Containers/AllowShrinking.h"
#include "Core/CameraRigAsset.h"  // IWYU pragma: keep
#include "Core/CameraSystemEvaluator.h"
#include "Core/RootCameraNode.h"
#include "Core/RootCameraNodeCameraRigEvent.h"
#include "Debug/CameraDebugBlock.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Services/CameraAction.h"
#include "Services/CameraActionEvaluator.h"
#include "Services/CameraActionScope.h"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_EVALUATION_SERVICE(FCameraActionService)

FCameraActionService::~FCameraActionService()
{
	// If we are destroying the owning evaluator while a cloning camera action is running,
	// we need to clean-up our delegate for root node events.
	if (RootNodeEvaluator && NumCloningActions > 0)
	{
		RootNodeEvaluator->OnCameraRigEvent().RemoveAll(this);
	}
}

void FCameraActionService::OnInitialize(const FCameraEvaluationServiceInitializeParams& Params)
{
	SetEvaluationServiceFlags(ECameraEvaluationServiceFlags::NeedsPostUpdate);

	RootNodeEvaluator = Params.Evaluator->GetRootNodeEvaluator();
}

void FCameraActionService::OnPostUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult)
{
	if (Params.EvaluationType == ECameraNodeEvaluationType::Standard)
	{
		CleanUpActions();
	}
}

FCameraActionInstanceID FCameraActionService::StartAction(const UCameraAction* CameraAction)
{
	if (!CameraAction)
	{
		return FCameraActionInstanceID();
	}
	
	TSharedPtr<FCameraActionScope> ActionScope = RootNodeEvaluator->GetActiveCameraRigActionScope(true);
	if (!ensure(ActionScope))
	{
		return FCameraActionInstanceID();
	}
	
	FCameraActionInstanceID NewInstanceID = ActionScope->StartAction(CameraAction);
	if (!ensure(NewInstanceID.IsValid()))
	{
		return FCameraActionInstanceID();
	}

	FActionSetInfo& NewActionSet = ActionSets.Emplace_GetRef();
	NewActionSet.Data = CameraAction;
	NewActionSet.Actions.Add(FActionInfo{ ActionScope, NewInstanceID });
	NewActionSet.InstanceID = FCameraActionInstanceID(NextActionSetID++);

	if (CameraAction->bPropagateToNewCameraRigs)
	{
		AddCloningAction();
	}
	
	return NewActionSet.InstanceID;
}

bool FCameraActionService::IsActionRunning(const FCameraActionInstanceID InInstanceID) const
{
	if (InInstanceID.IsValid())
	{
		return ActionSets.ContainsByPredicate([InInstanceID](const FActionSetInfo& Item)
				{
					return Item.InstanceID == InInstanceID;
				});
	}
	return false;
}

bool FCameraActionService::StopAction(const FCameraActionInstanceID InInstanceID)
{
	const int32 Index = ActionSets.IndexOfByPredicate([InInstanceID](const FActionSetInfo& Item)
			{
				return Item.InstanceID == InInstanceID;
			});
	if (Index != INDEX_NONE)
	{
		FActionSetInfo& ActionSet = ActionSets[Index];
		for (FActionInfo& Action : ActionSet.Actions)
		{
			TSharedPtr<FCameraActionScope> ActionScope = Action.ActionScope.Pin();
			if (ensure(ActionScope && Action.ActionID.IsValid()))
			{
				const bool bStopped = ActionScope->StopAction(Action.ActionID);
				ensure(bStopped);
			}
		}
		if (ActionSet.Data->bPropagateToNewCameraRigs)
		{
			RemoveCloningAction();
		}
		ActionSets.RemoveAt(Index);
		return true;
	}
	return false;
}

bool FCameraActionService::StopAllActionsOfClass(TSubclassOf<UCameraAction> InActionClass)
{
	bool bAnyStopped = false;
	for (auto It = ActionSets.CreateIterator(); It; ++It)
	{
		FActionSetInfo& ActionSet(*It);
		if (ActionSet.Data && ActionSet.Data->IsA(InActionClass))
		{
			for (FActionInfo& Action : ActionSet.Actions)
			{
				if (TSharedPtr<FCameraActionScope> ActionScope = Action.ActionScope.Pin())
				{
					const bool bStopped = ActionScope->StopAction(Action.ActionID);
					ensure(bStopped);
					bAnyStopped = true;
				}
			}
			if (ActionSet.Data->bPropagateToNewCameraRigs)
			{
				RemoveCloningAction();
			}
			It.RemoveCurrent();
		}
	}
	return bAnyStopped;
}

void FCameraActionService::OnRootCameraNodeCameraRigEvent(const FRootCameraNodeCameraRigEvent& InEvent)
{
	if (InEvent.EventType == ERootCameraNodeCameraRigEventType::Activated && InEvent.EventLayer == ECameraRigLayer::Main)
	{
		TSharedPtr<FCameraActionScope> ActionScope = RootNodeEvaluator->GetActiveCameraRigActionScope(true);
		if (ensure(ActionScope))
		{
			int32 NumCloned = 0;
			for (FActionSetInfo& ActionSet : ActionSets)
			{
				if (ensure(ActionSet.Data) && ActionSet.Data->bPropagateToNewCameraRigs)
				{
					CloneAction(ActionSet, ActionScope.ToSharedRef());
					++NumCloned;
				}
			}
			ensureMsgf(
					NumCloned > 0, 
					TEXT("No actions found to clone: we are still registered with root camera rig events for nothing"));
		}
	}
}

void FCameraActionService::AddCloningAction()
{
	const bool bRegisterEvent = (NumCloningActions == 0);
	++NumCloningActions;
	if (bRegisterEvent)
	{
		RootNodeEvaluator->OnCameraRigEvent().AddRaw(this, &FCameraActionService::OnRootCameraNodeCameraRigEvent);
	}
}

void FCameraActionService::RemoveCloningAction()
{
	ensure(NumCloningActions > 0);
	--NumCloningActions;
	if (NumCloningActions == 0)
	{
		RootNodeEvaluator->OnCameraRigEvent().RemoveAll(this);
	}
}

void FCameraActionService::CloneAction(FActionSetInfo& ActionSet, TSharedRef<FCameraActionScope> NewActionScope)
{
	if (!ensure(ActionSet.Actions.Num() > 0))
	{
		return;
	}

	const FActionInfo& ActiveAction = ActionSet.Actions.Last();
	TSharedPtr<FCameraActionScope> ActiveActionScope = ActiveAction.ActionScope.Pin();
	if (!ensure(ActiveActionScope))
	{
		return;
	}

	FCameraActionEvaluator* ActiveEvaluator = ActiveActionScope->GetAction(ActiveAction.ActionID);
	if (!ensure(ActiveEvaluator))
	{
		return;
	}

	TArray<uint8> EvaluatorSnapshot;
	FCameraActionEvaluatorSerializeParams Params;
	{
		FMemoryWriter Writer(EvaluatorSnapshot);
		ActiveEvaluator->Serialize(Params, Writer);
	}

	const FCameraActionInstanceID NewInstanceID = NewActionScope->StartAction(ActionSet.Data);
	FCameraActionEvaluator* NewEvaluator = NewActionScope->GetAction(NewInstanceID);
	if (ensure(NewEvaluator))
	{
		FMemoryReader Reader(EvaluatorSnapshot);
		NewEvaluator->Serialize(Params, Reader);

		ActionSet.Actions.Add(FActionInfo{ NewActionScope, NewInstanceID });
	}
}

void FCameraActionService::CleanUpActions()
{
	for (auto SetIt = ActionSets.CreateIterator(); SetIt; ++SetIt)
	{
		FActionSetInfo& ActionSet(*SetIt);
		for (auto ActionIt = ActionSet.Actions.CreateIterator(); ActionIt; ++ActionIt)
		{
			// Remove finished actions, whether they finished on their own, or were ended by their action scope 
			// being discarded.
			FActionInfo& ActionInfo(*ActionIt);
			if (TSharedPtr<FCameraActionScope> ActionScope = ActionInfo.ActionScope.Pin())
			{
				if (!ActionScope->IsActionRunning(ActionInfo.ActionID))
				{
					ActionIt.RemoveCurrent(EAllowShrinking::No);
				}
			}
			else
			{
				ActionIt.RemoveCurrent(EAllowShrinking::No);
			}
		}
		if (ActionSet.Actions.IsEmpty())
		{
			if (ActionSet.Data->bPropagateToNewCameraRigs)
			{
				RemoveCloningAction();
			}
			SetIt.RemoveCurrent(EAllowShrinking::No);
		}
	}
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

class FCameraActionServiceDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(, FCameraActionServiceDebugBlock)

public:

	FCameraActionServiceDebugBlock() = default;
	FCameraActionServiceDebugBlock(const FCameraActionService& InService);

protected:

	// FCameraDebugBlock interface.
	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;
	virtual void OnSerialize(FArchive& Ar) override;

private:

	struct FActionDebugInfo
	{
		FString ScopeCameraRigName;
		FCameraActionInstanceID ActionID;
	};

	struct FActionSetDebugInfo
	{
		FString ActionName;
		TArray<FActionDebugInfo> Actions;
		FCameraActionInstanceID InstanceID;
	};

	friend FArchive& operator<< (FArchive&, FActionDebugInfo&);
	friend FArchive& operator<< (FArchive&, FActionSetDebugInfo&);

	TArray<FActionSetDebugInfo> ActionSets;
};

FArchive& operator<< (FArchive& Ar, FCameraActionServiceDebugBlock::FActionDebugInfo& Item)
{
	Ar << Item.ScopeCameraRigName;
	Ar << Item.ActionID;
	return Ar;
}

FArchive& operator<< (FArchive& Ar, FCameraActionServiceDebugBlock::FActionSetDebugInfo& Item)
{
	Ar << Item.ActionName;
	Ar << Item.Actions;
	Ar << Item.InstanceID;
	return Ar;
}

UE_DEFINE_CAMERA_DEBUG_BLOCK(FCameraActionServiceDebugBlock)

FCameraActionServiceDebugBlock::FCameraActionServiceDebugBlock(const FCameraActionService& InService)
{
	for (const FCameraActionService::FActionSetInfo& ActionSet : InService.ActionSets)
	{
		FActionSetDebugInfo& ActionSetDebugInfo = ActionSets.Emplace_GetRef();
		ActionSetDebugInfo.ActionName = GetNameSafe(ActionSet.Data);
		ActionSetDebugInfo.InstanceID = ActionSet.InstanceID;

		for (const FCameraActionService::FActionInfo& Action : ActionSet.Actions)
		{
			FActionDebugInfo& ActionDebugInfo = ActionSetDebugInfo.Actions.Emplace_GetRef();
			ActionDebugInfo.ActionID = Action.ActionID;
			if (TSharedPtr<const FCameraActionScope> ActionScope = Action.ActionScope.Pin())
			{
				ActionDebugInfo.ScopeCameraRigName = GetNameSafe(ActionScope->GetCameraRig());
			}
			else
			{
				ActionDebugInfo.ScopeCameraRigName = TEXT("None");
			}
		}
	}
}

void FCameraActionServiceDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	for (const FActionSetDebugInfo& ActionSet : ActionSets)
	{
		Renderer.AddText(TEXT("Action Set: {cam_notice}%s{cam_default}\n"), *ActionSet.ActionName);
		Renderer.AddIndent();
		{
			for (const FActionDebugInfo& Action : ActionSet.Actions)
			{
				Renderer.AddText(TEXT("Action Scope {cam_notice2}%s{cam_default}\n"), *Action.ScopeCameraRigName);
			}
		}
		Renderer.RemoveIndent();
	}
}

void FCameraActionServiceDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << ActionSets;
}

void FCameraActionService::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	Builder.AttachDebugBlock<FCameraActionServiceDebugBlock>(*this);
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

} // namespace UE::Cameras

