// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraRigActor.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraRigComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayCameraRigActor)

#define LOCTEXT_NAMESPACE "GameplayCameraRigActor"

AGameplayCameraRigActor::AGameplayCameraRigActor(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	CameraRigComponent = CreateDefaultSubobject<UGameplayCameraRigComponent>(TEXT("CameraRigComponent"));
	RootComponent = CameraRigComponent;
}

USceneComponent* AGameplayCameraRigActor::GetDefaultAttachComponent() const
{
	return CameraRigComponent;
}

void AGameplayCameraRigActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGameplayCameraRigActor::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AGameplayCameraRigActor::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
}

#undef LOCTEXT_NAMESPACE

