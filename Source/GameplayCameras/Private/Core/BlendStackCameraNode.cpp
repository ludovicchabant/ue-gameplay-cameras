// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/BlendStackRootCameraNode.h"
#include "Core/CameraAsset.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraMode.h"
#include "Core/CameraSystemEvaluator.h"
#include "IGameplayCamerasModule.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "Modules/ModuleManager.h"
#include "Nodes/Blends/PopBlendCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackCameraNode)

FCameraNodeEvaluatorPtr UBlendStackCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	return Builder.BuildEvaluator<FBlendStackCameraNodeEvaluator>();
}

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendStackCameraNodeEvaluator)

void FBlendStackCameraNodeEvaluator::Push(const FBlendStackCameraPushParams& Params)
{
	if (!Entries.IsEmpty())
	{
		// Don't push anything if what is being requested is already the
		// active camera mode.
		const FCameraModeEntry& TopEntry(Entries.Top());
		if (!TopEntry.bIsFrozen
				&& TopEntry.CameraMode == Params.CameraMode
				&& TopEntry.EvaluationContext == Params.EvaluationContext)
		{
			return;
		}
	}

	// Create the new root node to wrap the new camera mode's root node, and the specific
	// blend node for this transition.
	// We need to const-cast here to be able to use our own blend stack node as the outer
	// of the new node.
	UObject* Outer = const_cast<UObject*>((UObject*)GetCameraNode());
	UBlendStackRootCameraNode* EntryRootNode = NewObject<UBlendStackRootCameraNode>(Outer, NAME_None);
	{
		UCameraNode* ModeRootNode = Params.CameraMode->RootNode;
		EntryRootNode->RootNode = ModeRootNode;

		// Find a transition and use its blend. If no transition is found,
		// make a camera cut transition.
		UBlendCameraNode* ModeBlend = nullptr;
		if (const FCameraModeTransition* Transition = FindTransition(Params))
		{
			ModeBlend = Transition->Blend;
		}
		else
		{
			ModeBlend = NewObject<UPopBlendCameraNode>(EntryRootNode, NAME_None);
		}
		EntryRootNode->Blend = ModeBlend;
	}

	// Make the new stack entry, and use its storage buffer to build the tree of evaluators.
	FCameraModeEntry NewEntry;

	FCameraNodeEvaluatorTreeBuilderParams BuildParams;
	BuildParams.RootCameraNode = EntryRootNode;
	BuildParams.Evaluator = Params.Evaluator;
	BuildParams.EvaluationContext = Params.EvaluationContext;
	FCameraNodeEvaluator* RootEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);

	NewEntry.EvaluationContext = Params.EvaluationContext;
	NewEntry.CameraMode = Params.CameraMode;
	NewEntry.RootNode = EntryRootNode;
	NewEntry.RootEvaluator = RootEvaluator->CastThisChecked<FBlendStackRootCameraNodeEvaluator>();
	NewEntry.bIsFirstFrame = true;

#if WITH_EDITOR
	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
	Params.CameraMode->GatherPackages(NewEntry.ListenedPackages);
	for (const UPackage* ListenPackage : NewEntry.ListenedPackages)
	{
		LiveEditManager->AddListener(ListenPackage, this);
	}
#endif  // WITH_EDITOR

	// Important: we need to move the new entry here because copying evaluator storage
	// is disabled.
	Entries.Add(MoveTemp(NewEntry));
}

FCameraNodeEvaluatorChildrenView FBlendStackCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView View;
	for (FCameraModeEntry& Entry : Entries)
	{
		View.Add(Entry.RootEvaluator);
	}
	return View;
}

void FBlendStackCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	OwningEvaluator = Params.Evaluator;
}

void FBlendStackCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UBlendStackCameraNode* BlendStackNode = GetCameraNodeAs<UBlendStackCameraNode>();

	// Start by evaluating all the root nodes in the stack.
	for (FCameraModeEntry& Entry : Entries)
	{
		const UCameraEvaluationContext* CurContext = Entry.EvaluationContext.Get();
		if (UNLIKELY(CurContext == nullptr))
		{
			Entry.Result.bIsValid = false;
			continue;
		}

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = CurContext;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		if (!Entry.bIsFrozen)
		{
			// If the context in which this camera mode runs doesn't have a valid result,
			// skip it.
			const FCameraNodeEvaluationResult& ContextResult(CurContext->GetInitialResult());
			if (UNLIKELY(!ContextResult.bIsValid))
			{
				CurResult.bIsValid = false;
				continue;
			}

			// Start with the input given to us.
			CurResult.CameraPose = OutResult.CameraPose;
			CurResult.CameraPose.ClearAllChangedFlags();

			// Override it with whatever the evaluation context has set on its result.
			CurResult.CameraPose.OverrideChanged(ContextResult.CameraPose);
			CurResult.bIsCameraCut = OutResult.bIsCameraCut || ContextResult.bIsCameraCut;
			CurResult.bIsValid = true;

			// Run the camera mode!
			Entry.RootEvaluator->Run(CurParams, CurResult);
		}
		else
		{
			// Only evaluate the blend via the root node.
			Entry.RootEvaluator->Run(CurParams, CurResult);
		}
	}

	// Now blend all the results, keeping track of blends that have reached 100% so
	// that we can remove any camera modes below (since the would have been completely
	// blended out by that).
	int32 EntryIndex = 0;
	int32 PopEntriesBelow = INDEX_NONE;
	for (FCameraModeEntry& Entry : Entries)
	{
		FCameraNodeEvaluationResult& CurResult(Entry.Result);
		if (UNLIKELY(!CurResult.bIsValid))
		{
			continue;
		}

		const FCameraPoseFlags ChangedFlags(CurResult.CameraPose.GetChangedFlags());

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = Entry.EvaluationContext.Get();
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;
		FCameraNodeBlendParams BlendParams(CurParams, CurResult);

		FCameraNodeBlendResult BlendResult(OutResult);

		FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
		if (EntryBlendEvaluator)
		{
			EntryBlendEvaluator->BlendResults(BlendParams, BlendResult);

			if (BlendResult.bIsBlendFull && BlendResult.bIsBlendFinished)
			{
				PopEntriesBelow = EntryIndex;
			}
		}
		else
		{
			OutResult.CameraPose.OverrideChanged(CurResult.CameraPose);

			PopEntriesBelow = EntryIndex;
		}

		CurResult.CameraPose.SetChangedFlags(ChangedFlags);
		
		++EntryIndex;
	}

	// Pop out camera modes that have been blended out.
	if (BlendStackNode->bAutoPop && PopEntriesBelow != INDEX_NONE)
	{
#if WITH_EDITOR
		IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
		TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
#endif  // WITH_EDITOR

		for (int32 Index = 0; Index < PopEntriesBelow; ++Index)
		{
#if WITH_EDITOR
			const FCameraModeEntry& FirstEntry = Entries[0];
			for (const UPackage* ListenPackage : FirstEntry.ListenedPackages)
			{
				LiveEditManager->RemoveListener(ListenPackage, this);
			}
#endif  // WITH_EDITOR

			Entries.RemoveAt(0);
		}
	}

	// Reset first frame flags.
	for (FCameraModeEntry& Entry : Entries)
	{
		Entry.bIsFirstFrame = false;
	}
}

