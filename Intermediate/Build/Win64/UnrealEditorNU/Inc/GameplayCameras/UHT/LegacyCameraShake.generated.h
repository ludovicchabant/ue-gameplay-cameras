// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Shakes/LegacyCameraShake.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class APlayerCameraManager;
class UCameraShakeBase;
class UCameraShakeSourceComponent;
class ULegacyCameraShake;
enum class ECameraShakePlaySpace : uint8;
struct FMinimalViewInfo;
#ifdef GAMEPLAYCAMERAS_LegacyCameraShake_generated_h
#error "LegacyCameraShake.generated.h already included, missing '#pragma once' in LegacyCameraShake.h"
#endif
#define GAMEPLAYCAMERAS_LegacyCameraShake_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_43_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FFOscillator_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<struct FFOscillator>();

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_85_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FROscillator_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<struct FROscillator>();

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_105_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FVOscillator_Statics; \
	GAMEPLAYCAMERAS_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<struct FVOscillator>();

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_RPC_WRAPPERS \
	virtual bool ReceiveIsFinished_Implementation() const; \
	DECLARE_FUNCTION(execStartLegacyCameraShakeFromSource); \
	DECLARE_FUNCTION(execStartLegacyCameraShake); \
	DECLARE_FUNCTION(execReceiveIsFinished);


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_CALLBACK_WRAPPERS
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_INCLASS \
private: \
	static void StaticRegisterNativesULegacyCameraShake(); \
	friend struct Z_Construct_UClass_ULegacyCameraShake_Statics; \
public: \
	DECLARE_CLASS(ULegacyCameraShake, UCameraShakeBase, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), NO_API) \
	DECLARE_SERIALIZER(ULegacyCameraShake)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ULegacyCameraShake(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ULegacyCameraShake) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ULegacyCameraShake); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ULegacyCameraShake); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ULegacyCameraShake(ULegacyCameraShake&&); \
	ULegacyCameraShake(const ULegacyCameraShake&); \
public: \
	NO_API virtual ~ULegacyCameraShake();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_124_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_RPC_WRAPPERS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_CALLBACK_WRAPPERS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_INCLASS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_127_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class ULegacyCameraShake>();

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_299_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesULegacyCameraShakePattern(); \
	friend struct Z_Construct_UClass_ULegacyCameraShakePattern_Statics; \
public: \
	DECLARE_CLASS(ULegacyCameraShakePattern, UCameraShakePattern, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), NO_API) \
	DECLARE_SERIALIZER(ULegacyCameraShakePattern)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_299_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ULegacyCameraShakePattern(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ULegacyCameraShakePattern(ULegacyCameraShakePattern&&); \
	ULegacyCameraShakePattern(const ULegacyCameraShakePattern&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ULegacyCameraShakePattern); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ULegacyCameraShakePattern); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ULegacyCameraShakePattern) \
	NO_API virtual ~ULegacyCameraShakePattern();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_296_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_299_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_299_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_299_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class ULegacyCameraShakePattern>();

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execConv_LegacyCameraShake);


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesULegacyCameraShakeFunctionLibrary(); \
	friend struct Z_Construct_UClass_ULegacyCameraShakeFunctionLibrary_Statics; \
public: \
	DECLARE_CLASS(ULegacyCameraShakeFunctionLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), NO_API) \
	DECLARE_SERIALIZER(ULegacyCameraShakeFunctionLibrary)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ULegacyCameraShakeFunctionLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ULegacyCameraShakeFunctionLibrary(ULegacyCameraShakeFunctionLibrary&&); \
	ULegacyCameraShakeFunctionLibrary(const ULegacyCameraShakeFunctionLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ULegacyCameraShakeFunctionLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ULegacyCameraShakeFunctionLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ULegacyCameraShakeFunctionLibrary) \
	NO_API virtual ~ULegacyCameraShakeFunctionLibrary();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_317_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h_320_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class ULegacyCameraShakeFunctionLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_LegacyCameraShake_h


#define FOREACH_ENUM_EOSCILLATORWAVEFORM(op) \
	op(EOscillatorWaveform::SineWave) \
	op(EOscillatorWaveform::PerlinNoise) 

enum class EOscillatorWaveform : uint8;
template<> struct TIsUEnumClass<EOscillatorWaveform> { enum { Value = true }; };
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<EOscillatorWaveform>();

#define FOREACH_ENUM_EINITIALOSCILLATOROFFSET(op) \
	op(EOO_OffsetRandom) \
	op(EOO_OffsetZero) 

enum EInitialOscillatorOffset : int;
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<EInitialOscillatorOffset>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
