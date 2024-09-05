// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/PlayerControllersDebugBlock.h"

#include "Camera/PlayerCameraManager.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#if UE_GAMEPLAY_CAMERAS_DEBUG

namespace UE::Cameras
{

UE_DEFINE_CAMERA_DEBUG_BLOCK(FPlayerControllersDebugBlock)

FPlayerControllersDebugBlock::FPlayerControllersDebugBlock()
{
}

void FPlayerControllersDebugBlock::Initialize(UWorld* World)
{
	if (!World)
	{
		return;
	}

	bHadValidWorld = true;

	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (!PlayerController)
		{
			continue;
		}
		if (!PlayerController->IsLocalPlayerController())
		{
			continue;
		}

		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		AActor* ActiveViewTarget = CameraManager ? CameraManager->GetViewTarget() : nullptr;

		FPlayerControllerDebugInfo PlayerControllerInfo;
		PlayerControllerInfo.PlayerControllerName = *GetNameSafe(PlayerController);
		PlayerControllerInfo.CameraManagerName = *GetNameSafe(CameraManager);
		PlayerControllerInfo.ActiveViewTargetName = *GetNameSafe(ActiveViewTarget);
		PlayerControllers.Add(PlayerControllerInfo);
	}
}

void FPlayerControllersDebugBlock::OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
{
	Renderer.AddText("{cam_title}Player Controllers:{cam_default}");
	Renderer.AddIndent();
	{
		Renderer.AddText(TEXT("%d active local player controller(s)\n"), PlayerControllers.Num());
		if (bHadValidWorld)
		{
			for (FPlayerControllerDebugInfo& PlayerController : PlayerControllers)
			{
				Renderer.AddText(TEXT("- {cam_notice}%s{cam_default}"), *PlayerController.PlayerControllerName);
				Renderer.AddIndent();
				{
					Renderer.AddText(TEXT("Camera manager: {cam_notice}%s{cam_default}\n"), *PlayerController.CameraManagerName);
					Renderer.AddText(TEXT("View target: {cam_notice}%s{cam_default}"), *PlayerController.ActiveViewTargetName);
				}
				Renderer.RemoveIndent();
			}
		}
		else
		{
			Renderer.AddText(TEXT("<invalid world>"));
		}
	}
	Renderer.RemoveIndent();
}

void FPlayerControllersDebugBlock::OnSerialize(FArchive& Ar)
{
	Ar << PlayerControllers;
	Ar << bHadValidWorld;
}

FArchive& operator<< (FArchive& Ar, FPlayerControllersDebugBlock::FPlayerControllerDebugInfo& PlayerControllerInfo)
{
	Ar << PlayerControllerInfo.PlayerControllerName;
	Ar << PlayerControllerInfo.CameraManagerName;
	Ar << PlayerControllerInfo.ActiveViewTargetName;
	return Ar;
}

}  // namespace UE::Cameras

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

