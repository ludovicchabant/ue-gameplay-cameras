// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemActor.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "GameFramework/PlayerController.h"
#include "GameplayCamerasSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraSystemActor)

#define LOCTEXT_NAMESPACE "GameplayCameraSystemActor"

AGameplayCameraSystemActor::AGameplayCameraSystemActor(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	CameraSystemComponent = CreateDefaultSubobject<UGameplayCameraSystemComponent>(TEXT("CameraSystemComponent"));
	RootComponent = CameraSystemComponent;
}

void AGameplayCameraSystemActor::BecomeViewTarget(APlayerController* PC)
{
	Super::BecomeViewTarget(PC);

	CameraSystemComponent->OnBecomeViewTarget();
}

void AGameplayCameraSystemActor::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	CameraSystemComponent->GetCameraView(DeltaTime, OutResult);
}

void AGameplayCameraSystemActor::EndViewTarget(APlayerController* PC)
{
	CameraSystemComponent->OnEndViewTarget();

	Super::EndViewTarget(PC);
}

void AGameplayCameraSystemActor::AutoManageActiveViewTarget(APlayerController* PlayerController)
{
	static const TCHAR* AutoSpawnedActorName = TEXT("AutoSpawnedGameplayCameraSystemActor");

	const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
	if (!Settings->bAutoSpawnCameraSystemActor)
	{
		return;
	}

	UGameplayCameraSystemHost* Host = UGameplayCameraSystemHost::FindHost(PlayerController);
	if (!Host)
	{
		UE_LOG(LogCameraSystem, Error, TEXT("Can't auto-manage active view target: no camera system host found!"));
		return;
	}
	
	AGameplayCameraSystemActor* SpawnedActor = FindObject<AGameplayCameraSystemActor>(PlayerController, AutoSpawnedActorName);
	if (!SpawnedActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = AutoSpawnedActorName;

		UWorld* World = PlayerController->GetWorld();
		SpawnedActor = World->SpawnActor<AGameplayCameraSystemActor>(SpawnParams);

		SpawnedActor->Rename(nullptr, PlayerController);
	}
	check(SpawnedActor);
	SpawnedActor->GetCameraSystemComponent()->ActivateCameraSystemForPlayerController(PlayerController);
}

#undef LOCTEXT_NAMESPACE