const FCameraModeTransition* FBlendStackCameraNodeEvaluator::FindTransition(const FBlendStackCameraPushParams& Params) const
{
	const UBlendStackCameraNode* BlendStackNode = GetCameraNodeAs<UBlendStackCameraNode>();

	const UCameraEvaluationContext* ToContext = Params.EvaluationContext.Get();
	const UCameraAsset* ToCameraAsset = ToContext ? ToContext->GetCameraAsset() : nullptr;
	const UCameraMode* ToCameraMode = Params.CameraMode;

	// Find a transition that works for blending towards ToCameraMode.
	// If the stack isn't empty, we need to find a transition that works between the previous and 
	// next camera modes. If the stack is empty, we blend the new camera mode in from nothing if
	// appropriate.
	if (!Entries.IsEmpty())
	{
		const FCameraModeTransition* TransitionToUse = nullptr;

		// Start by looking at exit transitions on the last active (top) camera mode.
		const FCameraModeEntry& TopEntry = Entries.Top();

		const UCameraEvaluationContext* FromContext = TopEntry.EvaluationContext.Get();
		const UCameraAsset* FromCameraAsset = FromContext ? FromContext->GetCameraAsset() : nullptr;
		const UCameraMode* FromCameraMode = TopEntry.CameraMode;

		if (!TopEntry.bIsFrozen)
		{
			// Look for exit transitions on the last active camera mode itself.
			TransitionToUse = FindTransition(
					FromCameraMode->ExitTransitions,
					FromCameraMode, FromCameraAsset, false,
					ToCameraMode, ToCameraAsset);
			if (TransitionToUse)
			{
				return TransitionToUse;
			}
			
			// Look for exit transitions on its parent camera asset.
			if (FromCameraAsset)
			{
				TransitionToUse = FindTransition(
						FromCameraAsset->ExitTransitions,
						FromCameraMode, FromCameraAsset, false,
						ToCameraMode, ToCameraAsset);
				if (TransitionToUse)
				{
					return TransitionToUse;
				}
			}
		}

		// Now look at enter transitions on the new camera mode.
		TransitionToUse = FindTransition(
				ToCameraMode->EnterTransitions,
				FromCameraMode, FromCameraAsset, TopEntry.bIsFrozen,
				ToCameraMode, ToCameraAsset);
		if (TransitionToUse)
		{
			return TransitionToUse;
		}

		// Look at enter transitions on its parent camera asset.
		if (ToCameraAsset)
		{
			TransitionToUse = FindTransition(
					ToCameraAsset->EnterTransitions,
					FromCameraMode, FromCameraAsset, TopEntry.bIsFrozen,
					ToCameraMode, ToCameraAsset);
			if (TransitionToUse)
			{
				return TransitionToUse;
			}
		}
	}
	else if (BlendStackNode->bBlendFirstCameraMode)
	{
		return FindTransition(
				ToCameraMode->EnterTransitions,
				nullptr, nullptr, false,
				ToCameraMode, ToCameraAsset);
	}

	return nullptr;
}

const FCameraModeTransition* FBlendStackCameraNodeEvaluator::FindTransition(
			TArrayView<const FCameraModeTransition> Transitions, 
			const UCameraMode* FromCameraMode, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraMode* ToCameraMode, const UCameraAsset* ToCameraAsset) const
{
	FCameraModeTransitionConditionMatchParams MatchParams;
	MatchParams.FromCameraMode = FromCameraMode;
	MatchParams.FromCameraAsset = FromCameraAsset;
	MatchParams.ToCameraMode = ToCameraMode;
	MatchParams.ToCameraAsset = ToCameraAsset;

	// The transition should be used if all its conditions pass.
	for (const FCameraModeTransition& Transition : Transitions)
	{
		bool bConditionsPass = true;
		for (const UCameraModeTransitionCondition* Condition : Transition.Conditions)
		{
			if (!Condition->TransitionMatches(MatchParams))
			{
				bConditionsPass = false;
				break;
			}
		}

		if (bConditionsPass)
		{
			return &Transition;
		}
	}

	return nullptr;
}

#if WITH_EDITOR

void FBlendStackCameraNodeEvaluator::OnPostBuildAsset(const FGameplayCameraAssetBuildEvent& BuildEvent)
{
	for (FCameraModeEntry& Entry : Entries)
	{
		const bool bRebuildEntry = Entry.ListenedPackages.Contains(BuildEvent.AssetPackage);
		if (bRebuildEntry)
		{
			Entry.EvaluatorStorage.DestroyEvaluatorTree();

			// Re-assign the root node in case the camera mode's root was changed.
			Entry.RootNode->RootNode = Entry.CameraMode->RootNode;

			// Remove the blend on the root node, since we don't want the reloaded camera mode to re-blend-in
			// for no good reason.
			Entry.RootNode->Blend = nullptr;

			// Rebuild the evaluator tree.
			FCameraNodeEvaluatorTreeBuilderParams BuildParams;
			BuildParams.RootCameraNode = Entry.RootNode;
			BuildParams.Evaluator = OwningEvaluator;
			BuildParams.EvaluationContext = Entry.EvaluationContext.Get();
			FCameraNodeEvaluator* RootEvaluator = Entry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);

			Entry.RootEvaluator = RootEvaluator->CastThisChecked<FBlendStackRootCameraNodeEvaluator>();

			// This is the first frame for this new hierarchy of evaluators.
			Entry.bIsFirstFrame = true;
		}
	}
}

#endif  // WITH_EDITOR

