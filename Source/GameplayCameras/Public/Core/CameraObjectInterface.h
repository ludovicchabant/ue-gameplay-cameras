// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Core/CameraContextDataTableFwd.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ObjectTreeGraphObject.h"
#include "UObject/ObjectPtr.h"

#include "CameraObjectInterface.generated.h"

class UCameraNode;
struct FCameraContextDataDefinition;
struct FCameraVariableDefinition;

/**
 * Base class for interface parameters on a camera rig asset.
 */
UCLASS(MinimalAPI, meta=(ObjectTreeGraphSelfPinDirection="Output"))
class UCameraObjectInterfaceParameterBase 
	: public UObject
	, public IObjectTreeGraphObject
{
	GENERATED_BODY()

public:

	/** The exposed name for this parameter. */
	UPROPERTY(EditAnywhere, Category=Camera)
	FString InterfaceParameterName;

	/** The camera node this parameter is connected to. */
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
class UCameraObjectInterfaceBlendableParameter : public UCameraObjectInterfaceParameterBase
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
class UCameraObjectInterfaceDataParameter : public UCameraObjectInterfaceParameterBase
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
 * Structure defining the public data interface of a camera object.
 */
USTRUCT()
struct FCameraObjectInterface
{
	GENERATED_BODY()

public:

	/** The list of exposed blendable parameters on the camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraObjectInterfaceBlendableParameter>> BlendableParameters;

	/** The list of exposed data parameters on the camera rig. */
	UPROPERTY(Instanced)
	TArray<TObjectPtr<UCameraObjectInterfaceDataParameter>> DataParameters;

public:
	
	/** Finds an exposed parameter by name. */
	GAMEPLAYCAMERAS_API UCameraObjectInterfaceBlendableParameter* FindBlendableParameterByName(const FString& ParameterName) const;
	GAMEPLAYCAMERAS_API UCameraObjectInterfaceDataParameter* FindDataParameterByName(const FString& ParameterName) const;

	/** Finds an exposed parameter by Guid. */
	GAMEPLAYCAMERAS_API UCameraObjectInterfaceBlendableParameter* FindBlendableParameterByGuid(const FGuid& ParameterGuid) const;
	GAMEPLAYCAMERAS_API UCameraObjectInterfaceDataParameter* FindDataParameterByGuid(const FGuid& ParameterGuid) const;

	/** Returns whether an exposed parameter with the given name exists. */
	GAMEPLAYCAMERAS_API bool HasBlendableParameter(const FString& ParameterName) const;

public:

	// Deprecated methods.
	UE_DEPRECATED(5.6, "Camera rigs are now all standalone assets and don't need a separate display name.")
	FString GetDisplayName() const { return DisplayName_DEPRECATED; }

private:

	// Deprecated properties.

	UPROPERTY()
	FString DisplayName_DEPRECATED;
};

