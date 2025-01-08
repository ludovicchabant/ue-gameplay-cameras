// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluator.h"
#include "GameFramework/BlueprintCameraContextDataTable.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BlueprintCameraNodeEvaluationResult.generated.h"

namespace UE::Cameras
{

struct FCameraNodeEvaluationResult;

}  // namespace UE::Cameras

/**
 * Blueprint wrapper for camera evaluation data.
 */
USTRUCT(BlueprintType, DisplayName="Camera Evalution Data")
struct GAMEPLAYCAMERAS_API FBlueprintCameraNodeEvaluationResult
{
	GENERATED_BODY()

public:

	using FCameraNodeEvaluationResult = UE::Cameras::FCameraNodeEvaluationResult;

	FBlueprintCameraNodeEvaluationResult();
	FBlueprintCameraNodeEvaluationResult(FCameraNodeEvaluationResult* InResult);

public:

	/** Gets the camera pose wrapper. */
	FBlueprintCameraPose GetCameraPose() const;
	/** Gets the variable table wrapper. */
	FBlueprintCameraVariableTable GetVariableTable() const;
	/** Gets the context data table wrapper. */
	FBlueprintCameraContextDataTable GetContextDataTable() const;

	/** Sets the camera pose on the underlying evaluation result. */
	void SetCameraPose(const FBlueprintCameraPose& CameraPose);

private:

	/** The underlying camera evaluation result. */
	FCameraNodeEvaluationResult* Result = nullptr;
};

/**
 * Blueprint function library for a camera node evaluation data wrapper.
 */
UCLASS()
class UBlueprintCameraNodeEvaluationResultFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Gets the camera pose. */
	UFUNCTION(BlueprintPure, Category="Camera")
	static FBlueprintCameraPose GetCameraPose(const FBlueprintCameraNodeEvaluationResult& Data);

	/** Gets the camera variable table. */
	UFUNCTION(BlueprintPure, Category="Camera")
	static FBlueprintCameraVariableTable GetVariableTable(const FBlueprintCameraNodeEvaluationResult& Data);

	/** Gets the camera context data table. */
	UFUNCTION(BlueprintPure, Category="Camera")
	static FBlueprintCameraContextDataTable GetContextDataTable(const FBlueprintCameraNodeEvaluationResult& Data);

public:

	/** Sets the camera pose. */
	UFUNCTION(BlueprintCallable, Category="Camera")
	static void SetCameraPose(FBlueprintCameraNodeEvaluationResult& Data, const FBlueprintCameraPose& CameraPose);
};

