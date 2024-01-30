// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Math/CriticalDamper.h"
#include "Nodes/CameraNodeTypes.h"

#include "DampenPositionCameraNode.generated.h"

/**
 * A camera node that offsets the location of the camera.
 */
UCLASS(MinimalAPI)
class UDampenPositionCameraNode : public UCameraNode
{
	GENERATED_BODY()

public:	

	UDampenPositionCameraNode(const FObjectInitializer& ObjectInit);

protected:

	virtual void OnInitialize(const FCameraNodeInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult) override;

public:

	UPROPERTY(EditAnywhere, Category=Damping)
	float ForwardDampingFactor = 0.f;

	UPROPERTY(EditAnywhere, Category=Damping)
	float LateralDampingFactor = 0.f;

	UPROPERTY(EditAnywhere, Category=Damping)
	float VerticalDampingFactor = 0.f;

private:

	FCriticalDamper ForwardDamper;
	FCriticalDamper LateralDamper;
	FCriticalDamper VerticalDamper;

	FVector3d PreviousLocation;
};

