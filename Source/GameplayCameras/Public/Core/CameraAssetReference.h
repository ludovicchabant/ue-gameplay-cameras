// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StructUtils/PropertyBag.h"
#include "UObject/ObjectPtr.h"

#include "CameraAssetReference.generated.h"

class UCameraAsset;
struct FPropertyTag;

namespace UE::Cameras
{
	class FCameraAssetReferenceDetailsCustomization;
	struct FCameraNodeEvaluationResult;
}

/**
 * A structure holding a reference to a camera asset, along with the interface parameter
 * override values for any of its camera rigs.
 */
USTRUCT(BlueprintType)
struct FCameraAssetReference
{
	GENERATED_BODY()

public:

	FCameraAssetReference();
	FCameraAssetReference(UCameraAsset* InCameraAsset);

	/** Returns whether this reference points to a valid camera asset. */
	bool IsValid() const
	{
		return CameraAsset != nullptr;
	}

	/** Gets the referenced camera asset. */
	UCameraAsset* GetCameraAsset()
	{
		return CameraAsset;
	}

	/** Gets the referenced camera asset. */
	const UCameraAsset* GetCameraAsset() const
	{
		return CameraAsset;
	}

	/** Sets the referenced camera asset. */
	void SetCameraAsset(UCameraAsset* InCameraAsset)
	{
		if (CameraAsset != InCameraAsset)
		{
			CameraAsset = InCameraAsset;
			RebuildParameters();
		}
	}

	/** Gets the parameters for this camera, some of which containing overrides. */
	const FInstancedPropertyBag& GetParameters() const
	{
		return Parameters;
	}

	/** Gets the parameters for this camera, some of which containing overrides. */
	FInstancedPropertyBag& GetParameters()
	{
		return Parameters;
	}

	/** Gets the IDs of the parameters with override values. */
	TConstArrayView<FGuid> GetOverriddenParameterGuids() const
	{
		return ParameterOverrideGuids;
	}

	/** Applies the parameter override values to the given evaluation result. */
	void ApplyParameterOverrides(UE::Cameras::FCameraNodeEvaluationResult& OutResult, bool bDrivenOverridesOnly);

public:

	// Internal API.

	bool SerializeFromMismatchedTag(FPropertyTag const& Tag, FStructuredArchive::FSlot Slot);

	GAMEPLAYCAMERAS_API bool IsParameterOverridden(const FGuid PropertyID) const;
	GAMEPLAYCAMERAS_API void SetParameterOverridden(const FGuid PropertyID, bool bIsOverridden);

	GAMEPLAYCAMERAS_API bool IsParameterAnimated(const FGuid PropertyID) const;
	GAMEPLAYCAMERAS_API void SetParameterAnimated(const FGuid PropertyID, bool bIsAnimated);

	GAMEPLAYCAMERAS_API bool NeedsRebuildParameters() const;
	GAMEPLAYCAMERAS_API bool RebuildParametersIfNeeded();
	GAMEPLAYCAMERAS_API void RebuildParameters();

private:

	/** The referenced camera asset. */
	UPROPERTY(EditAnywhere, Category=Camera)
	TObjectPtr<UCameraAsset> CameraAsset;

	/** The camera asset's parameters. */
	UPROPERTY(EditAnywhere, Category="", meta=(FixedLayout=true, InterpBagProperties=true, DisplayName="Camera Parameters"))
	FInstancedPropertyBag Parameters;

	/** The list of camera rig parameters with override values. */
	UPROPERTY(EditAnywhere, Category="")
	TArray<FGuid> ParameterOverrideGuids;

	UPROPERTY()
	TArray<FGuid> ParameterAnimatedGuids;

	friend class UE::Cameras::FCameraAssetReferenceDetailsCustomization;
};

template<>
struct TStructOpsTypeTraits<FCameraAssetReference> : public TStructOpsTypeTraitsBase2<FCameraAssetReference>
{
	enum
	{
		WithStructuredSerializeFromMismatchedTag = true
	};
};

