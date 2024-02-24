// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/BlendStackRootCameraNode.h"
#include "Core/CameraAsset.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraSystemEvaluator.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "Modules/ModuleManager.h"
#include "Nodes/Blends/PopBlendCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackCameraNode)

FCameraNodeEvaluatorPtr UBlendStackCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FBlendStackCameraNodeEvaluator>();
}

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendStackCameraNodeEvaluator)

void FBlendStackCameraNodeEvaluator::Push(const FBlendStackCameraPushParams& Params)
{
	if (!Entries.IsEmpty())
	{
		// Don't push anything if what is being requested is already the
		// active camera rig.
		const FCameraRigEntry& TopEntry(Entries.Top());
		if (!TopEntry.bIsFrozen
				&& TopEntry.CameraRig == Params.CameraRig
				&& TopEntry.EvaluationContext == Params.EvaluationContext)
		{
			return;
		}
	}

	// Create the new root node to wrap the new camera rig's root node, and the specific
	// blend node for this transition.
	// We need to const-cast here to be able to use our own blend stack node as the outer
	// of the new node.
	UObject* Outer = const_cast<UObject*>((UObject*)GetCameraNode());
	UBlendStackRootCameraNode* EntryRootNode = NewObject<UBlendStackRootCameraNode>(Outer, NAME_None);
	{
		UCameraNode* ModeRootNode = Params.CameraRig->RootNode;
		EntryRootNode->RootNode = ModeRootNode;

		// Find a transition and use its blend. If no transition is found,
		// make a camera cut transition.
		UBlendCameraNode* ModeBlend = nullptr;
		if (const FCameraRigTransition* Transition = FindTransition(Params))
		{
			ModeBlend = Transition->Blend;
		}
		if (!ModeBlend)
		{
			ModeBlend = NewObject<UPopBlendCameraNode>(EntryRootNode, NAME_None);
		}
		EntryRootNode->Blend = ModeBlend;
	}

	// Make the new stack entry, and use its storage buffer to build the tree of evaluators.
	FCameraRigEntry NewEntry;

	FCameraNodeEvaluatorTreeBuilderParams BuildParams;
	BuildParams.RootCameraNode = EntryRootNode;
	BuildParams.Evaluator = Params.Evaluator;
	BuildParams.EvaluationContext = Params.EvaluationContext;
	FCameraNodeEvaluator* RootEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);

	NewEntry.EvaluationContext = Params.EvaluationContext;
	NewEntry.CameraRig = Params.CameraRig;
	NewEntry.RootNode = EntryRootNode;
	NewEntry.RootEvaluator = RootEvaluator->CastThisChecked<FBlendStackRootCameraNodeEvaluator>();
	NewEntry.bIsFirstFrame = true;

#if WITH_EDITOR
	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
	Params.CameraRig->GatherPackages(NewEntry.ListenedPackages);
	for (const UPackage* ListenPackage : NewEntry.ListenedPackages)
	{
		int32& NumListens = AllListenedPackages.FindOrAdd(ListenPackage, 0);
		if (NumListens == 0)
		{
			LiveEditManager->AddListener(ListenPackage, this);
		}
		++NumListens;
	}
#endif  // WITH_EDITOR

	// Important: we need to move the new entry here because copying evaluator storage
	// is disabled.
	Entries.Add(MoveTemp(NewEntry));
}

