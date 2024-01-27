// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Shakes/WaveOscillatorCameraShakePattern.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYCAMERAS_WaveOscillatorCameraShakePattern_generated_h
#error "WaveOscillatorCameraShakePattern.generated.h already included, missing '#pragma once' in WaveOscillatorCameraShakePattern.h"
#endif
#define GAMEPLAYCAMERAS_WaveOscillatorCameraShakePattern_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_22_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FWaveOscillator_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<struct FWaveOscillator>();

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_58_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUWaveOscillatorCameraShakePattern(); \
	friend struct Z_Construct_UClass_UWaveOscillatorCameraShakePattern_Statics; \
public: \
	DECLARE_CLASS(UWaveOscillatorCameraShakePattern, USimpleCameraShakePattern, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), NO_API) \
	DECLARE_SERIALIZER(UWaveOscillatorCameraShakePattern)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_58_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UWaveOscillatorCameraShakePattern(UWaveOscillatorCameraShakePattern&&); \
	UWaveOscillatorCameraShakePattern(const UWaveOscillatorCameraShakePattern&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UWaveOscillatorCameraShakePattern); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UWaveOscillatorCameraShakePattern); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UWaveOscillatorCameraShakePattern) \
	NO_API virtual ~UWaveOscillatorCameraShakePattern();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_53_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_58_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_58_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h_58_ENHANCED_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class UWaveOscillatorCameraShakePattern>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_WaveOscillatorCameraShakePattern_h


#define FOREACH_ENUM_EINITIALWAVEOSCILLATOROFFSETTYPE(op) \
	op(EInitialWaveOscillatorOffsetType::Random) \
	op(EInitialWaveOscillatorOffsetType::Zero) 

enum class EInitialWaveOscillatorOffsetType : uint8;
template<> struct TIsUEnumClass<EInitialWaveOscillatorOffsetType> { enum { Value = true }; };
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<EInitialWaveOscillatorOffsetType>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
