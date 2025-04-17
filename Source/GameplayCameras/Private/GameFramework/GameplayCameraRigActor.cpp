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

UGameplayCameraComponentBase* AGameplayCameraRigActor::GetCameraComponentBase() const
{
	return CameraRigComponent;
}

#undef LOCTEXT_NAMESPACE

