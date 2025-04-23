// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraDirector.h"
#include "StateTreeReference.h"

#include "StateTreeCameraDirector.generated.h"

/**
 * A camera director that runs a StateTree to specify which camera rigs should be active
 * any given frame.
 */
UCLASS(MinimalAPI, EditInlineNew)
class UStateTreeCameraDirector : public UCameraDirector
{
	GENERATED_BODY()

public:

	UStateTreeCameraDirector(const FObjectInitializer& ObjectInit);

protected:

	// UCameraDirector interface.
	virtual FCameraDirectorEvaluatorPtr OnBuildEvaluator(FCameraDirectorEvaluatorBuilder& Builder) const override;
	virtual void OnBuildCameraDirector(UE::Cameras::FCameraBuildLog& BuildLog) override;
	virtual void OnGatherRigUsageInfo(FCameraDirectorRigUsageInfo& UsageInfo) const override;
	virtual void OnExtendAssetRegistryTags(FAssetRegistryTagsContext Context) const override;

public:

	/** The StateTree to execute. Must have been created with the CameraDirectorStateTreeSchema. */
	UPROPERTY(EditAnywhere, Category="StateTree",
			meta=(Schema="/Script/GameplayCameras.CameraDirectorStateTreeSchema"))
	FStateTreeReference StateTreeReference;
};

