// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/ActivateCameraRigFunctions.h"

#include "Camera/PlayerCameraManager.h"
#include "Core/CameraRigAsset.h"
#include "Core/RootCameraNode.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCameras.h"
#include "Templates/SharedPointer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActivateCameraRigFunctions)

TSharedPtr<UE::Cameras::FCameraEvaluationContext> UActivateCameraRigFunctions::GlobalContext;

void UActivateCameraRigFunctions::ActivateBaseCameraRig(APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRig(PlayerController, CameraRig, ECameraRigLayer::Base);
}

void UActivateCameraRigFunctions::ActivateGlobalCameraRig(APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRig(PlayerController, CameraRig, ECameraRigLayer::Global);
}

void UActivateCameraRigFunctions::ActivateVisualCameraRig(APlayerController* PlayerController, UCameraRigAsset* CameraRig)
{
	ActivateCameraRig(PlayerController, CameraRig, ECameraRigLayer::Visual);
}

void UActivateCameraRigFunctions::ActivateCameraRig(APlayerController* PlayerController, UCameraRigAsset* CameraRig, ECameraRigLayer EvaluationLayer)
{
	using namespace UE::Cameras;

	if (!CameraRig)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("No camera rig was given to activate!"));
		return;
	}

	if (FCameraSystemEvaluator* SystemEvaluator = FindCameraSystemEvaluator(PlayerController))
	{
		FActivateCameraRigParams Params;
		Params.CameraRig = CameraRig;
		Params.EvaluationContext = EnsureGlobalContext();
		Params.Evaluator = SystemEvaluator;
		Params.Layer = EvaluationLayer;
		SystemEvaluator->GetRootNodeEvaluator()->ActivateCameraRig(Params);
	}
}

UE::Cameras::FCameraSystemEvaluator* UActivateCameraRigFunctions::FindCameraSystemEvaluator(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		AActor* ViewTarget = PlayerController->PlayerCameraManager->GetViewTarget();
		if (AGameplayCameraSystemActor* SystemActor = Cast<AGameplayCameraSystemActor>(ViewTarget))
		{
			return SystemActor->GetCameraSystemComponent()->GetCameraSystemEvaluator().Get();
		}
	}
	return nullptr;
}

TSharedPtr<UE::Cameras::FCameraEvaluationContext> UActivateCameraRigFunctions::EnsureGlobalContext()
{
	using namespace UE::Cameras;

	if (!GlobalContext.IsValid())
	{
		GlobalContext = MakeShared<FCameraEvaluationContext>();
		GlobalContext->GetInitialResult().bIsValid = true;	
	}
	return GlobalContext;
}

