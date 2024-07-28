// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
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
	 * Whether to automatically pop camera rigs out of the stack when another rig
	 * has reached 100% blend above them.
	 */
	UPROPERTY()
	bool bAutoPop = true;

	/**
	 * Whether to blend-in the first camera rig when the stack is previously empty.
	 */
	UPROPERTY()
	bool bBlendFirstCameraRig = false;
};

namespace UE::Cameras
{

/**
 * Parameter structure for pushing a camera rig onto a blend stack.
 */
struct FBlendStackCameraPushParams
{
	/** The evaluator currently running.*/
	FCameraSystemEvaluator* Evaluator = nullptr;

	/** The evaluation context within which a camera rig's node tree should run. */
	TSharedPtr<const FCameraEvaluationContext> EvaluationContext;

	/** The source camera rig asset to instantiate and push on the blend stack. */
	TObjectPtr<const UCameraRigAsset> CameraRig;
};

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

	/** Push a new camera rig onto the blend stack. */
	void Push(const FBlendStackCameraPushParams& Params);

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
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
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

	// Utility functions for finding an appropriate transition.
	const UCameraRigTransition* FindTransition(const FBlendStackCameraPushParams& Params) const;
	const UCameraRigTransition* FindTransition(
			TArrayView<const TObjectPtr<UCameraRigTransition>> Transitions, 
			const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset) const;

	void PopEntries(int32 FirstIndexToKeep);

	bool InitializeEntry(
		FCameraRigEntry& NewEntry, 
		const UCameraRigAsset* CameraRig,
		FCameraSystemEvaluator* Evaluator,
		TSharedPtr<const FCameraEvaluationContext> EvaluationContext,
		UBlendStackRootCameraNode* EntryRootNode);

	void FreezeEntry(FCameraRigEntry& Entry);

	void GatherEntryParameterEvaluators(FCameraNodeEvaluator* RootEvaluator, TArray<FCameraNodeEvaluator*>& OutParameterEvaluators);

	void BroadcastCameraRigEvent(EBlendStackCameraRigEventType EventType, const FCameraRigEntry& Entry, const UCameraRigTransition* Transition = nullptr) const;

#if WITH_EDITOR
	void RemoveListenedPackages(FCameraRigEntry& Entry);
	void RemoveListenedPackages(TSharedPtr<IGameplayCamerasLiveEditManager> LiveEditManager, FCameraRigEntry& Entry);
#endif

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
		/** Evaluators needing parameter update. */
		TArray<FCameraNodeEvaluator*> ParameterEvaluators;
		/** Result for this node tree. */
		FCameraNodeEvaluationResult Result;
		/** Whether this is the first frame this entry runs. */
		bool bIsFirstFrame = false;
		/** Whether input slots were run (possibly from a preview update). */
		bool bInputRunThisFrame = false;
		/** Whether the blend node was run (possibly from a preview update). */
		bool bBlendRunThisFrame = false;
		/** Whether this entry is frozen. */
		bool bIsFrozen = false;

#if UE_GAMEPLAY_CAMERAS_TRACE
		bool bLogWarnings = true;
#endif  // UE_GAMEPLAY_CAMERAS_TRACE

#if WITH_EDITOR
		FCameraRigPackages ListenedPackages;
#endif  // WITH_EDITOR
	};

	/** The camera system evaluator running this node. */
	FCameraSystemEvaluator* OwningEvaluator = nullptr;

	/** Entries in the blend stack. */
	TArray<FCameraRigEntry> Entries;

	/** The delegate to invoke when an event occurs in this blend stack. */
	FOnBlendStackCameraRigEvent OnCameraRigEventDelegate;

#if WITH_EDITOR
	TMap<const UPackage*, int32> AllListenedPackages;
#endif  // WITH_EDITOR

#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class FBlendStackSummaryCameraDebugBlock;
	friend class FBlendStackCameraDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
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

