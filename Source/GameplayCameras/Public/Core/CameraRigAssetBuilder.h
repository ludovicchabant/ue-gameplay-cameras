// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraBuildLog.h"
#include "Core/CameraNodeEvaluatorFwd.h"
#include "Core/CameraNodeHierarchy.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "GameplayCameras.h"
#include "Templates/Tuple.h"

class FStructProperty;
class UCameraNode;
class UCameraRigAsset;
class UCameraRigCameraNode;
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
 * Once the build process is done, the BuildStatus property is set on the camera rig.
 */
class GAMEPLAYCAMERAS_API FCameraRigAssetBuilder
{
public:

	DECLARE_DELEGATE_TwoParams(FCustomBuildStep, UCameraRigAsset*, FCameraBuildLog&);

	/** Creates a new camera rig builder. */
	FCameraRigAssetBuilder(FCameraBuildLog& InBuildLog);

	/** Builds the given camera rig. */
	void BuildCameraRig(UCameraRigAsset* InCameraRig);

	/** Builds the given camera rig. */
	void BuildCameraRig(UCameraRigAsset* InCameraRig, FCustomBuildStep InCustomBuildStep);

private:

	void BuildCameraRigImpl();

	void BuildCameraNodeHierarchy();

	void CallPreBuild();

	void GatherOldDrivenParameters();
	void BuildNewDrivenParameters();
	void DiscardUnusedPrivateVariables();

	void BuildAllocationInfo();
	void BuildAllocationInfo(UCameraNode* CameraNode);

	void UpdateBuildStatus();

private:

	bool SetupCameraParameterOverride(UCameraRigInterfaceParameter* InterfaceParameter);
	bool SetupInnerCameraRigParameterOverride(UCameraRigInterfaceParameter* InterfaceParameter);

private:

	FCameraBuildLog& BuildLog;

	UCameraRigAsset* CameraRig = nullptr;

	FCameraNodeHierarchy CameraNodeHierarchy;

	using FDrivenParameterKey = TTuple<FStructProperty*, UCameraNode*>;
	TMap<FDrivenParameterKey, UCameraVariableAsset*> OldDrivenParameters;
	using FDrivenOverrideKey = TTuple<FGuid, UCameraRigCameraNode*>;
	TMap<FDrivenOverrideKey, UCameraVariableAsset*> OldDrivenOverrides;

	using FReusableInterfaceParameterInfo = TTuple<UCameraVariableAsset*, bool>;
	TMap<UCameraRigInterfaceParameter*, FReusableInterfaceParameterInfo> OldInterfaceParameters;

	FCameraRigAllocationInfo AllocationInfo;

	friend struct Internal::FPrivateVariableBuilder;
};

}  // namespace UE::Cameras

