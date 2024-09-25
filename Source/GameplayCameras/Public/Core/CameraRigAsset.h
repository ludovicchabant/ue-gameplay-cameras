// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildStatus.h"
#include "Core/CameraEventHandler.h"
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
class UCameraRigAsset;
class UCameraVariableAsset;

namespace UE::Cameras
{
	class FCameraBuildLog;
	class FCameraRigAssetBuilder;

	/**
	 * Interface for listening to changes on a camera rig asset.
	 */
	class ICameraRigAssetEventHandler
	{
	public:
		virtual ~ICameraRigAssetEventHandler() {}

		/** Called when the camera rig asset has been built. */
		virtual void OnCameraRigBuilt(const UCameraRigAsset* CameraRigAsset) {}
	};
}

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

public:

	GAMEPLAYCAMERAS_API void Append(const FCameraRigAllocationInfo& OtherAllocationInfo);

	GAMEPLAYCAMERAS_API friend bool operator==(const FCameraRigAllocationInfo& A, const FCameraRigAllocationInfo& B);
};

template<>
struct TStructOpsTypeTraits<FCameraRigAllocationInfo> : public TStructOpsTypeTraitsBase2<FCameraRigAllocationInfo>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true
	};
};

/**
 * An exposed camera rig parameter that drives a specific parameter on one of
 * its camera nodes.
 */
UCLASS(MinimalAPI, meta=(
			ObjectTreeGraphSelfPinDirection="Output"))
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

	UPROPERTY()
	FGuid Guid;

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
	virtual void GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
#endif

	// UObject interface.
	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

private:

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	FIntVector2 GraphNodePos = FIntVector2::ZeroValue;

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

	UPROPERTY()
	FString DisplayName;

	/** The list of exposed parameters on the camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigInterfaceParameter>> InterfaceParameters;

public:
	
	/** Finds an exposed parameter by name. */
	GAMEPLAYCAMERAS_API UCameraRigInterfaceParameter* FindInterfaceParameterByName(const FString& ParameterName) const;

	/** Finds an exposed parameter by Guid. */
	GAMEPLAYCAMERAS_API UCameraRigInterfaceParameter* FindInterfaceParameterByGuid(const FGuid& ParameterGuid) const;

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
	, public IHasCameraBuildStatus
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
	UPROPERTY(Instanced, meta=(ObjectTreeGraphPinDirection=Input))
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** List of exist transitions for this camera rig. */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphPinDirection=Output))
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;
	
	/** Default orientation initialization when this camera rig is activated. */
	UPROPERTY(EditAnywhere, Category="Transition")
	ECameraRigInitialOrientation InitialOrientation = ECameraRigInitialOrientation::None;

	/** Allocation information for all the nodes and variables in this camera rig. */
	UPROPERTY()
	FCameraRigAllocationInfo AllocationInfo;

	/** Gets the camera rig's unique ID. */
	const FGuid& GetGuid() const { return Guid; }

	/**
	 * Gets the display name of this camera rig.
	 * This is either the display name set on the interface object, or its internal name.
	 */
	GAMEPLAYCAMERAS_API FString GetDisplayName() const;

public:

	/** The current build state of this camera rig. */
	UPROPERTY(Transient)
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Dirty;

	/**
	 * Builds this camera rig.
	 * This will validate the data, build the allocation info, and create internal
	 * camera variables for any exposed parameters.
	 */
	GAMEPLAYCAMERAS_API void BuildCameraRig();

	/**
	 * Builds this camera rig, similar to BuildCameraRig() but using a given build log.
	 */
	GAMEPLAYCAMERAS_API void BuildCameraRig(UE::Cameras::FCameraBuildLog& InBuildLog);

public:

	// IGameplayTagAssetInterface interface.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	// IHasCameraBuildStatus interface.
	virtual ECameraBuildStatus GetBuildStatus() const override { return BuildStatus; }
	virtual void DirtyBuildStatus() override;

public:

	// Graph names for ObjectTreeGraph API.
	GAMEPLAYCAMERAS_API static const FName NodeTreeGraphName;
	GAMEPLAYCAMERAS_API static const FName TransitionsGraphName;

	/** Event handlers to be notified of data changes. */
	UE::Cameras::TCameraEventHandlerContainer<UE::Cameras::ICameraRigAssetEventHandler> EventHandlers;

protected:

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual void GetGraphNodePosition(FName InGraphName, int32& NodePosX, int32& NodePosY) const override;
	virtual void OnGraphNodeMoved(FName InGraphName, int32 NodePosX, int32 NodePosY, bool bMarkDirty) override;
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags(FName InGraphName) const override;
	virtual const FString& GetGraphNodeCommentText(FName InGraphName) const override;
	virtual void OnUpdateGraphNodeCommentText(FName InGraphName, const FString& NewComment) override;
	virtual void GetGraphNodeName(FName InGraphName, FText& OutName) const override;
	virtual void OnRenameGraphNode(FName InGraphName, const FString& NewName) override;
#endif

	// IObjectTreeGraphRootObject interface.
#if WITH_EDITOR
	virtual void GetConnectableObjects(FName InGraphName, TSet<UObject*>& OutObjects) const override;
	virtual void AddConnectableObject(FName InGraphName, UObject* InObject) override;
	virtual void RemoveConnectableObject(FName InGraphName, UObject* InObject) override;
#endif

	// UObject interface
	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

private:

	UPROPERTY()
	FGuid Guid;

#if WITH_EDITORONLY_DATA

	/** Position of the camera rig node in the node graph editor. */
	UPROPERTY()
	FIntVector2 NodeGraphNodePos = FIntVector2::ZeroValue;

	/** Position of the camera rig node in the transition graph editor. */
	UPROPERTY()
	FIntVector2 TransitionGraphNodePos = FIntVector2::ZeroValue;

	/** User-written comment in the node graph editor. */
	UPROPERTY()
	FString NodeGraphNodeComment;

	/** User-written comment in the transition graph editor. */
	UPROPERTY()
	FString TransitionGraphNodeComment;

	/** 
	 * A list of all the camera nodes, including the 'loose' ones that aren't connected
	 * to the root node, and therefore would be GC'ed if we didn't hold them here.
	 */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphHidden=true))
	TArray<TObjectPtr<UObject>> AllNodeTreeObjects;

	/**
	 * Similar to AllNodeTreeObjects, but for the transitions graph.
	 */
	UPROPERTY(Instanced, meta=(ObjectTreeGraphHidden=true))
	TArray<TObjectPtr<UObject>> AllTransitionsObjects;

	// Deprecated properties.

	UPROPERTY()
	int32 GraphNodePosX_DEPRECATED = 0;
	UPROPERTY()
	int32 GraphNodePosY_DEPRECATED = 0;

#endif  // WITH_EDITORONLY_DATA

	friend class UE::Cameras::FCameraRigAssetBuilder;
};

