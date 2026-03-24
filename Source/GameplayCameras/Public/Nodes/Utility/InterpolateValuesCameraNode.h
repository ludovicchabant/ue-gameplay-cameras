// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Core/CameraNode.h"
#include "Core/CameraValueInterpolator.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ICustomCameraNodeParameterProvider.h"

#include "InterpolateValuesCameraNode.generated.h"

/**
 * An entry in a value interpolator camera node. Each entry interpolates one value.
 */
USTRUCT()
struct FInterpolateValuesCameraNodeEntry
{
	GENERATED_BODY()

	/** The name of the value. */
	UPROPERTY(EditAnywhere, Category="Common")
	FString Name;

	/** The interpolator to use for this value. */
	UPROPERTY(EditAnywhere, Category="Common")
	TObjectPtr<UCameraValueInterpolator> Interpolator;

	/** The Guid of this entry. */
	UPROPERTY()
	FGuid Guid;

	// Generated on build.

	UPROPERTY()
	FCameraVariableID InVariableID;

	UPROPERTY()
	FCameraVariableID OutVariableID;
};

/**
 * Base node for all value interpolator camera nodes.
 */
UCLASS(Abstract)
class UInterpolateValuesCameraNodeBase
	: public UCameraNode
	, public ICustomCameraNodeParameterProvider
{
	GENERATED_BODY()

public:

	// UObject interface.
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif  // WITH_EDITOR

	// ICustomCameraNodeParameterProvider interface.
	virtual void GetCustomCameraNodeParameters(FCameraNodeParameterInfos& OutParameterInfos) override;

	// UInterpolateValuesCameraNodeBase interface.
	virtual ECameraVariableType GetVariableType() const PURE_VIRTUAL(UInterpolateValuesCameraNodeBase::GetVariableType, return ECameraVariableType::Boolean;);

protected:

	// UCameraNode interface.
	virtual void OnPreBuild(FCameraBuildContext& BuildContext) override;
	virtual void OnBuild(FCameraObjectBuildContext& BuildContext) override;

public:

	/** The values to interpolate. */
	UPROPERTY(EditAnywhere, Category="Comon")
	TArray<FInterpolateValuesCameraNodeEntry> Values;
};

/**
 * A camera node that interpolate values, whose results can be wired into subsequent nodes.
 */
UCLASS(MinimalAPI, DisplayName="Interpolate Float Values", meta=(CameraNodeCategories="Utility"))
class UInterpolateDoubleValuesCameraNode : public UInterpolateValuesCameraNodeBase
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

	// UInterpolateValuesCameraNodeBase interface.
	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Double; }
};

/**
 * A camera node that interpolate values, whose results can be wired into subsequent nodes.
 */
UCLASS(MinimalAPI, DisplayName="Interpolate Vector2 Values", meta=(CameraNodeCategories="Utility"))
class UInterpolateVector2ValuesCameraNode : public UInterpolateValuesCameraNodeBase
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

	// UInterpolateValuesCameraNodeBase interface.
	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector2d; }
};

/**
 * A camera node that interpolate values, whose results can be wired into subsequent nodes.
 */
UCLASS(MinimalAPI, DisplayName="Interpolate Vector Values", meta=(CameraNodeCategories="Utility"))
class UInterpolateVector3ValuesCameraNode : public UInterpolateValuesCameraNodeBase
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

	// UInterpolateValuesCameraNodeBase interface.
	virtual ECameraVariableType GetVariableType() const override { return ECameraVariableType::Vector3d; }
};

