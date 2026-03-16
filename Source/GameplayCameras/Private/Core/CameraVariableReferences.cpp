// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableReferences.h"

#include "Core/CameraVariableTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableReferences)

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
const ValueType* F##ValueName##CameraVariableReference::GetValue(const UE::Cameras::FCameraVariableTable& VariableTable) const\
{\
	if (Variable)\
	{\
		const FCameraVariableID VariableID = Variable->GetVariableID();\
		if (const ValueType* ActualValue = VariableTable.FindValue<ValueType>(VariableID))\
		{\
			return ActualValue;\
		}\
	}\
	return nullptr;\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
PRAGMA_ENABLE_DEPRECATION_WARNINGS

