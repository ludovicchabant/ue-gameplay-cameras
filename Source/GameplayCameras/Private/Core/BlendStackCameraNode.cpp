// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/BlendStackCameraRigEvent.h"
#include "Core/BlendStackRootCameraNode.h"
#include "Core/CameraAsset.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraSystemEvaluator.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraNodeEvaluationResultDebugBlock.h"
#include "Debug/CameraNodeEvaluatorDebugBlock.h"
#include "Debug/CameraPoseDebugBlock.h"
#include "Debug/VariableTableDebugBlock.h"
#include "HAL/IConsoleManager.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "Math/ColorList.h"
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

bool GGameplayCamerasDebugBlendStackShowUnchanged = false;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugBlendStackShowUnchanged(
	TEXT("GameplayCameras.Debug.BlendStack.ShowUnchanged"),
	GGameplayCamerasDebugBlendStackShowUnchanged,
	TEXT(""));

bool GGameplayCamerasDebugBlendStackShowVariableIDs = false;
static FAutoConsoleVariableRef CVarGameplayCamerasDebugBlendStackShowVariableIDs(
	TEXT("GameplayCameras.Debug.BlendStack.ShowVariableIDs"),
	GGameplayCamerasDebugBlendStackShowVariableIDs,
	TEXT(""));

UE_DEFINE_CAMERA_NODE_EVALUATOR(FBlendStackCameraNodeEvaluator)

FBlendStackCameraNodeEvaluator::~FBlendStackCameraNodeEvaluator()
{
	// Pop all our entries to unregister the live-edit callbacks.
	PopEntries(Entries.Num());
}

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
	const UCameraRigTransition* UsedTransition = nullptr;
	UObject* Outer = const_cast<UObject*>((UObject*)GetCameraNode());
	UBlendStackRootCameraNode* EntryRootNode = NewObject<UBlendStackRootCameraNode>(Outer, NAME_None);
	{
		EntryRootNode->RootNode = Params.CameraRig->RootNode;

		// Find a transition and use its blend. If no transition is found,
		// make a camera cut transition.
		UBlendCameraNode* ModeBlend = nullptr;
		if (const UCameraRigTransition* Transition = FindTransition(Params))
		{
			ModeBlend = Transition->Blend;
			UsedTransition = Transition;
		}
		if (!ModeBlend)
		{
			ModeBlend = NewObject<UPopBlendCameraNode>(EntryRootNode, NAME_None);
		}
		EntryRootNode->Blend = ModeBlend;
	}

	// Make the new stack entry, and use its storage buffer to build the tree of evaluators.
	FCameraRigEntry NewEntry;
	const bool bInitialized = InitializeEntry(
			NewEntry, 
			Params.CameraRig,
			Params.Evaluator,
			Params.EvaluationContext,
			EntryRootNode);
	if (!bInitialized)
	{
		return;
	}

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

	if (OnCameraRigEventDelegate.IsBound())
	{
		BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Pushed, Entries.Last(), UsedTransition);
	}
}

bool FBlendStackCameraNodeEvaluator::InitializeEntry(
		FCameraRigEntry& NewEntry, 
		const UCameraRigAsset* CameraRig,
		FCameraSystemEvaluator* Evaluator,
		TSharedPtr<const FCameraEvaluationContext> EvaluationContext,
		UBlendStackRootCameraNode* EntryRootNode)
{
	// Generate the hierarchy of node evaluators inside our storage buffer.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = EntryRootNode;
	BuildParams.AllocationInfo = &CameraRig->AllocationInfo.EvaluatorInfo;
	FCameraNodeEvaluator* RootEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);
	if (!ensureMsgf(RootEvaluator, TEXT("No root evaluator was created for new camera rig!")))
	{
		return false;
	}

	// Initialize the node evaluators.
	FCameraNodeEvaluatorInitializeParams InitParams;
	InitParams.Evaluator = Evaluator;
	InitParams.EvaluationContext = EvaluationContext;
	InitParams.LastActiveCameraRigInfo = GetActiveCameraRigEvaluationInfo();
	RootEvaluator->Initialize(InitParams);

	// Gather blended parameter evaluators.
	NewEntry.ParameterEvaluators.Reset();
	GatherEntryParameterEvaluators(RootEvaluator, NewEntry.ParameterEvaluators);

	// Allocate variables in the variable table.
	NewEntry.Result.VariableTable.Initialize(CameraRig->AllocationInfo.VariableTableInfo);

	// Wrap up!
	NewEntry.EvaluationContext = EvaluationContext;
	NewEntry.CameraRig = CameraRig;
	NewEntry.RootNode = EntryRootNode;
	NewEntry.RootEvaluator = RootEvaluator->CastThisChecked<FBlendStackRootCameraNodeEvaluator>();
	NewEntry.bIsFirstFrame = true;

	return true;
}

