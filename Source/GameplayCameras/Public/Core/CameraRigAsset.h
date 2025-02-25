// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildStatus.h"
#include "Core/CameraContextDataTableAllocationInfo.h"
#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraEventHandler.h"
#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraRigParameterDefinition.h"
#include "Core/CameraRigTransition.h"
#include "Core/CameraVariableTableAllocationInfo.h"
#include "Core/ObjectTreeGraphObject.h"
#include "Core/ObjectTreeGraphRootObject.h"
#include "CoreTypes.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "StructUtils/PropertyBag.h"
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

		/** Called when the camera rig's interface has changed. */
		virtual void OnCameraRigInterfaceChanged() {}

#if WITH_EDITOR
		virtual void OnObjectAddedToGraph(const FName GraphName, UObject* Object) {}
		virtual void OnObjectRemovedFromGraph(const FName GraphName, UObject* Object) {}
#endif  // WITH_EDITOR
	};
}

/**
 * Structure describing various allocations needed by a camera rig.
 */
USTRUCT()
struct FCameraRigAllocationInfo
{
	GENERATED_BODY()

	/** Allocation info for node evaluators. */
	UPROPERTY()
	FCameraNodeEvaluatorAllocationInfo EvaluatorInfo;

	/** Allocation info for the variable table. */
	UPROPERTY()
	FCameraVariableTableAllocationInfo VariableTableInfo;

	/** Allocation info for the context data table. */
	UPROPERTY()
	FCameraContextDataTableAllocationInfo ContextDataTableInfo;

public:

	GAMEPLAYCAMERAS_API void Append(const FCameraRigAllocationInfo& OtherAllocationInfo);

	bool operator==(const FCameraRigAllocationInfo& Other) const = default;
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
 * Base class for interface parameters on a camera rig asset.
 */
UCLASS(MinimalAPI, meta=(
			ObjectTreeGraphSelfPinDirection="Output"))
class UCameraRigInterfaceParameterBase 
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:

	/** The exposed name for this parameter. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FString InterfaceParameterName;

	/** The camera node this parameter is connected. */
	UPROPERTY(meta=(ObjectTreeGraphHidden=true))
	TObjectPtr<UCameraNode> Target;

	/**
	 * The name of the property this parameter is connected to on the target camera node.
	 * This may be an actual UObject property, but it may be something else, like the name
	 * of an interface parameter on a nested camera rig, or the name of a Blueprint property
	 * on the evaluator class of a Blueprint camera node.
	 */
	UPROPERTY()
	FName TargetPropertyName;

#if WITH_EDITORONLY_DATA

	/** Whether this parameter has been added to the node graph in the editor. */
	UPROPERTY()
	bool bHasGraphNode = false;

#endif  // WITH_EDITORONLY_DATA

public:

	/** Gets this parameter's unique ID. */
	const FGuid& GetGuid() const { return Guid; }

protected:

	/** The Guid of this parameter. */
	UPROPERTY()
	FGuid Guid;

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
 * An exposed camera rig parameter that drives a specific parameter on one of
 * its camera nodes.
 */
UCLASS(MinimalAPI)
class UCameraRigBlendableParameter : public UCameraRigInterfaceParameterBase
{
	GENERATED_BODY()

public:

	/** The type of this parameter. */
	UPROPERTY()
	ECameraVariableType ParameterType = ECameraVariableType::Boolean;

	/** The struct type of this parameter if it is a blendable struct. */
	UPROPERTY()
	TObjectPtr<const UScriptStruct> BlendableStructType;

	/**
	 * Whether this parameter's value should be pre-blended.
	 *
	 * Pre-blending means that if two blending camera rigs share this parameter, 
	 * each of their values will be blended in a first evaluation pass, and then
	 * both camera rigs will evaluate with the same blended value.
	 */
	UPROPERTY()
	bool bIsPreBlended = true;

	// Built on save/cook.

	/** The ID to use to access the underlying variable value in the variable table. */
	UPROPERTY()
	FCameraVariableID PrivateVariableID;


	// Deprecated.

	UPROPERTY()
	TObjectPtr<UCameraVariableAsset> PrivateVariable_DEPRECATED;

public:

