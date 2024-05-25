// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraRigTransition.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ObjectTreeGraphObject.h"
#include "Core/ObjectTreeGraphRootObject.h"
#include "CoreTypes.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "UObject/ObjectPtr.h"

#include "CameraRigAsset.generated.h"

class UCameraNode;
class UCameraVariableAsset;

namespace UE::Cameras { class FCameraRigAssetBuilder; }

/**
 *
 */
UENUM()
enum class ECameraRigBuildStatus : uint8
{
	Clean,
	CleanWithWarnings,
	WithErrors,
	Dirty
};

/**
 * Structure describing various allocations needed by a camera node.
 */
USTRUCT()
struct FCameraRigAllocationInfo
{
	GENERATED_BODY()

	/** Allocation info for node evaluators. */
	UPROPERTY()
	FCameraNodeEvaluatorAllocationInfo EvaluatorInfo;

	/** Allocation info for the camera variable. */
	UPROPERTY()
	FCameraVariableTableAllocationInfo VariableTableInfo;
};

/**
 * An exposed camera rig parameter that drives a specific parameter on one of
 * its camera nodes.
 */
UCLASS(MinimalAPI)
class UCameraRigInterfaceParameter
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:

	/** The camera node that this parameter drives. */
	UPROPERTY(meta=(ObjectTreeGraphHidden=true))
	TObjectPtr<UCameraNode> Target;

	/** The camera parameter on the target camera node that this parameter drives. */
	UPROPERTY()
	FName TargetPropertyName;

	/** The exposed name for this parameter. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FString InterfaceParameterName;

	// Built on save/cook.

	/**
	 * The private camera variable created to drive the target camera parameter on
	 * the target camera node. This variable is created by the build method on the
	 * camera rig.
	 */
	UPROPERTY()
	TObjectPtr<UCameraVariableAsset> PrivateVariable;

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
#endif

private:

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	int32 GraphNodePosX = 0;

	UPROPERTY()
	int32 GraphNodePosY = 0;

#endif  // WITH_EDITORONLY_DATA
};

/**
 * Structure defining the public data interface of a camera rig asset.
 */
USTRUCT()
struct FCameraRigInterface
{
	GENERATED_BODY()

public:

	/** The list of exposed parameters on the camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigInterfaceParameter>> InterfaceParameters;

public:
	
	/** Finds an exposed parameter by name. */
	GAMEPLAYCAMERAS_API UCameraRigInterfaceParameter* FindInterfaceParameterByName(const FString& ParameterName) const;

	/** Returns whether an exposed parameter with the given name exists. */
	GAMEPLAYCAMERAS_API bool HasInterfaceParameter(const FString& ParameterName) const;
};

/**
 * List of packages that contain the definition of a camera rig.
 * In most cases there's only one, but with nested assets there could be more.
 */
using FCameraRigPackages = TArray<const UPackage*, TInlineAllocator<4>>;

/**
 * A camera rig asset, which runs a hierarchy of camera nodes to drive 
 * the behavior of a camera.
 */
UCLASS(MinimalAPI)
class UCameraRigAsset
	: public UObject
	, public IGameplayTagAssetInterface
	, public IObjectTreeGraphObject
	, public IObjectTreeGraphRootObject
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	GAMEPLAYCAMERAS_API void GatherPackages(FCameraRigPackages& OutPackages) const;
#endif  // WITH_EDITOR

public:

	/** Root camera node. */
	UPROPERTY(Instanced)
	TObjectPtr<UCameraNode> RootNode;

	/** The gameplay tags on this camera rig. */
	UPROPERTY(EditAnywhere, Category=GameplayTags)
	FGameplayTagContainer GameplayTags;

	/** The public data interface of this camera rig. */
	UPROPERTY()
	FCameraRigInterface Interface;

	/** List of enter transitions for this camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** List of exist transitions for this camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;

	/** Allocation information for all the nodes and variables in this camera rig. */
	UPROPERTY()
	FCameraRigAllocationInfo AllocationInfo;

public:

	/** The current build state of this camera rig. */
	UPROPERTY(Transient)
	ECameraRigBuildStatus BuildStatus = ECameraRigBuildStatus::Dirty;

	/**
	 * Builds this camera rig.
	 * This will validate the data, build the allocation info, and create internal
	 * camera variables for any exposed parameters.
	 */
	GAMEPLAYCAMERAS_API void BuildCameraRig();

public:

	// IGameplayTagAssetInterface.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags() const override { return EObjectTreeGraphObjectSupportFlags::CommentText; }
	virtual const FString& GetGraphNodeCommentText() const override;
	virtual void OnUpdateGraphNodeCommentText(const FString& NewComment) override;
#endif

#if WITH_EDITOR
	virtual void AddConnectableObject(UObject* InObject) override;
	virtual void RemoveConnectableObject(UObject* InObject) override;
#endif

	// UObject interface
	virtual void PostLoad() override;
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

private:

#if WITH_EDITORONLY_DATA

	/** Position of the camera rig node in the graph editor. */
	UPROPERTY()
	int32 GraphNodePosX = 0;

	/** Position of the camera rig node in the graph editor. */
	UPROPERTY()
	int32 GraphNodePosY = 0;

	/** User-written comment in the graph editor. */
	UPROPERTY()
	FString GraphNodeComment;

	/** 
	 * A list of all the camera nodes, including the 'loose' ones that aren't connected
	 * to the root node, and therefore would be GC'ed if we didn't hold them here.
	 */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphHidden=true))
	TArray<TObjectPtr<UCameraNode>> AllNodes;

#endif  // WITH_EDITORONLY_DATA

	friend class UE::Cameras::FCameraRigAssetBuilder;
};

