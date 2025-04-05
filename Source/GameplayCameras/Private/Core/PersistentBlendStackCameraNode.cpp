// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PersistentBlendStackCameraNode.h"

#include "Algo/BinarySearch.h"
#include "Core/BlendCameraNode.h"
#include "Core/BlendStackCameraRigEvent.h"
#include "Core/BlendStackRootCameraNode.h"
#include "Core/CameraEvaluationContext.h"
#include "Helpers/CameraRigTransitionFinder.h"
#include "Nodes/Blends/InterruptedBlendCameraNode.h"
#include "Nodes/Blends/PopBlendCameraNode.h"
#include "Nodes/Blends/ReverseBlendCameraNode.h"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FPersistentBlendStackCameraNodeEvaluator)

FBlendStackEntryID FPersistentBlendStackCameraNodeEvaluator::Insert(const FBlendStackCameraInsertParams& Params)
{
	// See if we already have this camera rig and evaluation context in the stack.
	if (!Params.bForceInsert)
	{
		for (int32 Index = 0; Index < Entries.Num(); ++Index)
		{
			const FCameraRigEntry& Entry(Entries[Index]);
			const FCameraRigEntryExtraInfo& EntryExtraInfo(EntryExtraInfos[Index]);

			if (!Entry.bIsFrozen &&
					Entry.CameraRig == Params.CameraRig &&
					Entry.EvaluationContext == Params.EvaluationContext &&
					EntryExtraInfo.StackOrder == Params.StackOrder)
			{
				return FBlendStackEntryID();
			}
		}
	}

	UObject* Outer = const_cast<UObject*>((UObject*)GetCameraNode());
	UBlendStackRootCameraNode* EntryRootNode = NewObject<UBlendStackRootCameraNode>(Outer, NAME_None);
	{
		EntryRootNode->RootNode = Params.CameraRig->RootNode;

		// Find a transition to blend in. If no transition is found, use a pop blend.
		UBlendCameraNode* ModeBlend = nullptr;
		if (const UCameraRigTransition* Transition = FindEnterTransition(Params))
		{
			ModeBlend = Transition->Blend;
		}
		if (!ModeBlend)
		{
			ModeBlend = NewObject<UPopBlendCameraNode>(EntryRootNode, NAME_None);
		}
		EntryRootNode->Blend = ModeBlend;
	}

	FCameraRigEntry NewEntry;
	const bool bInitialized = InitializeEntry(
			NewEntry, 
			Params.CameraRig,
			Params.EvaluationContext,
			EntryRootNode,
			false);
	if (!bInitialized)
	{
		return FBlendStackEntryID();
	}

	FCameraRigEntryExtraInfo NewExtraInfo;
	NewExtraInfo.StackOrder = Params.StackOrder;
	NewExtraInfo.BlendStatus = EBlendStatus::BlendIn;

#if WITH_EDITOR
	AddPackageListeners(NewEntry);
#endif  // WITH_EDITOR

	const FBlendStackEntryID AddedEntryID(NewEntry.EntryID);

	ensure(Entries.Num() == EntryExtraInfos.Num());

	const int32 AddedIndex = Algo::UpperBoundBy(EntryExtraInfos, Params.StackOrder, &FCameraRigEntryExtraInfo::StackOrder);
	Entries.Insert(MoveTemp(NewEntry), AddedIndex);
	EntryExtraInfos.Insert(MoveTemp(NewExtraInfo), AddedIndex);

	if (OnCameraRigEventDelegate.IsBound())
	{
		BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Pushed, Entries[AddedIndex], nullptr);
	}

	return AddedEntryID;
}

