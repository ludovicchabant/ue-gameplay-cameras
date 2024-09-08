// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAssetReference.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAssetReference)

void FCameraRigParameterOverrides::Reset()
{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	ValueName##Overrides.Reset();
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
}

FCameraRigAssetReference::FCameraRigAssetReference()
{
}

FCameraRigAssetReference::FCameraRigAssetReference(UCameraRigAsset* InCameraRig)
	: CameraRig(InCameraRig)
{
}

bool FCameraRigAssetReference::UpdateParameterOverrides()
{
	if (!CameraRig)
	{
		bool bHasAnyOverride = false;
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides.ValueName##Overrides)\
		{\
			ParameterOverride.bInvalid = true;\
			bHasAnyOverride = true;\
		}
		UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		return bHasAnyOverride;
	}

	bool bAnyModified = false;
	FCameraRigInterface& CameraRigInterface = CameraRig->Interface;
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides.ValueName##Overrides)\
	{\
		UCameraRigInterfaceParameter* InterfaceParameter = CameraRigInterface.FindInterfaceParameterByGuid(\
				ParameterOverride.InterfaceParameterGuid);\
		\
		const bool bWasInvalid = ParameterOverride.bInvalid;\
		ParameterOverride.bInvalid = (InterfaceParameter == nullptr);\
		bAnyModified |= (bWasInvalid != ParameterOverride.bInvalid);\
		\
		if (InterfaceParameter)\
		{\
			if (ParameterOverride.InterfaceParameterName != InterfaceParameter->InterfaceParameterName)\
			{\
				ParameterOverride.InterfaceParameterName = InterfaceParameter->InterfaceParameterName;\
				bAnyModified = true;\
			}\
			UCameraVariableAsset* InterfaceParameterVariable = InterfaceParameter->PrivateVariable;\
			const FGuid NewPrivateVariableGuid = InterfaceParameterVariable ? InterfaceParameterVariable->GetGuid() : FGuid();\
			if (ParameterOverride.PrivateVariableGuid != NewPrivateVariableGuid)\
			{\
				ParameterOverride.PrivateVariableGuid = NewPrivateVariableGuid;\
				bAnyModified = true;\
			}\
		}\
	}
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	return bAnyModified;
}

bool FCameraRigAssetReference::SerializeFromMismatchedTag(struct FPropertyTag const& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_SoftObjectProperty)
	{
		FSoftObjectPtr CameraRigPath;
		Slot << CameraRigPath;
		CameraRig = Cast<UCameraRigAsset>(CameraRigPath.Get());
		return true;
	}
	return false;
}

