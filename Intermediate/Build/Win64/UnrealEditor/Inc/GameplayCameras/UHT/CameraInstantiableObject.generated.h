// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Core/CameraInstantiableObject.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYCAMERAS_CameraInstantiableObject_generated_h
#error "CameraInstantiableObject.generated.h already included, missing '#pragma once' in CameraInstantiableObject.h"
#endif
#define GAMEPLAYCAMERAS_CameraInstantiableObject_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_28_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUCameraInstantiableObject(); \
	friend struct Z_Construct_UClass_UCameraInstantiableObject_Statics; \
public: \
	DECLARE_CLASS(UCameraInstantiableObject, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), GAMEPLAYCAMERAS_API) \
	DECLARE_SERIALIZER(UCameraInstantiableObject)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_28_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	GAMEPLAYCAMERAS_API UCameraInstantiableObject(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UCameraInstantiableObject(UCameraInstantiableObject&&); \
	UCameraInstantiableObject(const UCameraInstantiableObject&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(GAMEPLAYCAMERAS_API, UCameraInstantiableObject); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UCameraInstantiableObject); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UCameraInstantiableObject) \
	GAMEPLAYCAMERAS_API virtual ~UCameraInstantiableObject();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_25_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_28_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_28_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_28_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class UCameraInstantiableObject>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h


#define FOREACH_ENUM_ECAMERANODEINSTANTIATIONSTATE(op) \
	op(ECameraNodeInstantiationState::None) \
	op(ECameraNodeInstantiationState::HasInstantiations) \
	op(ECameraNodeInstantiationState::IsInstantiated) 

enum class ECameraNodeInstantiationState;
template<> struct TIsUEnumClass<ECameraNodeInstantiationState> { enum { Value = true }; };
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ECameraNodeInstantiationState>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
