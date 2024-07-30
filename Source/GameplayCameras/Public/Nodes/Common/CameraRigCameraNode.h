// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"

#include "CameraRigCameraNode.generated.h"

namespace UE::Cameras
{
	class FCameraRigCameraNodeEvaluator;
}

USTRUCT()
struct FBooleanCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FBooleanCameraParameter;

	UPROPERTY()
	FBooleanCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FInteger32CameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FInteger32CameraParameter;

	UPROPERTY()
	FInteger32CameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FFloatCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FFloatCameraParameter;

	UPROPERTY()
	FFloatCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FDoubleCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FDoubleCameraParameter;

	UPROPERTY()
	FDoubleCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FVector2fCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FVector2fCameraParameter;

	UPROPERTY()
	FVector2fCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FVector2dCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FVector2dCameraParameter;

	UPROPERTY()
	FVector2dCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FVector3fCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FVector3fCameraParameter;

	UPROPERTY()
	FVector3fCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FVector3dCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FVector3dCameraParameter;

	UPROPERTY()
	FVector3dCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FVector4fCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FVector4fCameraParameter;

	UPROPERTY()
	FVector4fCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FVector4dCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FVector4dCameraParameter;

	UPROPERTY()
	FVector4dCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FRotator3fCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FRotator3fCameraParameter;

	UPROPERTY()
	FRotator3fCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FRotator3dCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FRotator3dCameraParameter;

	UPROPERTY()
	FRotator3dCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FTransform3fCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FTransform3fCameraParameter;

	UPROPERTY()
	FTransform3fCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

USTRUCT()
struct FTransform3dCameraRigParameterOverride
{
	GENERATED_BODY()

	using CameraParameterType = FTransform3dCameraParameter;

	UPROPERTY()
	FTransform3dCameraParameter Value;

	UPROPERTY()
	FGuid PrivateVariableGuid;

	UPROPERTY()
	FGuid InterfaceParameterGuid;

	UPROPERTY()
	FString InterfaceParameterName;
};

/**
 * A camera node that runs a camera rig's own node tree.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Utility"))
class UCameraRigCameraNode : public UCameraNode
{
	GENERATED_BODY()

public:

	template<typename ParameterOverrideType>
	ParameterOverrideType* FindParameterOverride(const FGuid& CameraRigParameterGuid);
	
	template<typename ParameterOverrideType>
	ParameterOverrideType& GetOrAddParameterOverride(const UCameraRigInterfaceParameter* CameraRigParameter);

	template<typename ParameterOverrideType>
	void RemoveParameterOverride(const FGuid& CameraRigParameterGuid);

private:

	template<typename ParameterOverrideType>
	ParameterOverrideType* FindParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid);
	
	template<typename ParameterOverrideType>
	ParameterOverrideType& GetOrAddParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const UCameraRigInterfaceParameter* CameraRigParameter);

	template<typename ParameterOverrideType>
	void RemoveParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid);

protected:

	// UCameraNode interface.
	virtual void OnBuild(FCameraRigBuildContext& BuildContext) override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

	/** The camera rig to run. */
	UPROPERTY(EditAnywhere, Category=Common, meta=(ObjectTreeGraphHidden=true))
	TObjectPtr<UCameraRigAsset> CameraRig;

private:

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

	friend class UE::Cameras::FCameraRigCameraNodeEvaluator;
};

template<typename ParameterOverrideType>
ParameterOverrideType* UCameraRigCameraNode::FindParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid)
{
	ParameterOverrideType* FoundItem = OverridesArray.FindByPredicate(
			[CameraRigParameterGuid](ParameterOverrideType& Item)
			{
				return (Item.InterfaceParameterGuid == CameraRigParameterGuid);
			});
	return FoundItem;
}

template<typename ParameterOverrideType>
ParameterOverrideType& UCameraRigCameraNode::GetOrAddParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const UCameraRigInterfaceParameter* CameraRigParameter)
{
	ParameterOverrideType* Existing = FindParameterOverride<ParameterOverrideType>(CameraRigParameter->Guid);
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
void UCameraRigCameraNode::RemoveParameterOverride(TArray<ParameterOverrideType>& OverridesArray, const FGuid& CameraRigParameterGuid)
{
	OverridesArray.RemoveAll(
			[CameraRigParameterGuid](ParameterOverrideType& Item)
			{
				return (Item.InterfaceParameterGuid == CameraRigParameterGuid);
			});
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
template<>\
inline F##ValueName##CameraRigParameterOverride* UCameraRigCameraNode::FindParameterOverride<F##ValueName##CameraRigParameterOverride>(const FGuid& CameraRigParameterGuid)\
{\
	return FindParameterOverride<F##ValueName##CameraRigParameterOverride>(ValueName##Overrides, CameraRigParameterGuid);\
}\
template<>\
inline F##ValueName##CameraRigParameterOverride& UCameraRigCameraNode::GetOrAddParameterOverride<F##ValueName##CameraRigParameterOverride>(const UCameraRigInterfaceParameter* CameraRigParameter)\
{\
	return GetOrAddParameterOverride<F##ValueName##CameraRigParameterOverride>(ValueName##Overrides, CameraRigParameter);\
}\
template<>\
inline void UCameraRigCameraNode::RemoveParameterOverride<F##ValueName##CameraRigParameterOverride>(const FGuid& CameraRigParameterGuid)\
{\
	RemoveParameterOverride<F##ValueName##CameraRigParameterOverride>(ValueName##Overrides, CameraRigParameterGuid);\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