void FPersistentBlendStackCameraNodeEvaluator::Remove(const FBlendStackCameraRemoveParams& Params)
{
	TArray<int32, TInlineAllocator<4>> EntriesToRemove;

	if (Params.EntryID.IsValid())
	{
		// Remove the entry by ID.
		const int32 EntryIndex = IndexOfEntry(Params.EntryID);
		if (EntryIndex != INDEX_NONE)
		{
			EntriesToRemove.Add(EntryIndex);
		}
	}
	else
	{
		// Remove any entries matching the given context and rig asset.
		for (int32 Index = Entries.Num() - 1; Index >= 0; --Index)
		{
			FCameraRigEntry& Entry(Entries[Index]);
			if (Entry.CameraRig == Params.CameraRig &&
					Entry.EvaluationContext == Params.EvaluationContext)
			{
				EntriesToRemove.Add(Index);
			}
		}
	}

	// If we need to remove the camera rigs immediately, simply pop out their entries.
	if (Params.bRemoveImmediately)
	{
		for (int32 Index : EntriesToRemove)
		{
			PopEntry(Index);
			EntryExtraInfos.RemoveAt(Index);
		}
	}
	// Else, we need to start blending out these entries.
	else
	{
		for (int32 Index : EntriesToRemove)
		{
			FCameraRigEntry& Entry(Entries[Index]);
			FCameraRigEntryExtraInfo& EntryExtraInfo(EntryExtraInfos[Index]);
			const UCameraRigTransition* Transition = FindExitTransition(Params, Entry);
			if (Transition && Transition->Blend)
			{
				// Swap the blend-in evaluator on this entry with a blend-out one.
				if (EntryExtraInfo.BlendStatus != EBlendStatus::BlendOut)
				{
					FCameraNodeEvaluatorBuilder BlendOutBuilder(Entry.EvaluatorStorage);
					FCameraNodeEvaluatorBuildParams BlendOutBuildParams(BlendOutBuilder);
					FBlendCameraNodeEvaluator* BlendOutEvaluator = BlendOutBuildParams.BuildEvaluatorAs<FBlendCameraNodeEvaluator>(Transition->Blend);

					FCameraNodeEvaluatorInitializeParams BlendOutInitParams;
					BlendOutInitParams.Evaluator = OwningEvaluator;
					BlendOutInitParams.EvaluationContext = Entry.EvaluationContext.Pin();
					BlendOutEvaluator->Initialize(BlendOutInitParams, Entry.Result);

					// Reverse this blend so it plays as a blend-out. Also, see if we are going to 
					// interrupt an ongoing blend-in... if so, give a chance for the blend-out to
					// start at an "equivalent spot".
					if (!BlendOutEvaluator->SetReversed(true))
					{
						BlendOutEvaluator = Entry.EvaluatorStorage.BuildEvaluator<FReverseBlendCameraNodeEvaluator>(BlendOutEvaluator);
					}
					if (EntryExtraInfo.BlendStatus == EBlendStatus::BlendIn)
					{
						FBlendCameraNodeEvaluator* OngoingBlend = Entry.RootEvaluator->GetBlendEvaluator();

						FCameraNodeBlendInterruptionParams InterruptionParams;
						InterruptionParams.InterruptedBlend = OngoingBlend;
						if (!BlendOutEvaluator->InitializeFromInterruption(InterruptionParams))
						{
							BlendOutEvaluator = Entry.EvaluatorStorage.BuildEvaluator<FInterruptedBlendCameraNodeEvaluator>(BlendOutEvaluator, OngoingBlend);
						}
					}
					// Note: neither the reverse or interrupted blends need initialization, but
					// technically we're missing calling it on them.
					Entry.RootEvaluator->SetBlendEvaluator(BlendOutEvaluator);

					EntryExtraInfo.BlendStatus = EBlendStatus::BlendOut;
					EntryExtraInfo.bIsBlendFinished = false;
					EntryExtraInfo.bIsBlendFull = false;
				}
				// else: we were already blending out, so let this continue.
			}
			else
			{
				// No transition found... just cut.
				PopEntry(Index);
				EntryExtraInfos.RemoveAt(Index);
			}
		}
	}
}

void FPersistentBlendStackCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	ensure(Entries.Num() == EntryExtraInfos.Num());

	// Validate our entries and resolve evaluation context weak pointers.
	TArray<FResolvedEntry> ResolvedEntries;
	ResolveEntries(ResolvedEntries);

	// Run the stack!
	InternalUpdate(ResolvedEntries, Params, OutResult);

	// Tidy things up.
	OnRunFinished(OutResult);
}

