// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraVariableReferences.h"

#include "Core/CameraVariableTable.h"

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
const ValueType* F##ValueName##CameraVariableReference::GetValue(const UE::Cameras::FCameraVariableTable& VariableTable) const\
{\
	if (VariableID)\
	{\
		if (const ValueType* ActualValue = VariableTable.FindValue<ValueType>(VariableID))\
		{\
			return ActualValue;\
		}\
	}\
	return nullptr;\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