FCameraNodeEvaluatorChildrenView FBlendStackCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView View;
	for (FCameraRigEntry& Entry : Entries)
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
	for (FCameraRigEntry& Entry : Entries)
	{
		TSharedPtr<const FCameraEvaluationContext> CurContext = Entry.EvaluationContext.Pin();
		if (UNLIKELY(!CurContext.IsValid()))
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
			// If the context in which this camera rig runs doesn't have a valid result,
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
			CurResult.VariableTable.OverrideAll(OutResult.VariableTable);
			CurResult.VariableTable.ClearAllWrittenThisFrameFlags();

			// Override it with whatever the evaluation context has set on its result.
			CurResult.CameraPose.OverrideChanged(ContextResult.CameraPose);
			CurResult.VariableTable.OverrideAll(ContextResult.VariableTable);
			CurResult.bIsCameraCut = OutResult.bIsCameraCut || ContextResult.bIsCameraCut;
			CurResult.bIsValid = true;

			// Run the camera rig!
			Entry.RootEvaluator->Run(CurParams, CurResult);
		}
		else
		{
			// Only evaluate the blend via the root node.
			Entry.RootEvaluator->Run(CurParams, CurResult);
		}
	}

	// Now blend all the results, keeping track of blends that have reached 100% so
	// that we can remove any camera rigs below (since the would have been completely
	// blended out by that).
	int32 EntryIndex = 0;
	int32 PopEntriesBelow = INDEX_NONE;
	for (FCameraRigEntry& Entry : Entries)
	{
		FCameraNodeEvaluationResult& CurResult(Entry.Result);
		if (UNLIKELY(!CurResult.bIsValid))
		{
			continue;
		}

		const FCameraPoseFlags ChangedFlags(CurResult.CameraPose.GetChangedFlags());

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = Entry.EvaluationContext.Pin();
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

	// Pop out camera rigs that have been blended out.
	if (BlendStackNode->bAutoPop && PopEntriesBelow != INDEX_NONE)
	{
#if WITH_EDITOR
		IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
		TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
#endif  // WITH_EDITOR

		for (int32 Index = 0; Index < PopEntriesBelow; ++Index)
		{
#if WITH_EDITOR
			const FCameraRigEntry& FirstEntry = Entries[0];
			for (const UPackage* ListenPackage : FirstEntry.ListenedPackages)
			{
				int32* NumListens = AllListenedPackages.Find(ListenPackage);
				if (ensure(NumListens))
				{
					--(*NumListens);
					if (*NumListens == 0)
					{
						LiveEditManager->RemoveListener(ListenPackage, this);
						AllListenedPackages.Remove(ListenPackage);
					}
				}
			}
#endif  // WITH_EDITOR

			Entries.RemoveAt(0);
		}
	}

	// Reset first frame flags.
	for (FCameraRigEntry& Entry : Entries)
	{
		Entry.bIsFirstFrame = false;
	}
}

const FCameraRigTransition* FBlendStackCameraNodeEvaluator::FindTransition(const FBlendStackCameraPushParams& Params) const
{
	const UBlendStackCameraNode* BlendStackNode = GetCameraNodeAs<UBlendStackCameraNode>();

	TSharedPtr<const FCameraEvaluationContext> ToContext = Params.EvaluationContext;
	const UCameraAsset* ToCameraAsset = ToContext ? ToContext->GetCameraAsset() : nullptr;
	const UCameraRigAsset* ToCameraRig = Params.CameraRig;

	// Find a transition that works for blending towards ToCameraRig.
	// If the stack isn't empty, we need to find a transition that works between the previous and 
	// next camera rigs. If the stack is empty, we blend the new camera rig in from nothing if
	// appropriate.
	if (!Entries.IsEmpty())
	{
		const FCameraRigTransition* TransitionToUse = nullptr;

		// Start by looking at exit transitions on the last active (top) camera rig.
		const FCameraRigEntry& TopEntry = Entries.Top();

		TSharedPtr<const FCameraEvaluationContext> FromContext = TopEntry.EvaluationContext.Pin();
		const UCameraAsset* FromCameraAsset = FromContext ? FromContext->GetCameraAsset() : nullptr;
		const UCameraRigAsset* FromCameraRig = TopEntry.CameraRig;

		if (!TopEntry.bIsFrozen)
		{
			// Look for exit transitions on the last active camera rig itself.
			TransitionToUse = FindTransition(
					FromCameraRig->ExitTransitions,
					FromCameraRig, FromCameraAsset, false,
					ToCameraRig, ToCameraAsset);
			if (TransitionToUse)
			{
				return TransitionToUse;
			}
			
			// Look for exit transitions on its parent camera asset.
			if (FromCameraAsset)
			{
				TransitionToUse = FindTransition(
						FromCameraAsset->ExitTransitions,
						FromCameraRig, FromCameraAsset, false,
						ToCameraRig, ToCameraAsset);
				if (TransitionToUse)
				{
					return TransitionToUse;
				}
			}
		}

		// Now look at enter transitions on the new camera rig.
		TransitionToUse = FindTransition(
				ToCameraRig->EnterTransitions,
				FromCameraRig, FromCameraAsset, TopEntry.bIsFrozen,
				ToCameraRig, ToCameraAsset);
		if (TransitionToUse)
		{
			return TransitionToUse;
		}

		// Look at enter transitions on its parent camera asset.
		if (ToCameraAsset)
		{
			TransitionToUse = FindTransition(
					ToCameraAsset->EnterTransitions,
					FromCameraRig, FromCameraAsset, TopEntry.bIsFrozen,
					ToCameraRig, ToCameraAsset);
			if (TransitionToUse)
			{
				return TransitionToUse;
			}
		}
	}
	else if (BlendStackNode->bBlendFirstCameraRig)
	{
		return FindTransition(
				ToCameraRig->EnterTransitions,
				nullptr, nullptr, false,
				ToCameraRig, ToCameraAsset);
	}

	return nullptr;
}

