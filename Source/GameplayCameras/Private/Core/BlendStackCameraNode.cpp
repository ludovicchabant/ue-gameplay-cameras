// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/BlendStackCameraRigEvent.h"
#include "Core/BlendStackRootCameraNode.h"
#include "Core/CameraAsset.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "Core/CameraRigCombinationRegistry.h"
#include "Core/CameraSystemEvaluator.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/CameraNodeEvaluationResultDebugBlock.h"
#include "Debug/CameraPoseDebugBlock.h"
#include "Debug/VariableTableDebugBlock.h"
#include "HAL/IConsoleManager.h"
#include "IGameplayCamerasLiveEditManager.h"
#include "IGameplayCamerasModule.h"
#include "Math/ColorList.h"
#include "Modules/ModuleManager.h"
#include "Nodes/Blends/PopBlendCameraNode.h"
#include "Nodes/Common/CameraRigCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackCameraNode)

FCameraNodeEvaluatorPtr UBlendStackCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;

	switch (BlendStackType)
	{
		case ECameraBlendStackType::AdditivePersistent:
			return Builder.BuildEvaluator<FPersistentBlendStackCameraNodeEvaluator>();
		case ECameraBlendStackType::IsolatedTransient:
			return Builder.BuildEvaluator<FTransientBlendStackCameraNodeEvaluator>();
		default:
			ensure(false);
			return nullptr;
	}
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
UE_DEFINE_CAMERA_NODE_EVALUATOR(FTransientBlendStackCameraNodeEvaluator)
UE_DEFINE_CAMERA_NODE_EVALUATOR(FPersistentBlendStackCameraNodeEvaluator)

FBlendStackCameraNodeEvaluator::~FBlendStackCameraNodeEvaluator()
{
	// Pop all our entries to unregister the live-edit callbacks.
	PopEntries(Entries.Num());
}

bool FBlendStackCameraNodeEvaluator::InitializeEntry(
		FCameraRigEntry& NewEntry, 
		const UCameraRigAsset* CameraRig,
		TSharedPtr<const FCameraEvaluationContext> EvaluationContext,
		UBlendStackRootCameraNode* EntryRootNode)
{
	// Clear the evaluator hierarchy in case we are hot-reloading an entry.
	NewEntry.EvaluatorHierarchy.Reset();

	// Generate the hierarchy of node evaluators inside our storage buffer.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = EntryRootNode;
	BuildParams.AllocationInfo = &CameraRig->AllocationInfo.EvaluatorInfo;
	FCameraNodeEvaluator* RootEvaluator = NewEntry.EvaluatorStorage.BuildEvaluatorTree(BuildParams);
	if (!ensureMsgf(RootEvaluator, TEXT("No root evaluator was created for new camera rig!")))
	{
		return false;
	}

	// Allocate variables in the variable table.
	NewEntry.Result.VariableTable.Initialize(CameraRig->AllocationInfo.VariableTableInfo);

	// Initialize the node evaluators.
	FCameraNodeEvaluatorInitializeParams InitParams(&NewEntry.EvaluatorHierarchy);
	InitParams.Evaluator = OwningEvaluator;
	InitParams.EvaluationContext = EvaluationContext;
	InitParams.LastActiveCameraRigInfo = GetActiveCameraRigEvaluationInfo();
	RootEvaluator->Initialize(InitParams, NewEntry.Result);

	// Wrap up!
	NewEntry.EvaluationContext = EvaluationContext;
	NewEntry.CameraRig = CameraRig;
	NewEntry.RootNode = EntryRootNode;
	NewEntry.RootEvaluator = RootEvaluator->CastThisChecked<FBlendStackRootCameraNodeEvaluator>();
	NewEntry.bWasContextInitialResultValid = EvaluationContext->GetInitialResult().bIsValid;
	NewEntry.bIsFirstFrame = true;

	return true;
}

void FBlendStackCameraNodeEvaluator::FreezeEntry(FCameraRigEntry& Entry)
{
	// Deallocate our node evaluators and clear any pointers we kept to them.
	Entry.EvaluatorStorage.DestroyEvaluatorTree(true);
	Entry.RootEvaluator = nullptr;
	Entry.EvaluatorHierarchy.Reset();

	Entry.RootNode = nullptr;

	Entry.EvaluationContext.Reset();

#if WITH_EDITOR
	RemoveListenedPackages(Entry);
#endif
	
	Entry.bIsFrozen = true;
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
				ActiveEntry.RootEvaluator ? ActiveEntry.RootEvaluator->GetRootEvaluator() : nullptr);
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

void FBlendStackCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	OwningEvaluator = Params.Evaluator;
}

void FBlendStackCameraNodeEvaluator::ResolveEntries(TArray<FResolvedEntry>& OutResolvedEntries)
{
	// Build up these structures so we don't re-resolve evaluation context weak-pointers
	// multiple times in this function..
	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		FCameraRigEntry& Entry(Entries[Index]);
		TSharedPtr<const FCameraEvaluationContext> CurContext = Entry.EvaluationContext.Pin();

		OutResolvedEntries.Add({ Entry, CurContext, Index });

		// While we make these resolved entries, emit warnings and errors as needed.
		if (!Entry.bIsFrozen)
		{
			// Check that we still have a valid context. If not, let's freeze the entry, since
			// we won't be able to evaluate it anymore.
			if (UNLIKELY(!CurContext.IsValid()))
			{
				FreezeEntry(Entry);

#if UE_GAMEPLAY_CAMERAS_TRACE
				if (Entry.bLogWarnings)
				{
					UE_LOG(LogCameraSystem, Warning,
							TEXT("Freezing camera rig '%s' because its evaluation context isn't valid anymore."),
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
#if UE_GAMEPLAY_CAMERAS_TRACE
				if (Entry.bLogWarnings)
				{
					UE_LOG(LogCameraSystem, Warning,
							TEXT("Camera rig '%s' may experience a hitch because its initial result isn't valid."),
							*GetNameSafe(Entry.CameraRig));
					Entry.bLogWarnings = false;
				}
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

				continue;
			}

			// If the context was previously invalid, and this isn't the first frame, flag
			// this update as a camera cut.
			if (UNLIKELY(!Entry.bWasContextInitialResultValid && !Entry.bIsFirstFrame))
			{
				Entry.bForceCameraCut = true;
			}
			Entry.bWasContextInitialResultValid = true;

			// Reset this entry's flags for this frame.
			FCameraNodeEvaluationResult& CurResult = Entry.Result;
			CurResult.CameraPose.ClearAllChangedFlags();
			CurResult.VariableTable.ClearAllWrittenThisFrameFlags();
		}
		// else: frozen entries may have null contexts or invalid initial results
		//       because we're not going to update them anyway. We will however blend
		//       them so we add them to the list of entries too.

#if UE_GAMEPLAY_CAMERAS_TRACE
		// This entry might have has warnings before. It's valid now, so let's
		// re-enable warnings if it becomes invalid again in the future.
		Entry.bLogWarnings = true;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE
	}
}

void FBlendStackCameraNodeEvaluator::OnRunFinished()
{
	// Reset transient flags.
	for (FCameraRigEntry& Entry : Entries)
	{
		Entry.bIsFirstFrame = false;
		Entry.bInputRunThisFrame = false;
		Entry.bBlendRunThisFrame = false;
		Entry.bForceCameraCut = false;
	}
}

void FBlendStackCameraNodeEvaluator::PopEntry(int32 EntryIndex)
{
	if (!ensure(Entries.IsValidIndex(EntryIndex)))
	{
		return;
	}

	FCameraRigEntry& Entry = Entries[EntryIndex];
#if WITH_EDITOR
	RemoveListenedPackages(Entry);
#endif  // WITH_EDITOR

	if (OnCameraRigEventDelegate.IsBound())
	{
		BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Popped, Entry);
	}

	Entries.RemoveAt(EntryIndex);
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
		FCameraRigEntry& FirstEntry = Entries[0];

#if WITH_EDITOR
		RemoveListenedPackages(LiveEditManager, FirstEntry);
#endif  // WITH_EDITOR

		if (OnCameraRigEventDelegate.IsBound())
		{
			BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Popped, FirstEntry);
		}

		Entries.RemoveAt(0);
	}
}

#if WITH_EDITOR

void FBlendStackCameraNodeEvaluator::AddPackageListeners(FCameraRigEntry& Entry)
{
	if (!ensure(Entry.CameraRig))
	{
		return;
	}

	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
	if (!LiveEditManager)
	{
		return;
	}

	FCameraRigPackages EntryPackages;
	Entry.CameraRig->GatherPackages(EntryPackages);

	Entry.ListenedPackages.Reset();
	Entry.ListenedPackages.Append(EntryPackages);

	for (const UPackage* ListenPackage : EntryPackages)
	{
		int32& NumListens = AllListenedPackages.FindOrAdd(ListenPackage, 0);
		if (NumListens == 0)
		{
			LiveEditManager->AddListener(ListenPackage, this);
		}
		++NumListens;
	}
}

