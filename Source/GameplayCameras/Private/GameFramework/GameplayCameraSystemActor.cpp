// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemActor.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraSystemComponent.h"
#include "GameFramework/PlayerController.h"

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

#undef LOCTEXT_NAMESPACE

