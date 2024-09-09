// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/ActivateCameraRigFunctions.h"

#include "Core/RootCameraNode.h"
#include "GameFramework/ControllerGameplayCameraEvaluationComponent.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActivateCameraRigFunctions)

void UActivateCameraRigFunctions::ActivatePersistentBaseCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRigImpl(WorldContextObject, PlayerController, CameraRig, ECameraRigLayer::Base);
}

void UActivateCameraRigFunctions::ActivatePersistentGlobalCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRigImpl(WorldContextObject, PlayerController, CameraRig, ECameraRigLayer::Global);
}

void UActivateCameraRigFunctions::ActivatePersistentVisualCameraRig(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRigImpl(WorldContextObject, PlayerController, CameraRig, ECameraRigLayer::Visual);
}

void UActivateCameraRigFunctions::ActivateCameraRigImpl(UObject* WorldContextObject, APlayerController* PlayerController, UCameraRigAsset* CameraRig, ECameraRigLayer EvaluationLayer)
{
	using namespace UE::Cameras;

	if (!CameraRig)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No camera rig was given to activate!"));
		return;
	}

	UControllerGameplayCameraEvaluationComponent* CameraEvaluationComponent = UControllerGameplayCameraEvaluationComponent::FindOrAddComponent(PlayerController);
	if (ensure(CameraEvaluationComponent))
	{
		CameraEvaluationComponent->ActivateCameraRig(CameraRig, EvaluationLayer);
	}
}