const FCameraRigTransition* FBlendStackCameraNodeEvaluator::FindTransition(
			TArrayView<const FCameraRigTransition> Transitions, 
			const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset) const
{
	FCameraRigTransitionConditionMatchParams MatchParams;
	MatchParams.FromCameraRig = FromCameraRig;
	MatchParams.FromCameraAsset = FromCameraAsset;
	MatchParams.ToCameraRig = ToCameraRig;
	MatchParams.ToCameraAsset = ToCameraAsset;

	// The transition should be used if all its conditions pass.
	for (const FCameraRigTransition& Transition : Transitions)
	{
		bool bConditionsPass = true;
		for (const UCameraRigTransitionCondition* Condition : Transition.Conditions)
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

void FBlendStackCameraNodeEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	for (FCameraRigEntry& Entry : Entries)
	{
		Collector.AddReferencedObject(Entry.CameraRig);
		Collector.AddReferencedObject(Entry.RootNode);
	}
}

#if WITH_EDITOR

void FBlendStackCameraNodeEvaluator::OnPostBuildAsset(const FGameplayCameraAssetBuildEvent& BuildEvent)
{
	for (FCameraRigEntry& Entry : Entries)
	{
		const bool bRebuildEntry = Entry.ListenedPackages.Contains(BuildEvent.AssetPackage);
		if (bRebuildEntry)
		{
			Entry.EvaluatorStorage.DestroyEvaluatorTree();

			// Re-assign the root node in case the camera rig's root was changed.
			Entry.RootNode->RootNode = Entry.CameraRig->RootNode;

			// Remove the blend on the root node, since we don't want the reloaded camera rig to re-blend-in
			// for no good reason.
			Entry.RootNode->Blend = NewObject<UPopBlendCameraNode>(Entry.RootNode, NAME_None);

			// Rebuild the evaluator tree.
			FCameraNodeEvaluatorTreeBuilderParams BuildParams;
			BuildParams.RootCameraNode = Entry.RootNode;
			BuildParams.Evaluator = OwningEvaluator;
			BuildParams.EvaluationContext = Entry.EvaluationContext.Pin();
			FCameraNodeEvaluator* RootEvaluator = Entry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);

			Entry.RootEvaluator = RootEvaluator->CastThisChecked<FBlendStackRootCameraNodeEvaluator>();

			// This is the first frame for this new hierarchy of evaluators.
			Entry.bIsFirstFrame = true;
		}
	}
}

#endif  // WITH_EDITOR

}  // namespace UE::Cameras

