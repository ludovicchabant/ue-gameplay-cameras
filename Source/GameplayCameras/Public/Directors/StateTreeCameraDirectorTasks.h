// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraRigAsset.h"
#include "Directors/CameraDirectorStateTreeSchema.h"

#include "StateTreeCameraDirectorTasks.generated.h"

/** Instance data for the "Activate Camera Rig" task. */
USTRUCT()
struct FGameplayCamerasActivateCameraRigTaskInstanceData
{
	GENERATED_BODY()

	/** The camera rig to activate. */
	UPROPERTY(EditAnywhere, Category="Cameras", meta=(UseCameraDirectorRigPicker=true))
	TObjectPtr<UCameraRigAsset> CameraRig;
};

/**
 * A task that activates a given camera rig inside a StateTreeCameraDirector.
 */
USTRUCT(meta=(DisplayName="Activate Camera Rig", Category="Cameras"))
struct FGameplayCamerasActivateCameraRigTask : public FGameplayCamerasStateTreeTask
{
	GENERATED_BODY()

	using FInstanceDataType = FGameplayCamerasActivateCameraRigTaskInstanceData;

protected:

	// FStateTreeTaskBase interface.
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

private:

	bool UpdateResult(FStateTreeExecutionContext& Context) const;

private:

	TStateTreeExternalDataHandle<FCameraDirectorStateTreeEvaluationData> CameraDirectorEvaluationDataHandle;

public:

	/** If true, the task will complete immediately. If false, the task will run until a transition triggers. */
	UPROPERTY(EditAnywhere, Category="State Tree")
	bool bRunOnce = false;
};

