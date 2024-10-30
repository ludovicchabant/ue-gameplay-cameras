// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraNodeEvaluatorHierarchy.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigEvaluationInfo.h"
#include "Debug/CameraDebugBlock.h"
#include "IGameplayCamerasLiveEditListener.h"

#include "BlendStackCameraNode.generated.h"

class UBlendStackRootCameraNode;
class UCameraAsset;
class UCameraRigAsset;
class UCameraRigTransition;

namespace UE::Cameras
{

class FBlendStackRootCameraNodeEvaluator;
class FCameraEvaluationContext;
class FCameraSystemEvaluator;
enum class EBlendStackCameraRigEventType;
struct FBlendStackCameraRigEvent;

#if UE_GAMEPLAY_CAMERAS_DEBUG
class FBlendStackCameraDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#if WITH_EDITOR
class IGameplayCamerasLiveEditManager;
#endif

}  // namespace UE::Cameras

/**
 * Describes a type of blend stack.
 */
UENUM()
enum class ECameraBlendStackType
{
	/**
	 * Camera rigs are evaluated in isolation before being blended together, and get 
	 * automatically popped out of the stack when another rig has reached 100% blend above 
	 * them.
	 */
	IsolatedTransient,
	/**
	 * Camera rigs are evaluated in an additive way, i.e. the result of a lower camera rig
	 * becomes the input of the next ones. Also, camera rigs stay in the stack until explicitly 
	 * removed.
	 */
	AdditivePersistent
};

/**
 * A blend stack implemented as a camera node.
 */
UCLASS(MinimalAPI, Hidden)
class UBlendStackCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** 
	 * The type of blend stack this should run as.
	 */
	UPROPERTY()
	ECameraBlendStackType BlendStackType = ECameraBlendStackType::IsolatedTransient;
};

namespace UE::Cameras
{

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlendStackCameraRigEvent, const FBlendStackCameraRigEvent&);

/**
 * Evaluator for a blend stack camera node.
 */
class FBlendStackCameraNodeEvaluator 
	: public TCameraNodeEvaluator<UBlendStackCameraNode>
#if WITH_EDITOR
	, public IGameplayCamerasLiveEditListener
#endif
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FBlendStackCameraNodeEvaluator)

