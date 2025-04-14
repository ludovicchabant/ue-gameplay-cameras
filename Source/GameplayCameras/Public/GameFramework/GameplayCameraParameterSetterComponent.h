// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Core/CameraRigAssetReference.h"
#include "Core/CameraVariableSetter.h"

#include "GameplayCameraParameterSetterComponent.generated.h"

namespace UE::Cameras { class FRootCameraNodeEvaluator; }

UCLASS(BlueprintType, MinimalAPI, ClassGroup=Camera, meta=(BlueprintSpawnableComponent))
class UGameplayCameraParameterSetterComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UGameplayCameraParameterSetterComponent(const FObjectInitializer& ObjInit);

public:

	/** The camera asset whose parameters to override. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Camera)
	FCameraRigAssetReference CameraRigReference;

public:

	UFUNCTION(BlueprintCallable, Category=Camera)
	void StartParameterSetters();

	UFUNCTION(BlueprintCallable, Category=Camera)
	void StopParameterSetters(bool bImmediately = false);

public:

	// UActorComponent interface.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UFUNCTION()
	void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:

	UE::Cameras::FRootCameraNodeEvaluator* GetRootNodeEvaluator();

private:

	TArray<FCameraVariableSetterHandle> SetterHandles;
};

