// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemActor.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/GameplayCameraSystemHost.h"
#include "GameFramework/GameplayCamerasPlayerCameraManager.h"
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

void AGameplayCameraSystemActor::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	CameraSystemComponent->GetCameraView(DeltaTime, OutResult);
}

AGameplayCameraSystemActor* AGameplayCameraSystemActor::GetAutoSpawnedCameraSystemActor(APlayerController* PlayerController, bool bSpawnIfMissing)
{
	bool bDidSpawn;
	return GetAutoSpawnedCameraSystemActor(PlayerController, bSpawnIfMissing, &bDidSpawn);
}

AGameplayCameraSystemActor* AGameplayCameraSystemActor::GetAutoSpawnedCameraSystemActor(APlayerController* PlayerController, bool bSpawnIfMissing, bool* bOutSpawned)
{
	static const TCHAR* AutoSpawnedActorName = TEXT("AutoSpawnedGameplayCameraSystemActor");

	UGameplayCameraSystemHost* Host = UGameplayCameraSystemHost::FindHost(PlayerController);
	if (!Host)
	{
		if (bSpawnIfMissing)
		{
			Host = UGameplayCameraSystemHost::FindOrCreateHost(PlayerController);
		}
		else
		{
			FFrame::KismetExecutionMessage(
					TEXT("Can't auto-manage active view target: no camera system host found!"),
					ELogVerbosity::Error);
			return nullptr;
		}
	}

	*bOutSpawned = false;
	AGameplayCameraSystemActor* SpawnedActor = FindObject<AGameplayCameraSystemActor>(PlayerController, AutoSpawnedActorName);
	if (!SpawnedActor)
	{
		if (bSpawnIfMissing)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = AutoSpawnedActorName;

			UWorld* World = PlayerController->GetWorld();
			SpawnedActor = World->SpawnActor<AGameplayCameraSystemActor>(SpawnParams);

			SpawnedActor->Rename(nullptr, PlayerController);

			*bOutSpawned = true;
		}
		else
		{
			return nullptr;
		}
	}

	return SpawnedActor;
}

void AGameplayCameraSystemActor::AutoManageActiveViewTarget(APlayerController* PlayerController)
{
	const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
	if (!Settings->bAutoSpawnCameraSystemActor)
	{
		return;
	}

	if (AGameplayCamerasPlayerCameraManager* PlayerCameraManager = Cast<AGameplayCamerasPlayerCameraManager>(PlayerController->PlayerCameraManager))
	{
		return;
	}

	bool bDidSpawn = false;
	AGameplayCameraSystemActor* SpawnedActor = GetAutoSpawnedCameraSystemActor(PlayerController, true, &bDidSpawn);
	if (SpawnedActor)
	{
		UGameplayCameraSystemComponent* CameraSystemComponent = SpawnedActor->CameraSystemComponent;
		if (ensure(CameraSystemComponent))
		{
			if (bDidSpawn)
			{
				CameraSystemComponent->bSetPlayerControllerRotation = Settings->bAutoSpawnCameraSystemActorSetsControlRotation;
			}

			CameraSystemComponent->ActivateCameraSystemForPlayerController(PlayerController);
		}
	}
}

#undef LOCTEXT_NAMESPACE