void FBlendStackCameraNodeEvaluator::RemoveListenedPackages(FCameraRigEntry& Entry)
{
	IGameplayCamerasModule& GameplayCamerasModule = FModuleManager::GetModuleChecked<IGameplayCamerasModule>("GameplayCameras");
	TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager = GameplayCamerasModule.GetLiveEditManager();
	RemoveListenedPackages(LiveEditManager, Entry);
}

void FBlendStackCameraNodeEvaluator::RemoveListenedPackages(TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager, FCameraRigEntry& Entry)
{
	if (!LiveEditManager)
	{
		return;
	}

	for (TWeakObjectPtr<const UPackage> WeakListenPackage : Entry.ListenedPackages)
	{
		int32* NumListens = AllListenedPackages.Find(WeakListenPackage);
		if (ensure(NumListens))
		{
			--(*NumListens);
			if (*NumListens == 0)
			{
				if (const UPackage* ListenPackage = WeakListenPackage.Get())
				{
					LiveEditManager->RemoveListener(ListenPackage, this);
				}
				AllListenedPackages.Remove(WeakListenPackage);
			}
		}
	}

	Entry.ListenedPackages.Reset();
}

#endif  // WITH_EDITOR

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
			Entry.EvaluatorHierarchy.Reset();

			// Re-assign the root node in case the camera rig's root was changed.
			Entry.RootNode->RootNode = Entry.CameraRig->RootNode;

			// Remove the blend on the root node, since we don't want the reloaded camera rig to re-blend-in
			// for no good reason.
			Entry.RootNode->Blend = NewObject<UPopBlendCameraNode>(Entry.RootNode, NAME_None);

			// Rebuild the evaluator tree.
			const bool bInitialized = InitializeEntry(
					Entry,
					Entry.CameraRig,
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

void FTransientBlendStackCameraNodeEvaluator::Push(const FBlendStackCameraPushParams& Params)
{
	bool bSearchedForTransition = false;
	const UCameraRigTransition* Transition = nullptr;

	if (!Entries.IsEmpty())
	{
		FCameraRigEntry& TopEntry(Entries.Top());
		if (!TopEntry.bIsFrozen 
				&& TopEntry.EvaluationContext == Params.EvaluationContext)
		{
			// Don't push anything is what is being requested is already the active 
			// camera rig.
			if (TopEntry.CameraRig == Params.CameraRig)
			{
				return;
			}

			// See if we can merge the new camera rig onto the active camera rig.
			const EBlendStackEntryComparison Comparison = TopEntry.RootEvaluator->Compare(Params.CameraRig);

			if (Comparison == EBlendStackEntryComparison::Active)
			{
				// This camera rig is already the active one on the merged stack.
				return;
			}

			if (Comparison == EBlendStackEntryComparison::EligibleForMerge)
			{
				// This camera rig can be merged with the one current running. However, we
				// only do it if the transition explicitly allows it.
				bSearchedForTransition = true;
				Transition = FindTransition(Params);

				if (Transition && Transition->bAllowCameraRigMerging)
				{
					PushVariantEntry(Params, Transition);
					return;
				}
			}
		}
	}

	// It's a legitimate new entry in the blend stack.
	if (!bSearchedForTransition)
	{
		bSearchedForTransition = true;
		Transition = FindTransition(Params);
	}

	PushNewEntry(Params, Transition);
}

void FTransientBlendStackCameraNodeEvaluator::PushNewEntry(const FBlendStackCameraPushParams& Params, const UCameraRigTransition* Transition)
{
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
		if (Transition)
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
			Params.EvaluationContext,
			EntryRootNode);
	if (!bInitialized)
	{
		return;
	}

#if WITH_EDITOR
	// Listen to changes to the packages inside which this camera rig is defined. We will hot-reload the
	// camera node evaluators for this camera rig when we detect changes.
	AddPackageListeners(NewEntry);
#endif  // WITH_EDITOR

	// Important: we need to move the new entry here because copying evaluator storage
	// is disabled.
	Entries.Add(MoveTemp(NewEntry));

	if (OnCameraRigEventDelegate.IsBound())
	{
		BroadcastCameraRigEvent(EBlendStackCameraRigEventType::Pushed, Entries.Last(), UsedTransition);
	}
}

