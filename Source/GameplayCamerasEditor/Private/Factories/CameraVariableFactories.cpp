// Copyright Epic Games, Inc. All Rights Reserved.

#include "Factories/CameraVariableFactories.h"

#include "Core/CameraVariableAssets.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableFactories)

#define LOCTEXT_NAMESPACE "CameraVariableFactories"

UCameraVariableAssetFactory::UCameraVariableAssetFactory(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	bCreateNew = true;
	bEditAfterNew = true;
}

bool UCameraVariableAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	U##ValueName##CameraVariableFactory::U##ValueName##CameraVariableFactory(const FObjectInitializer& ObjectInit)\
		: Super(ObjectInit)\
	{\
		SupportedClass = U##ValueName##CameraVariable::StaticClass();\
	}\
	UObject* U##ValueName##CameraVariableFactory::FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)\
	{\
		return NewObject<U##ValueName##CameraVariable>(Parent, Class, Name, Flags | RF_Transactional);\
	}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

#undef LOCTEXT_NAMESPACE