	/** Gets the camera variable definition for this parameter. */
	GAMEPLAYCAMERAS_API FCameraVariableDefinition GetVariableDefinition() const;

#if WITH_EDITORONLY_DATA
	GAMEPLAYCAMERAS_API FString GetVariableName() const;
#endif
};

UCLASS(MinimalAPI)
class UCameraRigDataParameter : public UCameraRigInterfaceParameterBase
{
	GENERATED_BODY()

public:

	/** The type of this parameter. */
	UPROPERTY()
	ECameraContextDataType DataType = ECameraContextDataType::Name;

	/** The type of container for this parameter. */
	UPROPERTY()
	ECameraContextDataContainerType DataContainerType = ECameraContextDataContainerType::None;

	/** An additional type object for this parameter. */
	UPROPERTY()
	TObjectPtr<const UObject> DataTypeObject;

	// Built on save/cook.

	/** The ID to use to access the underlying data in the context data table. */
	UPROPERTY()
	FCameraContextDataID PrivateDataID;

public:

	/** Gets the camera context data definition for this parameter. */
	GAMEPLAYCAMERAS_API FCameraContextDataDefinition GetDataDefinition() const;

#if WITH_EDITORONLY_DATA
	GAMEPLAYCAMERAS_API FString GetDataName() const;
#endif
};

/**
 * Structure defining the public data interface of a camera rig asset.
 */
USTRUCT()
struct FCameraRigInterface
{
	GENERATED_BODY()

public:

	/** The list of exposed blendable parameters on the camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigBlendableParameter>> BlendableParameters;

	/** The list of exposed data parameters on the camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigDataParameter>> DataParameters;

public:
	
	/** Finds an exposed parameter by name. */
	GAMEPLAYCAMERAS_API UCameraRigBlendableParameter* FindBlendableParameterByName(const FString& ParameterName) const;
	GAMEPLAYCAMERAS_API UCameraRigDataParameter* FindDataParameterByName(const FString& ParameterName) const;

	/** Finds an exposed parameter by Guid. */
	GAMEPLAYCAMERAS_API UCameraRigBlendableParameter* FindBlendableParameterByGuid(const FGuid& ParameterGuid) const;
	GAMEPLAYCAMERAS_API UCameraRigDataParameter* FindDataParameterByGuid(const FGuid& ParameterGuid) const;

	/** Returns whether an exposed parameter with the given name exists. */
	GAMEPLAYCAMERAS_API bool HasBlendableParameter(const FString& ParameterName) const;
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
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigTransition>> EnterTransitions;

	/** List of exist transitions for this camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraRigTransition>> ExitTransitions;
	
	/** Default orientation initialization when this camera rig is activated. */
	UPROPERTY(EditAnywhere, Category="Transition")
	ECameraRigInitialOrientation InitialOrientation = ECameraRigInitialOrientation::None;

	/** Allocation information for all the nodes and variables in this camera rig. */
	UPROPERTY()
	FCameraRigAllocationInfo AllocationInfo;

	/** Gets the camera rig's unique ID. */
	const FGuid& GetGuid() const { return Guid; }

	/** Gets the default values for the parameters exposed on this camera rig. */
	const FInstancedPropertyBag& GetDefaultParameters() const { return DefaultParameters; }

	/** Gets the default values for the parameters exposed on this camera rig. */
	FInstancedPropertyBag& GetDefaultParameters() { return DefaultParameters; }

	/** Gets the definitions of parameters exposed on this camera rig. */
	TConstArrayView<FCameraRigParameterDefinition> GetParameterDefinitions() const { return ParameterDefinitions; }

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
	virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
#if WITH_EDITOR
	virtual void GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const override;
#endif

private:

	/** The camera rig's unique ID. */
	UPROPERTY()
	FGuid Guid;

	/** The default interface parameter values, generated during build. */
	UPROPERTY()
	FInstancedPropertyBag DefaultParameters;

	/** The definitions of parameters exposed on this camera rig. */
	UPROPERTY()
	TArray<FCameraRigParameterDefinition> ParameterDefinitions;

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

