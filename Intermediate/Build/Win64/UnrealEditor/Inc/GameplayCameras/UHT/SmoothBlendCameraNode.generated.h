// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Nodes/Blends/SmoothBlendCameraNode.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYCAMERAS_SmoothBlendCameraNode_generated_h
#error "SmoothBlendCameraNode.generated.h already included, missing '#pragma once' in SmoothBlendCameraNode.h"
#endif
#define GAMEPLAYCAMERAS_SmoothBlendCameraNode_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_25_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSmoothBlendCameraNode(); \
	friend struct Z_Construct_UClass_USmoothBlendCameraNode_Statics; \
public: \
	DECLARE_CLASS(USmoothBlendCameraNode, USimpleFixedTimeBlendCameraNode, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), GAMEPLAYCAMERAS_API) \
	DECLARE_SERIALIZER(USmoothBlendCameraNode)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_25_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	GAMEPLAYCAMERAS_API USmoothBlendCameraNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	USmoothBlendCameraNode(USmoothBlendCameraNode&&); \
	USmoothBlendCameraNode(const USmoothBlendCameraNode&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(GAMEPLAYCAMERAS_API, USmoothBlendCameraNode); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USmoothBlendCameraNode); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(USmoothBlendCameraNode) \
	GAMEPLAYCAMERAS_API virtual ~USmoothBlendCameraNode();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_22_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_25_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_25_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_25_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class USmoothBlendCameraNode>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h


#define FOREACH_ENUM_ESMOOTHCAMERABLENDTYPE(op) \
	op(ESmoothCameraBlendType::SmoothStep) \
	op(ESmoothCameraBlendType::SmootherStep) 

enum class ESmoothCameraBlendType;
template<> struct TIsUEnumClass<ESmoothCameraBlendType> { enum { Value = true }; };
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ESmoothCameraBlendType>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