void FTransientBlendStackCameraNodeEvaluator::PushVariantEntry(const FBlendStackCameraPushParams& PushParams, const UCameraRigTransition* Transition)
{
	const UCameraRigCameraNode* PrefabNode = Cast<const UCameraRigCameraNode>(PushParams.CameraRig->RootNode);
	const UBlendCameraNode* Blend = Transition ? Transition->Blend : nullptr;

	FCameraRigEntry& TopEntry = Entries.Top();
	FCameraNodeEvaluatorBuilder Builder(TopEntry.EvaluatorStorage);
	FCameraNodeEvaluatorBuildParams BuildParams(Builder);
	TopEntry.RootEvaluator->MergeCameraRig(BuildParams, PrefabNode, Blend);

	// Swap out the camera rig registered as "active" for this entry.
#if WITH_EDITOR
	RemoveListenedPackages(TopEntry);
#endif
	{
		TopEntry.CameraRig = PushParams.CameraRig;
	}
#if WITH_EDITOR
	AddPackageListeners(TopEntry);
#endif
}

void FTransientBlendStackCameraNodeEvaluator::Freeze(const FBlendStackCameraFreezeParams& Params)
{
	for (FCameraRigEntry& Entry : Entries)
	{
		if (!Entry.bIsFrozen && 
				Entry.CameraRig == Params.CameraRig &&
				Entry.EvaluationContext == Params.EvaluationContext)
		{
			FreezeEntry(Entry);
		}
	}
}

void FTransientBlendStackCameraNodeEvaluator::FreezeAll(TSharedPtr<FCameraEvaluationContext> EvaluationContext)
{
	for (FCameraRigEntry& Entry : Entries)
	{
		if (!Entry.bIsFrozen && Entry.EvaluationContext == EvaluationContext)
		{
			FreezeEntry(Entry);
		}
	}
}

void FTransientBlendStackCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	// Validate our entries and resolve evaluation context weak pointers.
	TArray<FResolvedEntry> ResolvedEntries;
	ResolveEntries(ResolvedEntries);

	// Gather parameters to pre-blend, and evaluate blend nodes.
	InternalPreBlendPrepare(ResolvedEntries, Params, OutResult);

	// Blend input variables.
	InternalPreBlendExecute(ResolvedEntries, Params, OutResult);

	// Run the root nodes. They will use the pre-blended inputs from the last step.
	// Frozen entries are skipped, since they only ever use the last result they produced.
	InternalUpdate(ResolvedEntries, Params, OutResult);

	// Now blend all the results, keeping track of blends that have reached 100% so
	// that we can remove any camera rigs below (since they would have been completely
	// blended out by that).
	InternalPostBlendExecute(ResolvedEntries, Params, OutResult);

	// Tidy up.
	OnRunFinished();
}

void FTransientBlendStackCameraNodeEvaluator::InternalPreBlendPrepare(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	for (FResolvedEntry& ResolvedEntry : ResolvedEntries)
	{
		FCameraRigEntry& Entry(ResolvedEntry.Entry);

		if (UNLIKELY(Entry.bIsFrozen))
		{
			continue;
		}

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = ResolvedEntry.Context;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		// Start with the input given to us.
		CurResult.VariableTable.OverrideAll(OutResult.VariableTable);

		// Override it with whatever the evaluation context has set on its result.
		// Evaluation contexts may have private variables we need to pass along, such as when rig parameter
		// overrides have been set on them.
		const FCameraNodeEvaluationResult& ContextResult(ResolvedEntry.Context->GetInitialResult());
		CurResult.VariableTable.Override(ContextResult.VariableTable, ECameraVariableTableFilter::AllPublic | ECameraVariableTableFilter::Private);

		// Gather input parameters if needed (and remember if it was indeed needed).
		if (!Entry.bInputRunThisFrame)
		{
			bool bHasPreBlendedParameters = false;
			FCameraBlendedParameterUpdateParams InputParams(CurParams, CurResult.CameraPose);
			FCameraBlendedParameterUpdateResult InputResult(CurResult.VariableTable);

			Entry.EvaluatorHierarchy.ForEachEvaluator(ECameraNodeEvaluatorFlags::NeedsParameterUpdate,
					[&bHasPreBlendedParameters, &InputParams, &InputResult](FCameraNodeEvaluator* ParameterEvaluator)
					{
						ParameterEvaluator->UpdateParameters(InputParams, InputResult);
						bHasPreBlendedParameters = true;
					});

			ResolvedEntry.bHasPreBlendedParameters = bHasPreBlendedParameters;
			Entry.bInputRunThisFrame = true;
		}

		// Run blends.
		// Note that we pass last frame's camera pose to the Run() method. This may change.
		// Blends aren't expected to use the camera pose to do any logic until BlendResults().
		if (!Entry.bBlendRunThisFrame)
		{
			FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
			if (EntryBlendEvaluator)
			{
				EntryBlendEvaluator->Run(CurParams, CurResult);
			}

			Entry.bBlendRunThisFrame = true;
		}
	}
}