void FBlendStackCameraNodeEvaluator::GatherEntryParameterEvaluators(FCameraNodeEvaluator* RootEvaluator, TArray<FCameraNodeEvaluator*>& OutParameterEvaluators)
{
	TArray<FCameraNodeEvaluator*> EvaluatorStack;
	EvaluatorStack.Add(RootEvaluator);
	while (!EvaluatorStack.IsEmpty())
	{
		FCameraNodeEvaluator* CurEvaluator = EvaluatorStack.Pop();

		if (EnumHasAnyFlags(CurEvaluator->GetNodeEvaluatorFlags(), ECameraNodeEvaluatorFlags::NeedsParameterUpdate))
		{
			OutParameterEvaluators.Add(CurEvaluator);
		}
		else
		{
			FCameraNodeEvaluatorChildrenView CurChildren(CurEvaluator->GetChildren());
			for (FCameraNodeEvaluator* Child : ReverseIterate(CurChildren))
			{
				if (Child)
				{
					EvaluatorStack.Add(Child);
				}
			}
		}
	}
}

FCameraRigEvaluationInfo FBlendStackCameraNodeEvaluator::GetActiveCameraRigEvaluationInfo() const
{
	if (Entries.Num() > 0)
	{
		const FCameraRigEntry& ActiveEntry = Entries[0];
		FCameraRigEvaluationInfo Info(
				ActiveEntry.EvaluationContext.Pin(),
				ActiveEntry.CameraRig, 
				&ActiveEntry.Result,
				ActiveEntry.RootEvaluator->GetRootEvaluator());
		return Info;
	}
	return FCameraRigEvaluationInfo();
}

