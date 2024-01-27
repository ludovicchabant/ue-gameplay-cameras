// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraMode.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraModeTransition.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraMode() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraMode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraMode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraModeTransition();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UCameraMode
void UCameraMode::StaticRegisterNativesUCameraMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraMode);
UClass* Z_Construct_UClass_UCameraMode_NoRegister()
{
	return UCameraMode::StaticClass();
}
struct Z_Construct_UClass_UCameraMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A camera mode asset, which runs a hierarchy of camera nodes to drive \n * the behavior of a camera.\n */" },
		{ "IncludePath", "Core/CameraMode.h" },
		{ "ModuleRelativePath", "Public/Core/CameraMode.h" },
		{ "ToolTip", "A camera mode asset, which runs a hierarchy of camera nodes to drive\nthe behavior of a camera." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RootNode_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** Root camera node. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/CameraMode.h" },
		{ "ToolTip", "Root camera node." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EnterTransitions_MetaData[] = {
		{ "Category", "Blending" },
		{ "Comment", "/** List of enter transitions for this camera mode. */" },
		{ "ModuleRelativePath", "Public/Core/CameraMode.h" },
		{ "ToolTip", "List of enter transitions for this camera mode." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ExitTransitions_MetaData[] = {
		{ "Category", "Blending" },
		{ "Comment", "/** List of exist transitions for this camera mode. */" },
		{ "ModuleRelativePath", "Public/Core/CameraMode.h" },
		{ "ToolTip", "List of exist transitions for this camera mode." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RootNode;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EnterTransitions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_EnterTransitions;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ExitTransitions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ExitTransitions;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UCameraMode_Statics::NewProp_RootNode = { "RootNode", nullptr, (EPropertyFlags)0x0116000000080009, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraMode, RootNode), Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RootNode_MetaData), NewProp_RootNode_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCameraMode_Statics::NewProp_EnterTransitions_Inner = { "EnterTransitions", nullptr, (EPropertyFlags)0x0000008000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FCameraModeTransition, METADATA_PARAMS(0, nullptr) }; // 109385190
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UCameraMode_Statics::NewProp_EnterTransitions = { "EnterTransitions", nullptr, (EPropertyFlags)0x0010008000000001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraMode, EnterTransitions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EnterTransitions_MetaData), NewProp_EnterTransitions_MetaData) }; // 109385190
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCameraMode_Statics::NewProp_ExitTransitions_Inner = { "ExitTransitions", nullptr, (EPropertyFlags)0x0000008000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FCameraModeTransition, METADATA_PARAMS(0, nullptr) }; // 109385190
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UCameraMode_Statics::NewProp_ExitTransitions = { "ExitTransitions", nullptr, (EPropertyFlags)0x0010008000000001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraMode, ExitTransitions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ExitTransitions_MetaData), NewProp_ExitTransitions_MetaData) }; // 109385190
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCameraMode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraMode_Statics::NewProp_RootNode,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraMode_Statics::NewProp_EnterTransitions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraMode_Statics::NewProp_EnterTransitions,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraMode_Statics::NewProp_ExitTransitions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraMode_Statics::NewProp_ExitTransitions,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraMode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCameraMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraMode_Statics::ClassParams = {
	&UCameraMode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCameraMode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCameraMode_Statics::PropPointers),
	0,
	0x008800A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraMode_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraMode()
{
	if (!Z_Registration_Info_UClass_UCameraMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraMode.OuterSingleton, Z_Construct_UClass_UCameraMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraMode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraMode>()
{
	return UCameraMode::StaticClass();
}
UCameraMode::UCameraMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraMode);
UCameraMode::~UCameraMode() {}
// End Class UCameraMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraMode, UCameraMode::StaticClass, TEXT("UCameraMode"), &Z_Registration_Info_UClass_UCameraMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraMode), 1344075933U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraMode_h_3712111306(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
