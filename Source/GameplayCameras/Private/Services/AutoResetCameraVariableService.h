// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationService.h"
#include "UObject/WeakObjectPtr.h"

class UCameraVariableAsset;

namespace UE::Cameras
{

class FAutoResetCameraVariableService : public FCameraEvaluationService
{
	UE_DECLARE_CAMERA_EVALUATION_SERVICE(GAMEPLAYCAMERAS_API, FAutoResetCameraVariableService)

public:

	/** Adds a variable to the list of variables to reset every update. */
	void AddAutoResetVariable(const UCameraVariableAsset* InVariable);
	/** Remove a variable from the reset list. */
	void RemoveAutoResetVariable(const UCameraVariableAsset* InVariable);

protected:

	// FCameraEvaluationService interface.
	virtual void OnInitialize(const FCameraEvaluationServiceInitializeParams& Params) override;
	virtual void OnPreUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult) override;
	virtual void OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent) override;

private:

	TMap<TWeakObjectPtr<const UCameraVariableAsset>, uint32> AutoResetVariables;
};

}  // namespace UE::Cameras

