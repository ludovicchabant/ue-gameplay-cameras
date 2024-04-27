// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"

#include "CameraEvaluationResultInterop.generated.h"

class UBooleanCameraVariable;
class UDoubleCameraVariable;
class UFloatCameraVariable;
class UInteger32CameraVariable;
class URotator3dCameraVariable;
class UTransform3dCameraVariable;
class UVector2dCameraVariable;
class UVector3dCameraVariable;
class UVector4dCameraVariable;
struct FCameraPose;

namespace UE::Cameras
{

struct FCameraNodeEvaluationResult;

}  // namespace UE::Cameras

UCLASS(BlueprintType, DisplayName="Camera Evaluation Result")
class GAMEPLAYCAMERAS_API UCameraEvaluationResultInterop : public UObject
{
	GENERATED_BODY()

public:

	using FCameraNodeEvaluationResult = UE::Cameras::FCameraNodeEvaluationResult;

	UCameraEvaluationResultInterop(const FObjectInitializer& ObjectInit);

	void Setup(FCameraNodeEvaluationResult* InResult);
	void Teardown();

	FCameraNodeEvaluationResult* GetEvaluationResult() { return Result; }
	const FCameraNodeEvaluationResult* GetEvaluationResult() const { return Result; }

public:

	UFUNCTION(BlueprintPure, Category=Camera)
	const FCameraPose& GetCameraPose() const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Bool"))
	bool GetBooleanCameraVariable(UBooleanCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Int"))
	int32 GetInteger32CameraVariable(UInteger32CameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Float"))
	float GetFloatCameraVariable(UFloatCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Double"))
	double GetDoubleCameraVariable(UDoubleCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Vec2"))
	FVector2D GetVector2CameraVariable(UVector2dCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Vec3"))
	FVector GetVector3CameraVariable(UVector3dCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Vec4"))
	FVector4 GetVector4CameraVariable(UVector4dCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Rotator"))
	FRotator GetRotatorCameraVariable(URotator3dCameraVariable* InVariableAsset) const;

	UFUNCTION(BlueprintPure, Category=Camera, meta=(CompactNodeTitle="Get Transform"))
	FTransform GetTransformCameraVariable(UTransform3dCameraVariable* InVariableAsset) const;

public:

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetCameraPose(const FCameraPose& InCameraPose);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetBooleanCameraVariable(UBooleanCameraVariable* InVariableAsset, bool InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetInteger32CameraVariable(UInteger32CameraVariable* InVariableAsset, int32 InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetFloatCameraVariable(UFloatCameraVariable* InVariableAsset, float InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetDoubleCameraVariable(UDoubleCameraVariable* InVariableAsset, double InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetVector2CameraVariable(UVector2dCameraVariable* InVariableAsset, const FVector2D& InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetVector3CameraVariable(UVector3dCameraVariable* InVariableAsset, const FVector& InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetVector4CameraVariable(UVector4dCameraVariable* InVariableAsset, const FVector4& InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetRotatorCameraVariable(URotator3dCameraVariable* InVariableAsset, const FRotator& InValue);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetTransformCameraVariable(UTransform3dCameraVariable* InVariableAsset, const FTransform& InValue);

private:

	FCameraNodeEvaluationResult* Result = nullptr;
};

