// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

#include "CameraVariableCollection.generated.h"

class UCameraVariableAsset;

/**
 * An asset that represents a collection of camera variables.
 */
UCLASS(MinimalAPI)
class UCameraVariableCollection : public UObject
{
	GENERATED_BODY()

public:

	UCameraVariableCollection(const FObjectInitializer& ObjectInit);

public:

	/** The variables in this collection. */
	UPROPERTY(EditAnywhere, Category="Camera")
	TArray<TObjectPtr<UCameraVariableAsset>> Variables;
};

