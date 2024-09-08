// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"

#include "CameraRigAssetReference.generated.h"

struct FCameraRigAssetReference;

namespace UE::Cameras
{
	class FCameraRigAssetBuilder;
	class FCameraRigAssetReferenceDetailsCustomization;
}

/** Base struct for camera rig parameter overrides. */
USTRUCT()
struct FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	/**
	 * The Guid of the overriden interface parameter in the inner camera rig.
	 */
	UPROPERTY()
	FGuid InterfaceParameterGuid;

	/**
	 * The Guid of the overriden interface parameter's private variable in the
	 * inner camera rig.
	 *
	 * This can be derived from InterfaceParameterGuid, but we cache this during
	 * the build process to avoid searching for interface parameters.
	 */
	UPROPERTY()
	FGuid PrivateVariableGuid;

	/**
	 * The name of the overriden interface parameter in the inner camera rig.
	 *
	 * This can be derived from InterfaceParameterGuid, but we cache this during
	 * the build process to avoid searching for interface parameters.
	 */
	UPROPERTY()
	FString InterfaceParameterName;

	/**
	 *
	 */
	UPROPERTY()
	bool bInvalid = false;
};

USTRUCT()
struct FBooleanCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FBooleanCameraParameter;

	UPROPERTY()
	FBooleanCameraParameter Value;
};

USTRUCT()
struct FInteger32CameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FInteger32CameraParameter;

	UPROPERTY()
	FInteger32CameraParameter Value;
};

USTRUCT()
struct FFloatCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FFloatCameraParameter;

	UPROPERTY()
	FFloatCameraParameter Value;
};

USTRUCT()
struct FDoubleCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FDoubleCameraParameter;

	UPROPERTY()
	FDoubleCameraParameter Value;
};

USTRUCT()
struct FVector2fCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FVector2fCameraParameter;

	UPROPERTY()
	FVector2fCameraParameter Value;
};

USTRUCT()
struct FVector2dCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FVector2dCameraParameter;

	UPROPERTY()
	FVector2dCameraParameter Value;
};

USTRUCT()
struct FVector3fCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FVector3fCameraParameter;

	UPROPERTY()
	FVector3fCameraParameter Value;
};

USTRUCT()
struct FVector3dCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FVector3dCameraParameter;

	UPROPERTY()
	FVector3dCameraParameter Value;
};

USTRUCT()
struct FVector4fCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FVector4fCameraParameter;

	UPROPERTY()
	FVector4fCameraParameter Value;
};

USTRUCT()
struct FVector4dCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FVector4dCameraParameter;

	UPROPERTY()
	FVector4dCameraParameter Value;
};

USTRUCT()
struct FRotator3fCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FRotator3fCameraParameter;

	UPROPERTY()
	FRotator3fCameraParameter Value;
};

USTRUCT()
struct FRotator3dCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FRotator3dCameraParameter;

	UPROPERTY()
	FRotator3dCameraParameter Value;
};

USTRUCT()
struct FTransform3fCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FTransform3fCameraParameter;

	UPROPERTY()
	FTransform3fCameraParameter Value;
};

USTRUCT()
struct FTransform3dCameraRigParameterOverride : public FCameraRigParameterOverrideBase
{
	GENERATED_BODY()

	using CameraParameterType = FTransform3dCameraParameter;

	UPROPERTY()
	FTransform3dCameraParameter Value;
};

/**
 * A structure that holds lists of camera rig interface parameter overrides, one list
 * per parameter type.
 */
USTRUCT()
struct GAMEPLAYCAMERAS_API FCameraRigParameterOverrides
{
	GENERATED_BODY()

public:

	/** Whether the given camera rig interface parameter is currently overriden. */
	template<typename ParameterOverrideType>
	bool IsParameterOverriden(const FGuid& CameraRigParameterGuid) const;

	/** Find a parameter override for the given inner camera rig interface parameter. */
	template<typename ParameterOverrideType>
	ParameterOverrideType* FindParameterOverride(const FGuid& CameraRigParameterGuid);

	/** Find or create a parameter override for the given inner camera rig interface parameter. */
	template<typename ParameterOverrideType>
	ParameterOverrideType& FindOrAddParameterOverride(const UCameraRigInterfaceParameter* CameraRigParameter);

