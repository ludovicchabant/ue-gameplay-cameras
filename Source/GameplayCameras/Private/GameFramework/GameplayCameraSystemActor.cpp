// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/GameplayCameraSystemActor.h"

#include "GameFramework/GameplayCameraSystemComponent.h"

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

#undef LOCTEXT_NAMESPACE