FCameraNodeEvaluatorChildrenView FBlendStackCameraNodeEvaluator::OnGetChildren()
{
	FCameraNodeEvaluatorChildrenView View;
	for (FCameraRigEntry& Entry : Entries)
	{
		if (Entry.RootEvaluator)
		{
			View.Add(Entry.RootEvaluator);
		}
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

	// Filter out invalid entries and go through other basic validation.
	struct FValidEntry
	{
		FCameraRigEntry& Entry;
		TSharedPtr<const FCameraEvaluationContext> Context;
		int32 EntryIndex;
	};

	TArray<FValidEntry> ValidEntries;

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		FCameraRigEntry& Entry(Entries[Index]);
		TSharedPtr<const FCameraEvaluationContext> CurContext = Entry.EvaluationContext.Pin();

		if (!Entry.bIsFrozen)
		{
			// Check that we still have a valid context.
			if (UNLIKELY(!CurContext.IsValid()))
			{
				Entry.Result.bIsValid = false;

#if UE_GAMEPLAY_CAMERAS_TRACE
				if (Entry.bLogWarnings)
				{
					UE_LOG(LogCameraSystem, Warning,
							TEXT("Can't update camera rig '%s' because its evaluation context isn't valid."),
							*GetNameSafe(Entry.CameraRig));
					Entry.bLogWarnings = false;
				}
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

				continue;
			}

			// Check that we have a valid result for this context.
			const FCameraNodeEvaluationResult& ContextResult(CurContext->GetInitialResult());
			if (UNLIKELY(!ContextResult.bIsValid))
			{
				Entry.Result.bIsValid = false;

#if UE_GAMEPLAY_CAMERAS_TRACE
				if (Entry.bLogWarnings)
				{
					UE_LOG(LogCameraSystem, Warning,
							TEXT("Can't update camera rig '%s' because its initial result isn't valid."),
							*GetNameSafe(Entry.CameraRig));
					Entry.bLogWarnings = false;
				}
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

				continue;
			}
		}
		// else: frozen entries may have null contexts or invalid initial results
		//       because we're not going to update them anyway. We will however blend
		//       them so we add them to the valid entries.

		// This entry is valid, we'll process it.
		ValidEntries.Add({ Entry, CurContext, Index });

#if UE_GAMEPLAY_CAMERAS_TRACE
		// This entry might have been temporarily invalid before. It's valid now, so let's
		// re-enable warnings if it becomes invalid again in the future.
		Entry.bLogWarnings = true;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
	}

	// Gather parameters to pre-blend, and evaluate blend nodes.
	for (FValidEntry& ValidEntry : ValidEntries)
	{
		FCameraRigEntry& Entry(ValidEntry.Entry);
		TSharedPtr<const FCameraEvaluationContext> CurContext(ValidEntry.Context);

		if (Entry.bIsFrozen)
		{
			continue;
		}

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = CurContext;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		// Gather input parameters.
		if (!Entry.bInputRunThisFrame)
		{
			FCameraBlendedParameterUpdateParams InputParams(CurParams, CurResult.CameraPose);
			FCameraBlendedParameterUpdateResult InputResult(CurResult.VariableTable);

			for (FCameraNodeEvaluator* ParameterEvaluator : Entry.ParameterEvaluators)
			{
				ParameterEvaluator->UpdateParameters(InputParams, InputResult);
			}

			Entry.bInputRunThisFrame = true;
		}

		// Run blends.
		// Note that we pass last frame's camera pose to the Run() method. This may change.
		// Blends aren't expected to use the camera pose to do any logic until BlendResults().
		if (!Entry.bBlendRunThisFrame)
		{
			FBlendCameraNodeEvaluator* BlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
			if (ensure(BlendEvaluator))
			{
				BlendEvaluator->Run(CurParams, CurResult);
			}

			Entry.bBlendRunThisFrame = true;
		}
	}

	// Blend input variables.
	for (FValidEntry& ValidEntry : ValidEntries)
	{
		FCameraRigEntry& Entry(ValidEntry.Entry);
		TSharedPtr<const FCameraEvaluationContext> CurContext(ValidEntry.Context);

		// Don't filter out frozen entries here, they still contribute to the blend
		// using their last evaluated values.

		if (!Entry.ParameterEvaluators.IsEmpty())
		{
			FCameraNodeEvaluationResult& CurResult(Entry.Result);

			FCameraNodeEvaluationParams CurParams(Params);
			CurParams.EvaluationContext = CurContext;
			CurParams.bIsFirstFrame = Entry.bIsFirstFrame;
			FCameraNodePreBlendParams PreBlendParams(CurParams, CurResult.CameraPose, CurResult.VariableTable);

			FCameraNodePreBlendResult PreBlendResult(OutResult.VariableTable);

			FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
			if (ensure(EntryBlendEvaluator))
			{
				EntryBlendEvaluator->BlendParameters(PreBlendParams, PreBlendResult);
			}
		}
	}

	// Run the root nodes. They will use the pre-blended inputs from the last step.
	for (FValidEntry& ValidEntry : ValidEntries)
	{
		FCameraRigEntry& Entry(ValidEntry.Entry);
		TSharedPtr<const FCameraEvaluationContext> CurContext(ValidEntry.Context);

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = CurContext;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		// Start with the input given to us.
		CurResult.CameraPose = OutResult.CameraPose;
		CurResult.CameraPose.ClearAllChangedFlags();
		CurResult.VariableTable.OverrideAll(OutResult.VariableTable);
		CurResult.VariableTable.ClearAllWrittenThisFrameFlags();

		// Override it with whatever the evaluation context has set on its result.
		const FCameraNodeEvaluationResult& ContextResult(CurContext->GetInitialResult());
		CurResult.CameraPose.OverrideChanged(ContextResult.CameraPose);
		CurResult.VariableTable.OverrideAll(ContextResult.VariableTable);
		CurResult.bIsCameraCut = OutResult.bIsCameraCut || ContextResult.bIsCameraCut;
		CurResult.bIsValid = true;

		// Run the camera rig's root node.
		FCameraNodeEvaluator* RootEvaluator = Entry.RootEvaluator->GetRootEvaluator();
		if (ensure(RootEvaluator))
		{
			RootEvaluator->Run(CurParams, CurResult);
		}
	}

	// Now blend all the results, keeping track of blends that have reached 100% so
	// that we can remove any camera rigs below (since they would have been completely
	// blended out by that).
	int32 PopEntriesBelow = INDEX_NONE;
	for (FValidEntry& ValidEntry : ValidEntries)
	{
		FCameraRigEntry& Entry(ValidEntry.Entry);
		TSharedPtr<const FCameraEvaluationContext> CurContext(ValidEntry.Context);

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = CurContext;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;
		FCameraNodeBlendParams BlendParams(CurParams, CurResult);

		FCameraNodeBlendResult BlendResult(OutResult);

		FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
		if (EntryBlendEvaluator)
		{
			EntryBlendEvaluator->BlendResults(BlendParams, BlendResult);

			if (BlendResult.bIsBlendFull && BlendResult.bIsBlendFinished)
			{
				PopEntriesBelow = ValidEntry.EntryIndex;
			}
		}
		else
		{
			OutResult.CameraPose.OverrideAll(CurResult.CameraPose);

			PopEntriesBelow = ValidEntry.EntryIndex;
		}
	}

	// Pop out camera rigs that have been blended out.
	if (BlendStackNode->bAutoPop && PopEntriesBelow != INDEX_NONE)
	{
		PopEntries(PopEntriesBelow);
	}

	// Reset transient flags.
	for (FCameraRigEntry& Entry : Entries)
	{
		Entry.bIsFirstFrame = false;
		Entry.bInputRunThisFrame = false;
		Entry.bBlendRunThisFrame = false;
	}
}

