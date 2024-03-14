// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraRigAsset.h"
#include "Debug/CameraDebugBlock.h"
#include "IGameplayCamerasLiveEditListener.h"

#include "BlendStackCameraNode.generated.h"

struct FCameraRigTransition;
class UBlendStackRootCameraNode;
class UCameraAsset;
class UCameraRigAsset;

namespace UE::Cameras
{

class FBlendStackRootCameraNodeEvaluator;
class FCameraEvaluationContext;
class FCameraSystemEvaluator;

#if UE_GAMEPLAY_CAMERAS_DEBUG
class FBlendStackCameraDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

}  // namespace UE::Cameras

/**
 * A blend stack implemented as a camera node.
 */
UCLASS(MinimalAPI)
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

/**
 * Evaluator for a blend stack camera node.
 */
class FBlendStackCameraNodeEvaluator 
	: public TCameraNodeEvaluator<UBlendStackCameraNode>
#if WITH_EDITOR
	, public IGameplayCamerasLiveEditListener
#endif
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FBlendStackCameraNodeEvaluator)

public:

	~FBlendStackCameraNodeEvaluator();

	/** Push a new camera rig onto the blend stack. */
	void Push(const FBlendStackCameraPushParams& Params);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	FBlendStackCameraDebugBlock* BuildDetailedDebugBlock(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder);
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

protected:

	// FCameraNodeEvaluator interface
	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnAddReferencedObjects(FReferenceCollector& Collector) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

#if WITH_EDITOR
	// IGameplayCamerasLiveEditListener interface
	virtual void OnPostBuildAsset(const FGameplayCameraAssetBuildEvent& BuildEvent) override;
#endif

protected:

	// Utility functions for finding an appropriate transition.
	const FCameraRigTransition* FindTransition(const FBlendStackCameraPushParams& Params) const;
	const FCameraRigTransition* FindTransition(
			TArrayView<const FCameraRigTransition> Transitions, 
			const UCameraRigAsset* FromCameraRig, const UCameraAsset* FromCameraAsset, bool bFromFrozen,
			const UCameraRigAsset* ToCameraRig, const UCameraAsset* ToCameraAsset) const;

	void PopEntries(int32 FirstIndexToKeep);

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
		/** Result for this node tree. */
		FCameraNodeEvaluationResult Result;
		/** Whether this is the first frame this entry runs. */
		bool bIsFirstFrame = false;
		/** Whether this entry is frozen. */
		bool bIsFrozen = false;
#if WITH_EDITOR
		FCameraRigPackages ListenedPackages;
#endif  // WITH_EDITOR
	};

	/** The camera system evaluator running this node. */
	FCameraSystemEvaluator* OwningEvaluator = nullptr;

	/** Entries in the blend stack. */
	TArray<FCameraRigEntry> Entries;

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
	UE_DECLARE_CAMERA_DEBUG_BLOCK(FBlendStackSummaryCameraDebugBlock)

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
	UE_DECLARE_CAMERA_DEBUG_BLOCK(FBlendStackCameraDebugBlock)

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

