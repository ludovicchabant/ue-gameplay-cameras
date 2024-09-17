// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Nodes/CameraNodeTypes.h"

#include "LensParametersCameraNode.generated.h"

/**
 * A camera node that sets parameter values on the camera lens.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Lens"))
class ULensParametersCameraNode : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** Current focal length of the camera (i.e. controls FoV, zoom) */
	UPROPERTY(EditAnywhere, Category="Lens Parameters")
	FFloatCameraParameter FocalLength;

	/** Manually-controlled focus distance (manual focus mode only) */
	UPROPERTY(EditAnywhere, Category="Lens Parameters", meta=(Units=cm))
	FFloatCameraParameter FocusDistance;

	/** Current aperture, in terms of f-stop (e.g. 2.8 for f/2.8) */
	UPROPERTY(EditAnywhere, Category="Lens Parameters")
	FFloatCameraParameter Aperture;

	/** Current shutter speed, in 1/seconds */
	UPROPERTY(EditAnywhere, Category="Lens Parameters", DisplayName="Shutter Speed (1/s)")
    FFloatCameraParameter ShutterSpeed;

	/** 
	 * Whether post-process settings should be automatically applied to reflect
	 * the lens settings.
	 */
	UPROPERTY(EditAnywhere, Category="Lens Parameters")
	FBooleanCameraParameter EnablePhysicalCamera = false;
};