	/** Remove any parameter override for the given inner camera rig interface parameter. */
	template<typename ParameterOverrideType>
	void RemoveParameterOverride(const FGuid& CameraRigParameterGuid);

	/** Remove all parameter overrides. */
	void Reset();

public:

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	inline TArrayView<const F##ValueName##CameraRigParameterOverride> Get##ValueName##Overrides() const\
	{\
		return ValueName##Overrides;\
	}\
	inline TArrayView<F##ValueName##CameraRigParameterOverride> Get##ValueName##Overrides()\
	{\
		return ValueName##Overrides;\
	}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE


private:

	template<typename ParameterOverrideType>
	bool IsParameterOverriden(const TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid) const;

	template<typename ParameterOverrideType>
	ParameterOverrideType* FindParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid);

	template<typename ParameterOverrideType>
	ParameterOverrideType& FindOrAddParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const UCameraRigInterfaceParameter* CameraRigParameter);

	template<typename ParameterOverrideType>
	void RemoveParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid);


public:

	// Internal API.

	template<typename ParameterOverrideType>
	inline void AppendParameterOverrides(TArrayView<ParameterOverrideType> NewOverrides);

private:

	// Interface parameter overrides

	UPROPERTY()
	TArray<FBooleanCameraRigParameterOverride> BooleanOverrides;
	UPROPERTY()
	TArray<FInteger32CameraRigParameterOverride> Integer32Overrides;
	UPROPERTY()
	TArray<FFloatCameraRigParameterOverride> FloatOverrides;
	UPROPERTY()
	TArray<FDoubleCameraRigParameterOverride> DoubleOverrides;
	UPROPERTY()
	TArray<FVector2fCameraRigParameterOverride> Vector2fOverrides;
	UPROPERTY()
	TArray<FVector2dCameraRigParameterOverride> Vector2dOverrides;
	UPROPERTY()
	TArray<FVector3fCameraRigParameterOverride> Vector3fOverrides;
	UPROPERTY()
	TArray<FVector3dCameraRigParameterOverride> Vector3dOverrides;
	UPROPERTY()
	TArray<FVector4fCameraRigParameterOverride> Vector4fOverrides;
	UPROPERTY()
	TArray<FVector4dCameraRigParameterOverride> Vector4dOverrides;
	UPROPERTY()
	TArray<FRotator3fCameraRigParameterOverride> Rotator3fOverrides;
	UPROPERTY()
	TArray<FRotator3dCameraRigParameterOverride> Rotator3dOverrides;
	UPROPERTY()
	TArray<FTransform3fCameraRigParameterOverride> Transform3fOverrides;
	UPROPERTY()
	TArray<FTransform3dCameraRigParameterOverride> Transform3dOverrides;

	friend struct FCameraRigAssetReference;

	friend class UE::Cameras::FCameraRigAssetBuilder;
};

template<typename ParameterOverrideType>
bool FCameraRigParameterOverrides::IsParameterOverriden(const TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid) const
{
	return OverridesArray.ContainsByPredicate(
			[CameraRigParameterGuid](ParameterOverrideType& Item)
			{
				return (Item.InterfaceParameterGuid == CameraRigParameterGuid);
			});
}

template<typename ParameterOverrideType>
ParameterOverrideType* FCameraRigParameterOverrides::FindParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid)
{
	ParameterOverrideType* FoundItem = OverridesArray.FindByPredicate(
			[CameraRigParameterGuid](ParameterOverrideType& Item)
			{
				return (Item.InterfaceParameterGuid == CameraRigParameterGuid);
			});
	return FoundItem;
}

template<typename ParameterOverrideType>
ParameterOverrideType& FCameraRigParameterOverrides::FindOrAddParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const UCameraRigInterfaceParameter* CameraRigParameter)
{
	ParameterOverrideType* Existing = FindParameterOverride<ParameterOverrideType>(OverridesArray, CameraRigParameter->Guid);
	if (Existing)
	{
		return *Existing;
	}
	else
	{
		ParameterOverrideType& NewOverride = OverridesArray.Emplace_GetRef();
		NewOverride.InterfaceParameterGuid = CameraRigParameter->Guid;
		NewOverride.InterfaceParameterName = CameraRigParameter->InterfaceParameterName;
		return NewOverride;
	}
}