void FPersistentBlendStackCameraNodeEvaluator::InternalUpdate(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	constexpr ECameraVariableTableFilter VariableTableFilter = ECameraVariableTableFilter::ChangedOnly;
	constexpr ECameraContextDataTableFilter ContextDataTableFilter = ECameraContextDataTableFilter::ChangedOnly;

	TArray<int32, TInlineAllocator<4>> EntriesToRemove;

	for (FResolvedEntry& ResolvedEntry : ResolvedEntries)
	{
		FCameraRigEntry& Entry(ResolvedEntry.Entry);
		FCameraRigEntryExtraInfo& EntryExtraInfo(EntryExtraInfos[ResolvedEntry.EntryIndex]);

		if (!Entry.bIsFrozen)
		{
			FCameraNodeEvaluationParams CurParams(Params);
			CurParams.EvaluationContext = ResolvedEntry.Context;
			CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

			FCameraNodeEvaluationResult& CurResult(Entry.Result);

			// Start with the input given to us.
			{
				CurResult.Reset();
				CurResult.CameraPose = OutResult.CameraPose;
				CurResult.VariableTable.OverrideAll(OutResult.VariableTable);
				CurResult.CameraRigJoints.OverrideAll(OutResult.CameraRigJoints);
				CurResult.PostProcessSettings.OverrideAll(OutResult.PostProcessSettings);

				// Override it with whatever the evaluation context has set on its result.
				// Evaluation contexts may have private variables we need to pass along, such as when rig parameter
				// overrides have been set on them, so include private variables in the filter.
				const FCameraNodeEvaluationResult& ContextResult(ResolvedEntry.Context->GetInitialResult());
				CurResult.CameraPose.OverrideChanged(ContextResult.CameraPose);
				CurResult.VariableTable.Override(ContextResult.VariableTable, VariableTableFilter);
				CurResult.ContextDataTable.Override(ContextResult.ContextDataTable, ContextDataTableFilter);

				// Setup flags.
				CurResult.bIsCameraCut = OutResult.bIsCameraCut || ContextResult.bIsCameraCut || Entry.bForceCameraCut;
				CurResult.bIsValid = true;
			}

			// Update pre-blended parameters.
			{
				FCameraBlendedParameterUpdateParams InputParams(CurParams, CurResult.CameraPose);
				FCameraBlendedParameterUpdateResult InputResult(CurResult.VariableTable);

				Entry.EvaluatorHierarchy.ForEachEvaluator(ECameraNodeEvaluatorFlags::NeedsParameterUpdate,
						[&InputParams, &InputResult](FCameraNodeEvaluator* ParameterEvaluator)
						{
							ParameterEvaluator->UpdateParameters(InputParams, InputResult);
						});
			}

			// Run the blend node.
			FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
			if (EntryBlendEvaluator)
			{
				EntryBlendEvaluator->Run(CurParams, CurResult);
			}

			// Blend pre-blended parameters.
			if (EntryBlendEvaluator)
			{
				FCameraNodePreBlendParams PreBlendParams(CurParams, CurResult.CameraPose, CurResult.VariableTable);
				FCameraNodePreBlendResult PreBlendResult(OutResult.VariableTable);

				EntryBlendEvaluator->BlendParameters(PreBlendParams, PreBlendResult);
			}
			else
			{
				OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::None);
			}

			// Run the camera rig's root node.
			FCameraNodeEvaluator* RootEvaluator = Entry.RootEvaluator->GetRootEvaluator();
			if (RootEvaluator)
			{
				RootEvaluator->Run(CurParams, CurResult);
			}

			// Blend the results.
			if (EntryBlendEvaluator)
			{
				FCameraNodeBlendParams BlendParams(CurParams, CurResult);
				FCameraNodeBlendResult BlendResult(OutResult);

				EntryBlendEvaluator->BlendResults(BlendParams, BlendResult);

				EntryExtraInfo.bIsBlendFinished = BlendResult.bIsBlendFinished;
				EntryExtraInfo.bIsBlendFull = BlendResult.bIsBlendFull;
			}
			else
			{
				OutResult.OverrideAll(CurResult);
			}

			// Update blend state.
			if (EntryExtraInfo.BlendStatus == EBlendStatus::BlendIn)
			{
				if (EntryExtraInfo.bIsBlendFull && EntryExtraInfo.bIsBlendFinished)
				{
					EntryExtraInfo.BlendStatus = EBlendStatus::None;
				}
			}
			else if (EntryExtraInfo.BlendStatus == EBlendStatus::BlendOut)
			{
				if (EntryExtraInfo.bIsBlendFull && EntryExtraInfo.bIsBlendFinished)
				{
					EntriesToRemove.Add(ResolvedEntry.EntryIndex);
				}
			}
		}
		else
		{
			FCameraNodeEvaluationResult& CurResult(Entry.Result);

			OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::None);
			OutResult.OverrideAll(CurResult);
		}
	}

	for (int32 Index = EntriesToRemove.Num() - 1; Index >= 0; --Index)
	{
		PopEntry(EntriesToRemove[Index]);
		EntryExtraInfos.RemoveAt(EntriesToRemove[Index]);
	}
}

const UCameraRigTransition* FPersistentBlendStackCameraNodeEvaluator::FindEnterTransition(const FBlendStackCameraInsertParams& Params) const
{
	// If we are forced to use a specific transition, our search is over.
	if (Params.TransitionOverride)
	{
		return Params.TransitionOverride;
	}

	// Find a transition that works for blending the given camera rig in.
	return FCameraRigTransitionFinder::FindTransition(
			Params.CameraRig->EnterTransitions,
			nullptr, nullptr, false,
			Params.CameraRig, nullptr);
}

const UCameraRigTransition* FPersistentBlendStackCameraNodeEvaluator::FindExitTransition(const FBlendStackCameraRemoveParams& Params, const FCameraRigEntry& Entry) const
{
	// If we are forced to use a specific transition, our search is over.
	if (Params.TransitionOverride)
	{
		return Params.TransitionOverride;
	}

	// Find a transition that works for blending the given camera rig out.
	return FCameraRigTransitionFinder::FindTransition(
			Entry.CameraRig->ExitTransitions,
			Entry.CameraRig, nullptr, Entry.bIsFrozen,
			nullptr, nullptr);
}

#if WITH_EDITOR

void FPersistentBlendStackCameraNodeEvaluator::OnEntryReinitialized(int32 EntryIndex)
{
	if (!ensure(EntryExtraInfos.IsValidIndex(EntryIndex)))
	{
		return;
	}

	FCameraRigEntryExtraInfo& ExtraInfo = EntryExtraInfos[EntryIndex];
	{
		// When hot-reloading camera rigs, the base class replaces the blend node with pop so
		// let's update our own extra info accordingly.
		ExtraInfo.bIsBlendFull = true;
		ExtraInfo.bIsBlendFinished = true;
		ExtraInfo.BlendStatus = EBlendStatus::None;
	}
}

#endif

}  // namespace UE::Cameras

