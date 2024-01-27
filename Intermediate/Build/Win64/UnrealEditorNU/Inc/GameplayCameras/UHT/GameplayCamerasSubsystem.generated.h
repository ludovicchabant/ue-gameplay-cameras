// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayCamerasSubsystem.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class APlayerController;
class UCameraAnimationSequence;
struct FCameraAnimationHandle;
struct FCameraAnimationParams;
#ifdef GAMEPLAYCAMERAS_GameplayCamerasSubsystem_generated_h
#error "GameplayCamerasSubsystem.generated.h already included, missing '#pragma once' in GameplayCamerasSubsystem.h"
#endif
#define GAMEPLAYCAMERAS_GameplayCamerasSubsystem_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execStopAllCameraAnimations); \
	DECLARE_FUNCTION(execStopAllCameraAnimationsOf); \
	DECLARE_FUNCTION(execStopCameraAnimation); \
	DECLARE_FUNCTION(execIsCameraAnimationActive); \
	DECLARE_FUNCTION(execPlayCameraAnimation);


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayCamerasSubsystem(); \
	friend struct Z_Construct_UClass_UGameplayCamerasSubsystem_Statics; \
public: \
	DECLARE_CLASS(UGameplayCamerasSubsystem, UWorldSubsystem, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), NO_API) \
	DECLARE_SERIALIZER(UGameplayCamerasSubsystem)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayCamerasSubsystem(); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UGameplayCamerasSubsystem(UGameplayCamerasSubsystem&&); \
	UGameplayCamerasSubsystem(const UGameplayCamerasSubsystem&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayCamerasSubsystem); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayCamerasSubsystem); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UGameplayCamerasSubsystem) \
	NO_API virtual ~UGameplayCamerasSubsystem();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_15_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_18_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class UGameplayCamerasSubsystem>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
