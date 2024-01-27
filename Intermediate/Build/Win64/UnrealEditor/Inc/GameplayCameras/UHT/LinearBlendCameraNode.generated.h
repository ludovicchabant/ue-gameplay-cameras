// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Nodes/Blends/LinearBlendCameraNode.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYCAMERAS_LinearBlendCameraNode_generated_h
#error "LinearBlendCameraNode.generated.h already included, missing '#pragma once' in LinearBlendCameraNode.h"
#endif
#define GAMEPLAYCAMERAS_LinearBlendCameraNode_generated_h

#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesULinearBlendCameraNode(); \
	friend struct Z_Construct_UClass_ULinearBlendCameraNode_Statics; \
public: \
	DECLARE_CLASS(ULinearBlendCameraNode, USimpleFixedTimeBlendCameraNode, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayCameras"), GAMEPLAYCAMERAS_API) \
	DECLARE_SERIALIZER(ULinearBlendCameraNode)


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_15_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	GAMEPLAYCAMERAS_API ULinearBlendCameraNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ULinearBlendCameraNode(ULinearBlendCameraNode&&); \
	ULinearBlendCameraNode(const ULinearBlendCameraNode&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(GAMEPLAYCAMERAS_API, ULinearBlendCameraNode); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ULinearBlendCameraNode); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ULinearBlendCameraNode) \
	GAMEPLAYCAMERAS_API virtual ~ULinearBlendCameraNode();


#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_12_PROLOG
#define FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_15_INCLASS_NO_PURE_DECLS \
	FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYCAMERAS_API UClass* StaticClass<class ULinearBlendCameraNode>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