void FBlendStackCameraNodeEvaluator::PopEntries(int32 FirstIndexToKeep)
{
	if (UNLIKELY(Entries.IsEmpty()))
	{
		return;
	}

#if WITH_EDITOR
	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
#endif  // WITH_EDITOR

	for (int32 Index = 0; Index < FirstIndexToKeep; ++Index)
	{
		const FCameraRigEntry& FirstEntry = Entries[0];

#if WITH_EDITOR
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

		if (OnCameraRigEventDelegate.IsBound())
		{
			BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Popped, FirstEntry);
		}

		Entries.RemoveAt(0);
	}
}

const UCameraRigTransition* FBlendStackCameraNodeEvaluator::FindTransition(const FBlendStackCameraPushParams& Params) const
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
		const UCameraRigTransition* TransitionToUse = nullptr;

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
						FromCameraAsset->GetExitTransitions(),
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
					ToCameraAsset->GetEnterTransitions(),
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

const UCameraRigTransition* FBlendStackCameraNodeEvaluator::FindTransition(
			TArrayView<const TObjectPtr<UCameraRigTransition>> Transitions, 
			const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset) const
{
	FCameraRigTransitionConditionMatchParams MatchParams;
	MatchParams.FromCameraRig = FromCameraRig;
	MatchParams.FromCameraAsset = FromCameraAsset;
	MatchParams.ToCameraRig = ToCameraRig;
	MatchParams.ToCameraAsset = ToCameraAsset;

	// The transition should be used if all its conditions pass.
	for (TObjectPtr<const UCameraRigTransition> Transition : Transitions)
	{
		bool bConditionsPass = true;
		for (const UCameraRigTransitionCondition* Condition : Transition->Conditions)
		{
			if (!Condition->TransitionMatches(MatchParams))
			{
				bConditionsPass = false;
				break;
			}
		}

		if (bConditionsPass)
		{
			return Transition;
		}
	}

	return nullptr;
}

void FBlendStackCameraNodeEvaluator::BroadcastCameraRigEvent(EBlendStackCameraRigEventType EventType, const FCameraRigEntry& Entry, const UCameraRigTransition* Transition) const
{
	FBlendStackCameraRigEvent Event;
	Event.EventType = EventType;
	Event.BlendStackEvaluator = this;
	Event.CameraRigInfo = FCameraRigEvaluationInfo(
			Entry.EvaluationContext.Pin(),
			Entry.CameraRig,
			&Entry.Result,
			Entry.RootEvaluator);
	Event.Transition = Transition;

	OnCameraRigEventDelegate.Broadcast(Event);
}

void FBlendStackCameraNodeEvaluator::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	for (FCameraRigEntry& Entry : Entries)
	{
		Collector.AddReferencedObject(Entry.CameraRig);
		Collector.AddReferencedObject(Entry.RootNode);
	}
}

void FBlendStackCameraNodeEvaluator::OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		int32 NumEntries = Entries.Num();
		Ar << NumEntries;
	}
	else if (Ar.IsLoading())
	{
		int32 LoadedNumEntries = 0;
		Ar << LoadedNumEntries;

		ensure(LoadedNumEntries == Entries.Num());
	}

	for (FCameraRigEntry& Entry : Entries)
	{
		Entry.Result.Serialize(Ar);
		Ar << Entry.bIsFirstFrame;
		Ar << Entry.bInputRunThisFrame;
		Ar << Entry.bBlendRunThisFrame;
		Ar << Entry.bIsFrozen;
#if UE_GAMEPLAY_CAMERAS_TRACE
		Ar << Entry.bLogWarnings;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
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
			const bool bInitialized = InitializeEntry(
					Entry,
					Entry.CameraRig,
					OwningEvaluator,
					Entry.EvaluationContext.Pin(),
					Entry.RootNode);
			if (!bInitialized)
			{
				Entry.bIsFrozen = true;
				continue;
			}
		}
	}
}

