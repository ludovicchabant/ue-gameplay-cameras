// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationService.h"
#include "UObject/WeakObjectPtr.h"

class UCameraVariableAsset;

namespace UE::Cameras
{

class FCameraEvaluationContext;
class FCameraVariableTable;
struct FCameraEvaluationContextStack;

class FAutoResetCameraVariableService : public FCameraEvaluationService
{
	UE_DECLARE_CAMERA_EVALUATION_SERVICE(GAMEPLAYCAMERAS_API, FAutoResetCameraVariableService)

public:

	FAutoResetCameraVariableService();

	/** Adds a variable to the list of variables to reset every update. */
	void AddAutoResetVariable(const UCameraVariableAsset* InVariable);
	/** Remove a variable from the reset list. */
	void RemoveAutoResetVariable(const UCameraVariableAsset* InVariable);

	/**
	 * Marks a variable as used from Blueprint, Verse, etc. This is an alternative
	 * to AddAutoResetVariable, for when it is unknown when the variable isn't used
	 * anymore. The variable will be removed from the list after it is seen that it
	 * hasn't been written to during a frame.
	 */
	void RegisterVariableUseFromScripting(const UCameraVariableAsset* InVariable);

public:

	// Internal API.
	void PerformVariableResets(FCameraVariableTable& RootVariableTable);
	void PerformVariableResets(FCameraVariableTable& RootVariableTable, const FCameraEvaluationContextStack& ContextStack);

protected:

	// FCameraEvaluationService interface.
	virtual void OnRootCameraNodeEvent(const FRootCameraNodeCameraRigEvent& InEvent) override;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	void DoPerformVariableResets(FCameraVariableTable& VariableTable, bool bOnlyNotWrittenThisFrame);
	void DoPerformVariableResets(TSharedPtr<FCameraEvaluationContext> EvaluationContext);
	void RemoveMarkedVariablesAndClearFlags();

private:

	struct FEntry
	{
		uint32 RefCount = 0;
		bool bUsedByScripting = false;
		bool bMarkedForRemoval = false;
#if UE_GAMEPLAY_CAMERAS_DEBUG
		bool bDebugWasUsedByScripting = false;
#endif
	};

	TMap<TWeakObjectPtr<const UCameraVariableAsset>, FEntry> AutoResetVariables;

#if UE_GAMEPLAY_CAMERAS_DEBUG
	friend class FAutoResetCameraVariableDebugBlock;
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG
};

}  // namespace UE::Cameras

