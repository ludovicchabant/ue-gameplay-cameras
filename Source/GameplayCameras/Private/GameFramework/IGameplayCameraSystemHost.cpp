// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/IGameplayCameraSystemHost.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraEvaluationContextStack.h"
#include "Core/CameraSystemEvaluator.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void IGameplayCameraSystemHost::InitializeCameraSystem()
{
	using namespace UE::Cameras;

	FCameraSystemEvaluatorCreateParams Params;
	Params.Owner = GetAsObject();
	InitializeCameraSystem(Params);
}

void IGameplayCameraSystemHost::InitializeCameraSystem(const UE::Cameras::FCameraSystemEvaluatorCreateParams& Params)
{
	ensure(!CameraSystemEvaluator.IsValid());
	ensure(Params.Owner != nullptr && Params.Owner == GetAsObject());

	CameraSystemEvaluator = MakeShared<FCameraSystemEvaluator>();
	CameraSystemEvaluator->Initialize(Params);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	ensure(!DebugDrawDelegateHandle.IsValid());
	UWorld* World = Params.Owner->GetWorld();
	if (World && World->IsGameWorld())
	{
		DebugDrawDelegateHandle = UDebugDrawService::Register(
				TEXT("Game"), FDebugDrawDelegate::CreateRaw(this, &IGameplayCameraSystemHost::DebugDraw));
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

TScriptInterface<IGameplayCameraSystemHost> IGameplayCameraSystemHost::GetAsScriptInterface()
{
	TScriptInterface<IGameplayCameraSystemHost> Result(GetAsObject());
	checkSlow(Result.GetInterface() != nullptr);
	return Result;
}

IGameplayCameraSystemHost* IGameplayCameraSystemHost::FindActiveHost(APlayerController* PlayerController)
{
	if (PlayerController->PlayerCameraManager)
	{
		if (IGameplayCameraSystemHost* CameraManagerHost = Cast<IGameplayCameraSystemHost>(PlayerController->PlayerCameraManager))
		{
			return CameraManagerHost;
		}
	}
	else if (AActor* ViewTarget = PlayerController->GetViewTarget())
	{
		if (IGameplayCameraSystemHost* ViewTargetHost = ViewTarget->FindComponentByInterface<IGameplayCameraSystemHost>())
		{
			return ViewTargetHost;
		}
	}
	return nullptr;
}

void IGameplayCameraSystemHost::EnsureCameraSystemInitialized()
{
	if (!CameraSystemEvaluator.IsValid())
	{
		InitializeCameraSystem();
	}
}

void IGameplayCameraSystemHost::DestroyCameraSystem()
{
#if UE_GAMEPLAY_CAMERAS_DEBUG
	if (DebugDrawDelegateHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawDelegateHandle);
		DebugDrawDelegateHandle.Reset();
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

	CameraSystemEvaluator.Reset();
}

void IGameplayCameraSystemHost::OnAddReferencedObjects(FReferenceCollector& Collector)
{
	if (CameraSystemEvaluator.IsValid())
	{
		CameraSystemEvaluator->AddReferencedObjects(Collector);
	}
}

void IGameplayCameraSystemHost::UpdateCameraSystem(float DeltaTime)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator.IsValid())
	{
		FCameraSystemEvaluationParams Params;
		Params.DeltaTime = DeltaTime;
		CameraSystemEvaluator->Update(Params);
	}
}

#if WITH_EDITOR

void IGameplayCameraSystemHost::UpdateCameraSystemForEditorPreview(float DeltaTime)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator.IsValid())
	{
		FCameraSystemEvaluationParams Params;
		Params.DeltaTime = DeltaTime;
		CameraSystemEvaluator->EditorPreviewUpdate(Params);
	}
}

#endif  // WITH_EDITOR

TSharedPtr<UE::Cameras::FCameraSystemEvaluator> IGameplayCameraSystemHost::GetCameraSystemEvaluator()
{
	return CameraSystemEvaluator;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void IGameplayCameraSystemHost::DebugDraw(UCanvas* Canvas, APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (CameraSystemEvaluator.IsValid())
	{
		// We're looking from the outside if we are not the view target, or if we don't have a player
		// anymore (which happens in spectator mode like with the debug camera).
		bool bIsDebugCameraEnabled = false;
		if (TSharedPtr<FCameraEvaluationContext> ActiveContext = CameraSystemEvaluator->GetEvaluationContextStack().GetActiveContext())
		{
			const APlayerController* ActivePlayerController = ActiveContext->GetPlayerController();
			bIsDebugCameraEnabled = !ActivePlayerController || !ActivePlayerController->Player;
		}

		FCameraSystemDebugUpdateParams DebugUpdateParams;
		DebugUpdateParams.CanvasObject = Canvas;
		DebugUpdateParams.bIsDebugCameraEnabled = bIsDebugCameraEnabled;
		CameraSystemEvaluator->DebugUpdate(DebugUpdateParams);
	}
}

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

