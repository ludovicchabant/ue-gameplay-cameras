// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "ActorFactories/ActorFactory.h"

#include "GameplayCameraRigActorFactory.generated.h"

UCLASS()
class UGameplayCameraRigActorFactory : public UActorFactory
{
	GENERATED_BODY()

public:

	UGameplayCameraRigActorFactory(const FObjectInitializer& ObjInit);

public:

	// UActorFactory interface.
	virtual AActor* SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams) override;
	virtual bool CanCreateActorFrom( const FAssetData& AssetData, FText& OutErrorMsg ) override;
	virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
};