void FTransientBlendStackCameraNodeEvaluator::InternalPreBlendExecute(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	for (FResolvedEntry& ResolvedEntry : ResolvedEntries)
	{
		FCameraRigEntry& Entry(ResolvedEntry.Entry);
		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		if (!Entry.bIsFrozen)
		{
			FCameraNodeEvaluationParams CurParams(Params);
			CurParams.EvaluationContext = ResolvedEntry.Context;
			CurParams.bIsFirstFrame = Entry.bIsFirstFrame;
			FCameraNodePreBlendParams PreBlendParams(CurParams, CurResult.CameraPose, CurResult.VariableTable);

			FCameraNodePreBlendResult PreBlendResult(OutResult.VariableTable);

			FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
			if (EntryBlendEvaluator)
			{
				EntryBlendEvaluator->BlendParameters(PreBlendParams, PreBlendResult);
			}
			else
			{
				OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::Input);
			}
		}
		else
		{
			// Frozen entries still contribute to the blend using their last evaluated values.
			OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::Input);
		}
	}
}

void FTransientBlendStackCameraNodeEvaluator::InternalUpdate(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	for (FResolvedEntry& ResolvedEntry : ResolvedEntries)
	{
		FCameraRigEntry& Entry(ResolvedEntry.Entry);

		if (UNLIKELY(Entry.bIsFrozen))
		{
			continue;
		}

		FCameraNodeEvaluationParams CurParams(Params);
		CurParams.EvaluationContext = ResolvedEntry.Context;
		CurParams.bIsFirstFrame = Entry.bIsFirstFrame;

		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		// Start with the input given to us.
		CurResult.CameraPose = OutResult.CameraPose;
		CurResult.CameraRigJoints.OverrideAll(OutResult.CameraRigJoints);
		CurResult.PostProcessSettings.OverrideAll(OutResult.PostProcessSettings);

		// Override it with whatever the evaluation context has set on its result.
		const FCameraNodeEvaluationResult& ContextResult(ResolvedEntry.Context->GetInitialResult());
		CurResult.CameraPose.OverrideChanged(ContextResult.CameraPose);
		CurResult.bIsCameraCut = OutResult.bIsCameraCut || ContextResult.bIsCameraCut || Entry.bForceCameraCut;
		CurResult.bIsValid = true;

		// Run the camera rig's root node.
		FCameraNodeEvaluator* RootEvaluator = Entry.RootEvaluator->GetRootEvaluator();
		if (RootEvaluator)
		{
			RootEvaluator->Run(CurParams, CurResult);
		}
	}
}

void FTransientBlendStackCameraNodeEvaluator::InternalPostBlendExecute(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	int32 PopEntriesBelow = INDEX_NONE;
	for (FResolvedEntry& ResolvedEntry : ResolvedEntries)
	{
		FCameraRigEntry& Entry(ResolvedEntry.Entry);
		FCameraNodeEvaluationResult& CurResult(Entry.Result);

		if (!Entry.bIsFrozen)
		{
			FCameraNodeEvaluationParams CurParams(Params);
			CurParams.EvaluationContext = ResolvedEntry.Context;
			CurParams.bIsFirstFrame = Entry.bIsFirstFrame;
			FCameraNodeBlendParams BlendParams(CurParams, CurResult);

			FCameraNodeBlendResult BlendResult(OutResult);

			FBlendCameraNodeEvaluator* EntryBlendEvaluator = Entry.RootEvaluator->GetBlendEvaluator();
			if (EntryBlendEvaluator)
			{
				EntryBlendEvaluator->BlendResults(BlendParams, BlendResult);

				if (BlendResult.bIsBlendFull && BlendResult.bIsBlendFinished)
				{
					PopEntriesBelow = ResolvedEntry.EntryIndex;
				}
			}
			else
			{
				OutResult.OverrideAll(CurResult);

				PopEntriesBelow = ResolvedEntry.EntryIndex;
			}
		}
		else
		{
			OutResult.OverrideAll(CurResult);

			PopEntriesBelow = ResolvedEntry.EntryIndex;
		}
	}

	// Pop out camera rigs that have been blended out.
	const UBlendStackCameraNode* BlendStackNode = GetCameraNodeAs<UBlendStackCameraNode>();
	if (BlendStackNode->BlendStackType == ECameraBlendStackType::IsolatedTransient && PopEntriesBelow != INDEX_NONE)
	{
		PopEntries(PopEntriesBelow);
	}
}

