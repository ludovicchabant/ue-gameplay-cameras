// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"

#include "CameraRigCameraNode.generated.h"

namespace UE::Cameras
{
	class FCameraRigCameraNodeEvaluator;
}

/**
 * A camera node that runs a camera rig's own node tree.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Utility"))
class UCameraRigCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual void OnPreBuild(FCameraBuildLog& BuildLog) override;
	virtual void OnBuild(FCameraRigBuildContext& BuildContext) override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

	// UObject interface.
	void PostLoad() override;

public:

	/** The camera rig to run. */
	UPROPERTY(EditAnywhere, Category=Common, meta=(ObjectTreeGraphHidden=true))
	FCameraRigAssetReference CameraRigReference;

private:

	// Deprecated properties, predating FCameraRigAssetReference

	UPROPERTY()
	TObjectPtr<UCameraRigAsset> CameraRig_DEPRECATED;

	UPROPERTY()
	TArray<FBooleanCameraRigParameterOverride> BooleanOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FInteger32CameraRigParameterOverride> Integer32Overrides_DEPRECATED;
	UPROPERTY()
	TArray<FFloatCameraRigParameterOverride> FloatOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FDoubleCameraRigParameterOverride> DoubleOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FVector2fCameraRigParameterOverride> Vector2fOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FVector2dCameraRigParameterOverride> Vector2dOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FVector3fCameraRigParameterOverride> Vector3fOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FVector3dCameraRigParameterOverride> Vector3dOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FVector4fCameraRigParameterOverride> Vector4fOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FVector4dCameraRigParameterOverride> Vector4dOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FRotator3fCameraRigParameterOverride> Rotator3fOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FRotator3dCameraRigParameterOverride> Rotator3dOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FTransform3fCameraRigParameterOverride> Transform3fOverrides_DEPRECATED;
	UPROPERTY()
	TArray<FTransform3dCameraRigParameterOverride> Transform3dOverrides_DEPRECATED;
};

