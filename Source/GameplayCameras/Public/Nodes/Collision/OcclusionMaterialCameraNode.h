// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Engine/EngineTypes.h"

#include "OcclusionMaterialCameraNode.generated.h"

class UMaterialInterface;

UCLASS(MinimalAPI, meta=(CameraNodeCategories="Collision"))
class UOcclusionMaterialCameraNode : public UCameraNode
{
	GENERATED_BODY()

public:

	/** Material to apply on objects causing occlusion. */
	UPROPERTY(EditAnywhere, Category="Occlusion")
	TObjectPtr<UMaterialInterface> OcclusionTransparencyMaterial = nullptr;

	/** Radius of the sphere used for occlusion testing. */
	UPROPERTY(EditAnywhere, Category="Occlusion")
	FFloatCameraParameter OcclusionSphereRadius;

	/** Collision channel to use for the occlusion testing. */
	UPROPERTY(EditAnywhere, Category="Occlusion")
	TEnumAsByte<ECollisionChannel> OcclusionChannel;

	/** World-space offset from the target to the occlusion trace's end. */
	UPROPERTY(EditAnywhere, Category="Occlusion")
	FVector3dCameraParameter OcclusionTargetOffset;

public:

	UOcclusionMaterialCameraNode(const FObjectInitializer& ObjectInit);

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;
};

