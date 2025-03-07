// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/CameraContextDataTableAllocationInfo.h"
#include "Core/CameraEventHandler.h"
#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraObjectInterface.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "Core/CameraVariableTableAllocationInfo.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"

#include "BaseCameraObject.generated.h"

class UCameraNode;

namespace UE::Cameras
{
	class FCameraBuildLog;
	class FCameraObjectInterfaceParameterBuilder;

	/**
	 * Interface for listening to changes on a camera rig asset.
	 */
	class ICameraObjectEventHandler
	{
	public:
		virtual ~ICameraObjectEventHandler() {}

		/** Called when the camera object's interface has changed. */
		virtual void OnCameraObjectInterfaceChanged() {}
	};
}

/**
 * Structure describing various allocations needed by a camera rig.
 */
USTRUCT()
struct FCameraObjectAllocationInfo
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

	GAMEPLAYCAMERAS_API void Append(const FCameraObjectAllocationInfo& OtherAllocationInfo);

	bool operator==(const FCameraObjectAllocationInfo& Other) const = default;
};

template<>
struct TStructOpsTypeTraits<FCameraObjectAllocationInfo> : public TStructOpsTypeTraitsBase2<FCameraObjectAllocationInfo>
{
	enum
	{
		WithCopy = true,
		WithIdenticalViaEquality = true
	};
};

UCLASS(Abstract, MinimalAPI)
class UBaseCameraObject : public UObject
{
	GENERATED_BODY()

public:

	/** The public data interface of this camera object. */
	UPROPERTY()
	FCameraObjectInterface Interface;

	/** Event handlers to be notified of data changes. */
	UE::Cameras::TCameraEventHandlerContainer<UE::Cameras::ICameraObjectEventHandler> EventHandlers;

	/** Allocation information for all the nodes and variables in this camera object. */
	UPROPERTY()
	FCameraObjectAllocationInfo AllocationInfo;

public:

	/** Gets the default values for the parameters exposed on this camera rig. */
	const FInstancedPropertyBag& GetDefaultParameters() const { return DefaultParameters; }

	/** Gets the default values for the parameters exposed on this camera rig. */
	FInstancedPropertyBag& GetDefaultParameters() { return DefaultParameters; }

	/** Gets the definitions of parameters exposed on this camera rig. */
	TConstArrayView<FCameraObjectInterfaceParameterDefinition> GetParameterDefinitions() const { return ParameterDefinitions; }

public:

	/** Get the root node of this camera object. */
	virtual UCameraNode* GetRootNode() { return nullptr; }

private:

	/** The default interface parameter values, generated during build. */
	UPROPERTY()
	FInstancedPropertyBag DefaultParameters;

	/** The definitions of parameters exposed on this camera rig. */
	UPROPERTY()
	TArray<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions;

	friend class UE::Cameras::FCameraObjectInterfaceParameterBuilder;
};

