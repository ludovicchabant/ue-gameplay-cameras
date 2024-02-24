// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "Templates/Function.h"
#include "UObject/ObjectPtr.h"

class FStructProperty;
class UCameraNode;
class UCameraVariableAsset;
class UScriptStruct;

namespace UE::Cameras
{

/**
 * Class that can compute the allocation info of a camera rig.
 */
class FCameraRigAllocationInfoBuilder
{
public:

	/** Builds the allocation info for the given rig. */
	GAMEPLAYCAMERAS_API void BuildAllocationInfo(const UCameraRigAsset* CameraRigAsset, FCameraRigAllocationInfo& AllocationInfo);

private:

	static void EnsureParameterHandlersInitialized();
	static void GatherVariable(const UCameraVariableAsset* InVariable, FCameraVariableTableAllocationInfo& AllocationInfo);

	void BuildAllocationInfo(const UCameraNode* CameraNode);

private:

	using FParameterHandler = TFunction<void(UObject*, FStructProperty*, FCameraRigAllocationInfo&)>;
	using FParameterHandlerMap = TMap<const UScriptStruct*, FParameterHandler>;

	static bool bParameterHandlersInitialized;
	static FParameterHandlerMap ParameterHandlers;

	FCameraRigAllocationInfo* CurrentAllocationInfo = nullptr;
};

}  // namespace UE::Cameras