#endif  // WITH_EDITOR

#if UE_GAMEPLAY_CAMERAS_DEBUG

void FBlendStackCameraNodeEvaluator::OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	Builder.AttachDebugBlock<FBlendStackSummaryCameraDebugBlock>(*this);
}

FBlendStackCameraDebugBlock* FBlendStackCameraNodeEvaluator::BuildDetailedDebugBlock(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder)
{
	FBlendStackCameraDebugBlock& StackDebugBlock = Builder.BuildDebugBlock<FBlendStackCameraDebugBlock>(*this);
	for (const FCameraRigEntry& Entry : Entries)
	{
		// Each entry has a wrapper debug block with 2 children blocks:
		// - block for the blend
		// - block for the result
		FCameraDebugBlock& EntryDebugBlock = Builder.BuildDebugBlock<FCameraDebugBlock>();
		StackDebugBlock.AddChild(&EntryDebugBlock);
		{
			if (Entry.RootEvaluator)
			{
				Builder.StartParentDebugBlockOverride(EntryDebugBlock);
				{
					FCameraNodeEvaluator* BlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
					BlendEvaluator->BuildDebugBlocks(Params, Builder);
				}
				Builder.EndParentDebugBlockOverride();
			}
			else
			{
				// Dummy debug block.
				EntryDebugBlock.AddChild(&Builder.BuildDebugBlock<FCameraDebugBlock>());
			}

			FCameraNodeEvaluationResultDebugBlock& ResultDebugBlock = Builder.BuildDebugBlock<FCameraNodeEvaluationResultDebugBlock>();
			EntryDebugBlock.AddChild(&ResultDebugBlock);
			{
				ResultDebugBlock.Initialize(Entry.Result, Builder);
				ResultDebugBlock.GetCameraPoseDebugBlock()->WithShowUnchangedCVar(TEXT("GameplayCameras.Debug.BlendStack.ShowUnchanged"));
				ResultDebugBlock.GetVariableTableDebugBlock()->WithShowVariableIDsCVar(TEXT("GameplayCameras.Debug.BlendStack.ShowVariableIDs"));
			}
		}
	}
	return &StackDebugBlock;
}

UE_DEFINE_CAMERA_DEBUG_BLOCK(FBlendStackSummaryCameraDebugBlock);

FBlendStackSummaryCameraDebugBlock::FBlendStackSummaryCameraDebugBlock()
{
}

FBlendStackSummaryCameraDebugBlock::FBlendStackSummaryCameraDebugBlock(const FBlendStackCameraNodeEvaluator& InEvaluator)
{
	NumEntries = InEvaluator.Entries.Num();
}

void FBlendStackSummaryCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText(TEXT("%d entries"), NumEntries);
}

void FBlendStackSummaryCameraDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << NumEntries;
}

UE_DEFINE_CAMERA_DEBUG_BLOCK(FBlendStackCameraDebugBlock);

FBlendStackCameraDebugBlock::FBlendStackCameraDebugBlock()
{
}

FBlendStackCameraDebugBlock::FBlendStackCameraDebugBlock(const FBlendStackCameraNodeEvaluator& InEvaluator)
{
	for (const FBlendStackCameraNodeEvaluator::FCameraRigEntry& Entry : InEvaluator.Entries)
	{
		FEntryDebugInfo EntryDebugInfo;
		EntryDebugInfo.CameraRigName = GetNameSafe(Entry.CameraRig);
		Entries.Add(EntryDebugInfo);
	}
}

void FBlendStackCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	TArrayView<FCameraDebugBlock*> ChildrenView(GetChildren());

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		const FEntryDebugInfo& Entry(Entries[Index]);

		Renderer.AddText(TEXT("{cam_passive}[%d]{cam_highlighted} %s\n"), Index + 1, *Entry.CameraRigName);

		if (ChildrenView.IsValidIndex(Index))
		{
			Renderer.AddIndent();
			ChildrenView[Index]->DebugDraw(Params, Renderer);
			Renderer.RemoveIndent();
		}
	}

	// We've already manually renderered our children blocks.
	Renderer.SkipAllBlocks();
}

void FBlendStackCameraDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << Entries;
}

FArchive& operator<< (FArchive& Ar, FBlendStackCameraDebugBlock::FEntryDebugInfo& EntryDebugInfo)
{
	Ar << EntryDebugInfo.CameraRigName;
	return Ar;
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

