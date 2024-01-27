// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Directors/SingleCameraDirector.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYCAMERAS_SingleCameraDirector_generated_h
#error "SingleCameraDirector.generated.h already included, missing '#pragma once' in SingleCameraDirector.h"
#endif
#define GAMEPLAYCAMERAS_SingleCameraDirector_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSingleCameraDirector(); \
	friend struct Z_Construct_UClass_USingleCameraDirector_Statics; \
public: \
	DECLARE_CLASS(USingleCameraDirector, UCameraDirector, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), NO_API) \
	DECLARE_SERIALIZER(USingleCameraDirector)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_15_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	USingleCameraDirector(USingleCameraDirector&&); \
	USingleCameraDirector(const USingleCameraDirector&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, USingleCameraDirector); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USingleCameraDirector); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(USingleCameraDirector) \
	NO_API virtual ~USingleCameraDirector();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_12_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_15_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class USingleCameraDirector>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
