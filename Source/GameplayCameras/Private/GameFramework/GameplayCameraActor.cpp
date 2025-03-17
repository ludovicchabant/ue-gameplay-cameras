// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraActor.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraActor)

#define LOCTEXT_NAMESPACE "GameplayCameraActor"

AGameplayCameraActor::AGameplayCameraActor(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	CameraComponent = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
}

USceneComponent* AGameplayCameraActor::GetDefaultAttachComponent() const
{
	return CameraComponent;
}

void AGameplayCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGameplayCameraActor::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AGameplayCameraActor::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
}

#undef LOCTEXT_NAMESPACE

