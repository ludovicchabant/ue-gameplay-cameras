// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"

#include "CameraVariableFactories.generated.h"

class UCameraVariableAsset;

/**
 * Implements a factory for camera variable assets.
 */
UCLASS(hidecategories=Object)
class UCameraVariableAssetFactory : public UFactory
{
	GENERATED_BODY()

public:

	// The type of camera variable asset to create.
	UPROPERTY(EditAnywhere, Category=CameraVariable)
	TSubclassOf<UCameraVariableAsset> CameraVariableAssetType;

	UCameraVariableAssetFactory(const FObjectInitializer& ObjectInit);

	// UFactory Interface
	virtual FText GetDisplayName() const override;
	virtual uint32 GetMenuCategories() const override;
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

