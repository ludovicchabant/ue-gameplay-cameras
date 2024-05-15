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

	/** Horizontal size of filmback or digital sensor, in mm. */
	UPROPERTY(EditAnywhere, Category="Filmback", meta=(ClampMin="0.001", ForceUnits=mm))
	FFloatCameraParameter SensorWidth;

	/** Vertical size of filmback or digital sensor, in mm. */
	UPROPERTY(EditAnywhere, Category="Filmback", meta=(ClampMin="0.001", ForceUnits=mm))
	FFloatCameraParameter SensorHeight;

	/** Manually-controlled focus distance (manual focus mode only) */
	UPROPERTY(EditAnywhere, Category="Lens Parameters", meta=(Units=cm))
	FFloatCameraParameter FocusDistance;

	/** Current focal length of the camera (i.e. controls FoV, zoom) */
	UPROPERTY(EditAnywhere, Category="Lens Parameters")
	FFloatCameraParameter FocalLength;

	/** Current aperture, in terms of f-stop (e.g. 2.8 for f/2.8) */
	UPROPERTY(EditAnywhere, Category="Lens Parameters")
	FFloatCameraParameter Aperture;
};


