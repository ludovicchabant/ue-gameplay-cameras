// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemHost.h"

#include "Core/CameraSystemEvaluator.h"
#include "Debug/DebugDrawService.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemActor.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Templates/UnrealTemplate.h"

const TCHAR* UGameplayCameraSystemHost::DefaultHostName = TEXT("GameplayCameraSystemHost");

UGameplayCameraSystemHost* UGameplayCameraSystemHost::FindOrCreateHost(APlayerController* PlayerController, const TCHAR* HostName)
{
	static bool bIsCreatingHost = false;

	if (!ensure(PlayerController))
	{
		return nullptr;
	}

	if (UGameplayCameraSystemHost* ExistingHost = FindHost(PlayerController, HostName, true))
	{
		return ExistingHost;
	}
	
	if (!ensureMsgf(!bIsCreatingHost, TEXT("Detected reentrant call to UGameplayCameraSystemHost::FindOrCreateHost!")))
	{
		return nullptr;
	}

	TGuardValue<bool> IsCreatingHostGuard(bIsCreatingHost, true);

	if (!HostName)
	{
		HostName = DefaultHostName;
	}

	UGameplayCameraSystemHost* NewHost = NewObject<UGameplayCameraSystemHost>(PlayerController, HostName);

	NewHost->Initialize();

	return NewHost;
}

UGameplayCameraSystemHost* UGameplayCameraSystemHost::FindHost(APlayerController* PlayerController, const TCHAR* HostName, bool bAllowNull)
{
	if (!PlayerController)
	{
		ensureMsgf(bAllowNull, TEXT("Can't find gameplay camera system host: null player controller provided!"));
		return nullptr;
	}

	if (!HostName)
	{
		HostName = DefaultHostName;
	}
	
	UGameplayCameraSystemHost* Host = FindObject<UGameplayCameraSystemHost>(PlayerController, HostName);
	ensureMsgf(Host || bAllowNull, 
			TEXT("Can't find gameplay camera system host named '%s' under player controller '%s'."),
			HostName, *GetNameSafe(PlayerController));
	return Host;
}

UGameplayCameraSystemHost::UGameplayCameraSystemHost(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameplayCameraSystemHost::Initialize()
{
	Evaluator = MakeShared<FCameraSystemEvaluator>();
	Evaluator->Initialize(this);

#if UE_GAMEPLAY_CAMERAS_DEBUG
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		DebugDrawDelegateHandle = UDebugDrawService::Register(
				TEXT("Game"), FDebugDrawDelegate::CreateUObject(this, &UGameplayCameraSystemHost::DebugDraw));
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
}

void UGameplayCameraSystemHost::BeginDestroy()
{
#if UE_GAMEPLAY_CAMERAS_DEBUG
	if (DebugDrawDelegateHandle.IsValid())
	{
		UDebugDrawService::Unregister(DebugDrawDelegateHandle);
		DebugDrawDelegateHandle.Reset();
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

	Evaluator.Reset();

	Super::BeginDestroy();
}

void UGameplayCameraSystemHost::AddReferencedObjects(UObject* Object, FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Object, Collector);

	UGameplayCameraSystemHost* This = CastChecked<UGameplayCameraSystemHost>(Object);
	if (This->Evaluator.IsValid())
	{
		This->Evaluator->AddReferencedObjects(Collector);
	}
}

APlayerController* UGameplayCameraSystemHost::GetPlayerController()
{
	return GetTypedOuter<APlayerController>();
}

TSharedPtr<UE::Cameras::FCameraSystemEvaluator> UGameplayCameraSystemHost::GetCameraSystemEvaluator()
{
	return Evaluator;
}

#if UE_GAMEPLAY_CAMERAS_DEBUG

void UGameplayCameraSystemHost::DebugDraw(UCanvas* Canvas, APlayerController* PlayerController)
{
	using namespace UE::Cameras;

	if (Evaluator.IsValid())
	{
		// We're looking from the outside if we are not the view target, or if we don't have a player
		// anymore (which happens in spectator mode like with the debug camera).
		APlayerController* ActualPlayerController = GetPlayerController();
		const bool bIsDebugCameraEnabled = !ActualPlayerController || !ActualPlayerController->Player;

		FCameraSystemDebugUpdateParams DebugUpdateParams;
		DebugUpdateParams.CanvasObject = Canvas;
		DebugUpdateParams.bIsDebugCameraEnabled = bIsDebugCameraEnabled;
		Evaluator->DebugUpdate(DebugUpdateParams);
	}
}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

