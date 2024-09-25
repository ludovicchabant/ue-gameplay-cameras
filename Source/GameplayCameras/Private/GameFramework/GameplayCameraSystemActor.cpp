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

AGameplayCameraSystemActor* AGameplayCameraSystemActor::GetAutoSpawnedCameraSystemActor(APlayerController* PlayerController, bool bForceSpawn)
{
	static const TCHAR* AutoSpawnedActorName = TEXT("AutoSpawnedGameplayCameraSystemActor");

	const UGameplayCamerasSettings* Settings = GetDefault<UGameplayCamerasSettings>();
	if (!Settings->bAutoSpawnCameraSystemActor)
	{
		return nullptr;
	}

	UGameplayCameraSystemHost* Host = UGameplayCameraSystemHost::FindHost(PlayerController);
	if (!Host)
	{
		if (bForceSpawn)
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

	AGameplayCameraSystemActor* SpawnedActor = FindObject<AGameplayCameraSystemActor>(PlayerController, AutoSpawnedActorName);
	if (!SpawnedActor)
	{
		if (bForceSpawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = AutoSpawnedActorName;

			UWorld* World = PlayerController->GetWorld();
			SpawnedActor = World->SpawnActor<AGameplayCameraSystemActor>(SpawnParams);

			SpawnedActor->Rename(nullptr, PlayerController);
			
			UGameplayCameraSystemComponent* CameraSystemComponent = SpawnedActor->CameraSystemComponent;
			if (ensure(CameraSystemComponent))
			{
				CameraSystemComponent->bSetPlayerControllerRotation = Settings->bAutoSpawnCameraSystemActorSetsControlRotation;
			}
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
	AGameplayCameraSystemActor* SpawnedActor = GetAutoSpawnedCameraSystemActor(PlayerController, true);
	if (SpawnedActor)
	{
		SpawnedActor->GetCameraSystemComponent()->ActivateCameraSystemForPlayerController(PlayerController);
	}
}

#undef LOCTEXT_NAMESPACE

