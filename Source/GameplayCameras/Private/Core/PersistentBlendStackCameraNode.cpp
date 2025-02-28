// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/PersistentBlendStackCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/BlendStackCameraRigEvent.h"
#include "Core/BlendStackRootCameraNode.h"
#include "Core/CameraEvaluationContext.h"

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FPersistentBlendStackCameraNodeEvaluator)

void FPersistentBlendStackCameraNodeEvaluator::Insert(const FBlendStackCameraInsertParams& Params)
{
	// See if we already have this camera rig and evaluation context in the stack.
	if (!Params.bForceInsert)
	{
		for (const FCameraRigEntry& Entry : Entries)
		{
			if (!Entry.bIsFrozen &&
					Entry.CameraRig == Params.CameraRig &&
					Entry.EvaluationContext == Params.EvaluationContext)
			{
				return;
			}
		}
	}

	// TODO: add support for slot indices or something, to allow callers to specify a place in the stack.
	UObject* Outer = const_cast<UObject*>((UObject*)GetCameraNode());
	UBlendStackRootCameraNode* EntryRootNode = NewObject<UBlendStackRootCameraNode>(Outer, NAME_None);
	{
		EntryRootNode->RootNode = Params.CameraRig->RootNode;
		// TODO: add support for blending in and out.
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
		return;
	}

#if WITH_EDITOR
	AddPackageListeners(NewEntry);
#endif  // WITH_EDITOR

	Entries.Add(MoveTemp(NewEntry));

	if (OnCameraRigEventDelegate.IsBound())
	{
		BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Pushed, Entries.Last(), nullptr);
	}
}

void FPersistentBlendStackCameraNodeEvaluator::Remove(const FBlendStackCameraRemoveParams& Params)
{
	for (int32 Index = Entries.Num() - 1; Index >= 0; --Index)
	{
		FCameraRigEntry& Entry(Entries[Index]);
		if (Entry.CameraRig == Params.CameraRig &&
				Entry.EvaluationContext == Params.EvaluationContext)
		{
			PopEntry(Index);
		}
	}
}

void FPersistentBlendStackCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
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

	for (FResolvedEntry& ResolvedEntry : ResolvedEntries)
	{
		FCameraRigEntry& Entry(ResolvedEntry.Entry);

		if (!Entry.bIsFrozen)
		{
			FCameraNodeEvaluationParams CurParams(Params);
			CurParams.EvaluationContext = ResolvedEntry.Context;
			CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

			FCameraNodeEvaluationResult& CurResult(Entry.Result);

			// Start with the input given to us.
			{
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
			}
			else
			{
				OutResult.OverrideAll(CurResult);
			}
		}
		else
		{
			FCameraNodeEvaluationResult& CurResult(Entry.Result);

			OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::None);
			OutResult.OverrideAll(CurResult);
		}
	}
}

}  // namespace UE::Cameras

