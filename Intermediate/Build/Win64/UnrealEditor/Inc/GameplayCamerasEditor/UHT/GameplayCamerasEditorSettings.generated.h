// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayCamerasEditorSettings.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYCAMERASEDITOR_GameplayCamerasEditorSettings_generated_h
#error "GameplayCamerasEditorSettings.generated.h already included, missing '#pragma once' in GameplayCamerasEditorSettings.h"
#endif
#define GAMEPLAYCAMERASEDITOR_GameplayCamerasEditorSettings_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_16_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayCamerasEditorSettings(); \
	friend struct Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics; \
public: \
	DECLARE_CLASS(UGameplayCamerasEditorSettings, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCamerasEditor"), NO_API) \
	DECLARE_SERIALIZER(UGameplayCamerasEditorSettings) \
	static const TCHAR* StaticConfigName() {return TEXT("EditorPerProjectUserSettings");} \



#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_16_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UGameplayCamerasEditorSettings(UGameplayCamerasEditorSettings&&); \
	UGameplayCamerasEditorSettings(const UGameplayCamerasEditorSettings&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayCamerasEditorSettings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayCamerasEditorSettings); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayCamerasEditorSettings) \
	NO_API virtual ~UGameplayCamerasEditorSettings();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_13_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_16_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_16_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_16_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERASEDITOR_API UClass* StaticClass<class UGameplayCamerasEditorSettings>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