public:

	~FBlendStackCameraNodeEvaluator();

	/** Returns information about the top (active) camera rig, if any. */
	FCameraRigEvaluationInfo GetActiveCameraRigEvaluationInfo() const;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FBlendStackCameraDebugBlock* BuildDetailedDebugBlock(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

public:

	/** Gets the delegate for blend stack events. */
	FOnBlendStackCameraRigEvent& OnCameraRigEvent() { return OnCameraRigEventDelegate; }

protected:

	// FCameraNodeEvaluator interface
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;
	virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#if WITH_EDITOR
	// IGameplayCamerasLiveEditListener interface
	virtual void OnPostBuildAsset(const FGameplayCameraAssetBuildEvent& BuildEvent) override;
#endif

protected:

	struct FCameraRigEntry;

	bool InitializeEntry(
		FCameraRigEntry& NewEntry, 
		const UCameraRigAsset* CameraRig,
		TSharedPtr<const FCameraEvaluationContext> EvaluationContext,
		UBlendStackRootCameraNode* EntryRootNode);

	void FreezeEntry(FCameraRigEntry& Entry);

	void PopEntry(int32 EntryIndex);
	void PopEntries(int32 FirstIndexToKeep);

	void BroadcastCameraRigEvent(EBlendStackCameraRigEventType EventType, const FCameraRigEntry& Entry, const UCameraRigTransition* Transition = nullptr) const;

#if WITH_EDITOR
	void AddPackageListeners(FCameraRigEntry& Entry);
	void RemoveListenedPackages(FCameraRigEntry& Entry);
	void RemoveListenedPackages(TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager, FCameraRigEntry& Entry);
#endif

protected:

	struct FResolvedEntry
	{
		FCameraRigEntry& Entry;
		TSharedPtr<const FCameraEvaluationContext> Context;
		int32 EntryIndex;
		bool bHasPreBlendedParameters;
	};

	void ResolveEntries(TArray<FResolvedEntry>& OutResolvedEntries);
	void OnRunFinished();

protected:

	struct FCameraRigEntry
	{
		/** Evaluation context in which this entry runs. */
		TWeakPtr<const FCameraEvaluationContext> EvaluationContext;
		/** The camera rig asset that this entry runs. */
		TObjectPtr<const UCameraRigAsset> CameraRig;
		/** The root node. */
		TObjectPtr<UBlendStackRootCameraNode> RootNode;
		/** Storage buffer for all evaluators in this node tree. */
		FCameraNodeEvaluatorStorage EvaluatorStorage;
		/** Root evaluator. */
		FBlendStackRootCameraNodeEvaluator* RootEvaluator = nullptr;
		/** The evaluator tree. */
		FCameraNodeEvaluatorHierarchy EvaluatorHierarchy;
		/** Result for this node tree. */
		FCameraNodeEvaluationResult Result;
		/** Whether this is the first frame this entry runs. */
		bool bIsFirstFrame = false;
		/** Whether the context's initial result was valid last frame. */
		bool bWasContextInitialResultValid = false;
		/** Whether input slots were run (possibly from a preview update). */
		bool bInputRunThisFrame = false;
		/** Whether the blend node was run (possibly from a preview update). */
		bool bBlendRunThisFrame = false;
		/** Whether to force a camera cut on this entry this frame. */
		bool bForceCameraCut = false;
		/** Whether this entry is frozen. */
		bool bIsFrozen = false;

#if UE_GAMEPLAY_CAMERAS_TRACE
		bool bLogWarnings = true;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

#if WITH_EDITOR
		TArray<TWeakObjectPtr<const UPackage>, TInlineAllocator<4>> ListenedPackages;
#endif  // WITH_EDITOR
	};

	/** The camera system evaluator running this node. */
	FCameraSystemEvaluator* OwningEvaluator = nullptr;

	/** Entries in the blend stack. */
	TArray<FCameraRigEntry> Entries;

	/** The delegate to invoke when an event occurs in this blend stack. */
	FOnBlendStackCameraRigEvent OnCameraRigEventDelegate;

#if WITH_EDITOR
	TMap<TWeakObjectPtr<const UPackage>, int32> AllListenedPackages;
#endif  // WITH_EDITOR

#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class FBlendStackSummaryCameraDebugBlock;
	friend class FBlendStackCameraDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

/**
 * Parameter structure for pushing a camera rig onto a transient blend stack.
 */
struct FBlendStackCameraPushParams
{
	/** The evaluation context within which a camera rig's node tree should run. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset to instantiate and push on the blend stack. */
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

/**
 * Parameter structure for freezing a camera rig inside a transient blend stack.
 */
struct FBlendStackCameraFreezeParams
{
	/** The evaluation context within which a camera rig's node tree is running. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset that is running. */
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

/**
 * Evaluator for a transient blend stack.
 */
class FTransientBlendStackCameraNodeEvaluator 
	: public FBlendStackCameraNodeEvaluator
{

	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FTransientBlendStackCameraNodeEvaluator, FBlendStackCameraNodeEvaluator)

public:

	/** Push a new camera rig onto the blend stack. */
	void Push(const FBlendStackCameraPushParams& Params);

	/** Freeze a camera rig. */
	void Freeze(const FBlendStackCameraFreezeParams& Params);

	/** Freeze all camera rigs that belong to a given evaluation context. */
	void FreezeAll(TSharedPtr<FCameraEvaluationContext> EvaluationContext);

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	// Update methods.
	void InternalPreBlendPrepare(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);
	void InternalPreBlendExecute(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);
	void InternalUpdate(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);
	void InternalPostBlendExecute(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

	// Utility functions for finding an appropriate transition.
	const UCameraRigTransition* FindTransition(const FBlendStackCameraPushParams& Params) const;
	const UCameraRigTransition* FindTransition(
			TArrayView<const TObjectPtr<UCameraRigTransition>> Transitions, 
			const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset) const;

	void PushVariantEntry(const FBlendStackCameraPushParams& Params, const UCameraRigTransition* Transition);
	void PushNewEntry(const FBlendStackCameraPushParams& Params, const UCameraRigTransition* Transition);

};

/**
 * Parameter structure for inserting a camera rig into a persistent blend stack.
 */
struct FBlendStackCameraInsertParams
{
	/** The evaluation context within which a camera rig's node tree should run. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset to instantiate and push on the blend stack. */
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

/**
 * Parameter structure for removing a camera rig from a persistent blend stack.
 */
struct FBlendStackCameraRemoveParams
{
	/** The evaluation context within which a camera rig's node tree should run. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset to instantiate and push on the blend stack. */
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

/**
 * Evaluator for a persistent blend stack.
 */
class FPersistentBlendStackCameraNodeEvaluator 
	: public FBlendStackCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(GAMEPLAYCAMERAS_API, FPersistentBlendStackCameraNodeEvaluator, FBlendStackCameraNodeEvaluator)

public:

	/** Insert a new camera rig onto the blend stack. */
	void Insert(const FBlendStackCameraInsertParams& Params);

	/** Remove an existing camera rig from the blend stack. */
	void Remove(const FBlendStackCameraRemoveParams& Params);

protected:

	// FCameraNodeEvaluator interface.
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	// Update methods.
	void InternalUpdate(TArrayView<FResolvedEntry> ResolvedEntries, const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

	// Utility functions for finding an appropriate transition.
	const UCameraRigTransition* FindTransition(const FBlendStackCameraPushParams& Params) const;
};

#if UE_GAMEPLAY_CAMERAS_DEBUG

class FBlendStackSummaryCameraDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(GAMEPLAYCAMERAS_API, FBlendStackSummaryCameraDebugBlock)

public:

	FBlendStackSummaryCameraDebugBlock();
	FBlendStackSummaryCameraDebugBlock(const FBlendStackCameraNodeEvaluator& InEvaluator);

protected:

	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;
	virtual void OnSerialize(FArchive& Ar) override;

private:

	int32 NumEntries;
	ECameraBlendStackType BlendStackType;
};

class FBlendStackCameraDebugBlock : public FCameraDebugBlock
{
	UE_DECLARE_CAMERA_DEBUG_BLOCK(GAMEPLAYCAMERAS_API, FBlendStackCameraDebugBlock)

public:

	FBlendStackCameraDebugBlock();
	FBlendStackCameraDebugBlock(const FBlendStackCameraNodeEvaluator& InEvaluator);
	
protected:

	virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override;
	virtual void OnSerialize(FArchive& Ar) override;

private:

	struct FEntryDebugInfo
	{
		FString CameraRigName;
	};

	TArray<FEntryDebugInfo> Entries;

	friend FArchive& operator<< (FArchive&, FEntryDebugInfo&);
};

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

