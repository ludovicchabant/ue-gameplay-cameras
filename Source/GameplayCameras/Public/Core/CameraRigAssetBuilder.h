// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "Templates/Tuple.h"
#include "UObject/WeakObjectPtr.h"

class FStructProperty;
class UCameraNode;
class UCameraRigAsset;
class UCameraVariableAsset;
struct FCameraVariableTableAllocationInfo;

namespace UE::Cameras
{

namespace Internal { struct FPrivateVariableBuilder; }

/**
 * A class that can prepare a camera rig for runtime use.
 *
 * This builder class sets up internal camera variables that handle exposed camera
 * rig parameters, computes the allocation information of the camera rig, and
 * does various kinds of validation.
 *
 * Once the build process is done, the BuildStatus proeprty is set on the camera rig.
 */
class FCameraRigAssetBuilder
{
public:

	/** Builds the given camera rig. */
	void BuildCameraRig(UCameraRigAsset* InCameraRig);

private:

	void BuildCameraRigImpl();

	void FlattenCameraNodeHierarchy();

	void GatherOldDrivenParameters();
	void BuildNewDrivenParameters();
	void DiscardUnusedPrivateVariables();

	void BuildAllocationInfo();
	void BuildAllocationInfo(const UCameraNode* CameraNode);

	void UpdateBuildStatus();

private:

	UCameraRigAsset* CameraRig = nullptr;

	TArray<UCameraNode*> FlattenedNodes;

	using FDrivenParameterKey = TTuple<FStructProperty*, UCameraNode*>;
	TMap<FDrivenParameterKey, UCameraVariableAsset*> OldDrivenParameters;

	FCameraRigAllocationInfo AllocationInfo;

	bool bHasErrors;
	bool bHasWarnings;

	friend struct Internal::FPrivateVariableBuilder;
};

}  // namespace UE::Cameras