template<typename ParameterOverrideType>
void FCameraRigParameterOverrides::RemoveParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid)
{
	OverridesArray.RemoveAll(
			[CameraRigParameterGuid](ParameterOverrideType& Item)
			{
				return (Item.InterfaceParameterGuid == CameraRigParameterGuid);
			});
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
template<>\
inline bool FCameraRigParameterOverrides::IsParameterOverriden<F##ValueName##CameraRigParameterOverride>(const FGuid& CameraRigParameterGuid) const\
{\
	return this->IsParameterOverriden<F##ValueName##CameraRigParameterOverride>(this->ValueName##Overrides, CameraRigParameterGuid);\
}\
template<>\
inline F##ValueName##CameraRigParameterOverride* FCameraRigParameterOverrides::FindParameterOverride<F##ValueName##CameraRigParameterOverride>(const FGuid& CameraRigParameterGuid)\
{\
	return this->FindParameterOverride<F##ValueName##CameraRigParameterOverride>(this->ValueName##Overrides, CameraRigParameterGuid);\
}\
template<>\
inline F##ValueName##CameraRigParameterOverride& FCameraRigParameterOverrides::FindOrAddParameterOverride<F##ValueName##CameraRigParameterOverride>(const UCameraRigInterfaceParameter* CameraRigParameter)\
{\
	return this->FindOrAddParameterOverride<F##ValueName##CameraRigParameterOverride>(this->ValueName##Overrides, CameraRigParameter);\
}\
template<>\
inline void FCameraRigParameterOverrides::RemoveParameterOverride<F##ValueName##CameraRigParameterOverride>(const FGuid& CameraRigParameterGuid)\
{\
	this->RemoveParameterOverride<F##ValueName##CameraRigParameterOverride>(this->ValueName##Overrides, CameraRigParameterGuid);\
}\
template<>\
inline void FCameraRigParameterOverrides::AppendParameterOverrides<F##ValueName##CameraRigParameterOverride>(TArrayView<F##ValueName##CameraRigParameterOverride> NewOverrides)\
{\
	this->ValueName##Overrides.Append(NewOverrides);\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

/**
 * A structure holding a reference to a camera rig asset, along with the interface parameter
 * override values.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYCAMERAS_API FCameraRigAssetReference
{
	GENERATED_BODY()

public:

	FCameraRigAssetReference();
	FCameraRigAssetReference(UCameraRigAsset* InCameraRig);

	/** Returns whether this reference points to a valid camera rig. */
	bool IsValid() const
	{
		return CameraRig != nullptr;
	}

	/** Gets the referenced camera rig. */
	UCameraRigAsset* GetCameraRig()
	{
		return CameraRig;
	}

	/** Gets the referenced camera rig. */
	const UCameraRigAsset* GetCameraRig() const
	{
		return CameraRig;
	}

	/**
	 * Sets the referenced camerar rig.
	 * This will check any existing parameter overrides, tagging them as invalid as needed.
	 */
	void SetCameraRig(UCameraRigAsset* InCameraRig)
	{
		if (CameraRig != InCameraRig)
		{
			CameraRig = InCameraRig;
			UpdateParameterOverrides();
		}
	}

	/** Gets the parameter overrides. */
	const FCameraRigParameterOverrides& GetParameterOverrides() const
	{
		return ParameterOverrides;
	}

	/** Gets the parameter overrides. */
	FCameraRigParameterOverrides& GetParameterOverrides()
	{
		return ParameterOverrides;
	}

public:

	// Internal API.

	bool SerializeFromMismatchedTag(struct FPropertyTag const& Tag, FStructuredArchive::FSlot Slot);
	bool UpdateParameterOverrides();

private:

	/** The referenced camera rig. */
	UPROPERTY(EditAnywhere, Category="")
	TObjectPtr<UCameraRigAsset> CameraRig;

	/** The parameter overrides. */
	UPROPERTY(EditAnywhere, Category="", meta=(FixedLayout))
	FCameraRigParameterOverrides ParameterOverrides;

	friend class UE::Cameras::FCameraRigAssetReferenceDetailsCustomization;
};

template<>
struct TStructOpsTypeTraits<FCameraRigAssetReference> : public TStructOpsTypeTraitsBase2<FCameraRigAssetReference>
{
	enum
	{
		WithStructuredSerializeFromMismatchedTag = true
	};
};

