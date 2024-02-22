// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/RootCameraNode.h"

#include "Core/CameraRigAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RootCameraNode)

void FRootCameraNodeEvaluator::ActivateCameraRig(const FActivateCameraRigParams& Params)
{
	OnActivateCameraRig(Params);
}

