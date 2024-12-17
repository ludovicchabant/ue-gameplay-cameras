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
struct FInstancedPropertyBag;

namespace UE::Cameras
{

namespace Internal { struct FInterfaceParameterBindingBuilder; }

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
	void BuildInterfaceParameters();
	void BuildInterfaceParameterBindings();
	void DiscardUnusedParameters();

	void CallBuild();
	void CallBuild(FCameraRigBuildContext& BuildContext, UCameraNode* CameraNode);

	void BuildDefaultParameters();

	void UpdateBuildStatus();

private:

	bool SetupCameraParameterOrVariableReferenceOverride(const UCameraRigBlendableParameter* BlendableParameter);
	bool SetupCustomBlendableParameterOverride(const UCameraRigBlendableParameter* BlendableParameter);

	bool SetupDataContextPropertyOverride(const UCameraRigDataParameter* DataParameter);
	bool SetupCustomDataParameterOverride(const UCameraRigDataParameter* DataParameter);

public:

	// Internal API.

	static void BuildDefaultParameters(UCameraRigAsset* CameraRigAsset, FInstancedPropertyBag& OutPropertyBag);
	static void AppendDefaultParameters(const FCameraRigInterface& CameraRigInterface, TArray<FPropertyBagPropertyDesc>& OutProperties);

private:

	FCameraBuildLog& BuildLog;

	UCameraRigAsset* CameraRig = nullptr;

	FCameraNodeHierarchy CameraNodeHierarchy;

	using FDrivenParameterKey = TTuple<FName, UCameraNode*>;
	TMap<FDrivenParameterKey, TObjectPtr<UCameraVariableAsset>> OldDrivenBlendableParameters;
	TMap<FDrivenParameterKey, FCameraContextDataID> OldDrivenDataParameters;

	friend struct Internal::FInterfaceParameterBindingBuilder;
};

}  // namespace UE::Cameras

