// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "GameplayCamerasSettings.generated.h"

/**
 * The settings for the Gameplay Cameras runtime.
 */
UCLASS(Config=GameplayCameras, DefaultConfig, MinimalAPI, meta=(DisplayName="Gameplay Cameras"))
class UGameplayCamerasSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	/** The default angle tolerance to accept an aiming operation. */
	UPROPERTY(EditAnywhere, Config, Category="IK Aiming")
	double DefaultIKAimingAngleTolerance = 0.1;  // 0.1 degrees

	/** The default distance tolerance to accept an aiming operation. */
	UPROPERTY(EditAnywhere, Config, Category="IK Aiming")
	double DefaultIKAimingDistanceTolerance = 1.0;  // 1cm

	/** The default number of iterations for an aiming operation. */
	UPROPERTY(EditAnywhere, Config, Category="IK Aiming")
	uint8 DefaultIKAimingMaxIterations = 3;

	/** The distance below which any IK aiming operation is disabled. */
	UPROPERTY(EditAnywhere, Config, Category="IK Aiming")
	double DefaultIKAimingMinDistance = 100.0;  // 1m

protected:

	// UDeveloperSettings interface.
	virtual FName GetCategoryName() const override;
};

