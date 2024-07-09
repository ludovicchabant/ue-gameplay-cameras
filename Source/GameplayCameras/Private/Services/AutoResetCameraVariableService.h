// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationService.h"
#include "UObject/ObjectPtr.h"

class UCameraVariableAsset;

namespace UE::Cameras
{

class FAutoResetCameraVariableService : public FCameraEvaluationService
{
protected:

	// FCameraEvaluationService interface.
	virtual void OnInitialize(const FCameraEvaluationServiceInitializeParams& Params) override;
	virtual void OnPreUpdate(const FCameraEvaluationServiceUpdateParams& Params, FCameraEvaluationServiceUpdateResult& OutResult) override;
	virtual void OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent) override;

private:

	void AddAutoResetVariable(const UCameraVariableAsset* InVariable);
	void RemoveAutoResetVariable(const UCameraVariableAsset* InVariable);

private:

	TMap<const UCameraVariableAsset*, uint32> AutoResetVariables;
};

}  // namespace UE::Cameras

