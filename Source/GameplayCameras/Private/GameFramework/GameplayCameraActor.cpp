// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraActor.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/PlayerController.h"
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

#undef LOCTEXT_NAMESPACE