const UCameraRigTransition* FTransientBlendStackCameraNodeEvaluator::FindTransition(const FBlendStackCameraPushParams& Params) const
{
	const UBlendStackCameraNode* BlendStackNode = GetCameraNodeAs<UBlendStackCameraNode>();

	TSharedPtr<const FCameraEvaluationContext> ToContext = Params.EvaluationContext;
	const UCameraAsset* ToCameraAsset = ToContext ? ToContext->GetCameraAsset() : nullptr;
	const UCameraRigAsset* ToCameraRig = Params.CameraRig;

	// If the new entry is a combination, look for transitions on its "main" camera rig.
	ToCameraRig = UCombinedCameraRigsCameraNode::GetMainCameraRigIfCombination(ToCameraRig);

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

		// If the top entry is a combination, look for transitions on its "main" camera rig.
		FromCameraRig = UCombinedCameraRigsCameraNode::GetMainCameraRigIfCombination(FromCameraRig);

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
	// else: make the first camera rig in the stack start at 100% blend immediately.

	return nullptr;
}

const UCameraRigTransition* FTransientBlendStackCameraNodeEvaluator::FindTransition(
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
		const bool bConditionsPass = Transition->AllConditionsMatch(MatchParams);
		if (bConditionsPass)
		{
			return Transition;
		}
	}

	return nullptr;
}

void FPersistentBlendStackCameraNodeEvaluator::Insert(const FBlendStackCameraInsertParams& Params)
{
	// See if we already have this camera rig and evaluation context in the stack.
	for (const FCameraRigEntry& Entry : Entries)
	{
		if (!Entry.bIsFrozen &&
				Entry.CameraRig == Params.CameraRig &&
				Entry.EvaluationContext == Params.EvaluationContext)
		{
			return;
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
			EntryRootNode);
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
	OnRunFinished();
}

void FPersistentBlendStackCameraNodeEvaluator::InternalUpdate(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
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
				// overrides have been set on them.
				const FCameraNodeEvaluationResult& ContextResult(ResolvedEntry.Context->GetInitialResult());
				CurResult.CameraPose.OverrideChanged(ContextResult.CameraPose);
				CurResult.VariableTable.Override(ContextResult.VariableTable, ECameraVariableTableFilter::AllPublic | ECameraVariableTableFilter::Private);

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
				OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::Input);
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

			OutResult.VariableTable.Override(CurResult.VariableTable, ECameraVariableTableFilter::Input);
			OutResult.OverrideAll(CurResult);
		}
	}
}

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
			FCameraNodeEvaluator* BlendEvaluator = Entry.RootEvaluator ? Entry.RootEvaluator->GetBlendEvaluator() : nullptr;
			if (BlendEvaluator)
			{
				Builder.StartParentDebugBlockOverride(EntryDebugBlock);
				{
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
	BlendStackType = InEvaluator.GetCameraNodeAs<UBlendStackCameraNode>()->BlendStackType;
}

void FBlendStackSummaryCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText(TEXT("%d entries"), NumEntries);
}

void FBlendStackSummaryCameraDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << NumEntries;
	Ar << BlendStackType;
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
		EntryDebugInfo.CameraRigName = Entry.CameraRig ? Entry.CameraRig->GetDisplayName() : FString("<None>");
		Entries.Add(EntryDebugInfo);
	}
}

void FBlendStackCameraDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	TArrayView<FCameraDebugBlock*> ChildrenView(GetChildren());

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		const FEntryDebugInfo& Entry(Entries[Index]);

		Renderer.AddText(TEXT("{cam_passive}[%d] {cam_notice}%s{cam_default}\n"), Index + 1, *Entry.CameraRigName);

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

