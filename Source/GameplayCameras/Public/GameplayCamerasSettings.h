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

	/**
	 * Automatically spawn a camera system actor when any gameplay camera activates and no camera system
	 * is found on the player controller's camera manager, or as a view target. This camera system actor
	 * will be spawned and set as the view target automatically.
	 */
	UPROPERTY(EditAnywhere, Config, Category="General")
	bool bAutoSpawnCameraSystemActor = true;

	/**
	 * Whether the automatically spawned camera system actor should set the control rotation on the
	 * associated player controller. This is useful if camera rigs are managing their own rotation, e.g.
	 * by specifying input slots on boom arms instead of using the existing control rotation.
	 * Do not mix handling control rotation via camera nodes, and handling control rotation by calling
	 * methods like AddYawInput/AddPitchInput/AddRollInput, as this can lead to a feedback loop.
	 */
	UPROPERTY(EditAnywhere, Config, Category="General", meta=(EditCondition="bAutoSpawnCameraSystemActor"))
	bool bAutoSpawnCameraSystemActorSetsControlRotation = false;

	/**
	 * The number of camera rigs combined in one frame past which the camera system emits a warning.
	 */
	UPROPERTY(EditAnywhere, Config, Category="General")
	int32 CombinedCameraRigNumThreshold = 10;

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

