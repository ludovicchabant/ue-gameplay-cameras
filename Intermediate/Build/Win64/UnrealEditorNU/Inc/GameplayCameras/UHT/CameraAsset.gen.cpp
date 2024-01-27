// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraAsset.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraModeTransition.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraAsset() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraAsset();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraAsset_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraDirector_NoRegister();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraModeTransition();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UCameraAsset
void UCameraAsset::StaticRegisterNativesUCameraAsset()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraAsset);
UClass* Z_Construct_UClass_UCameraAsset_NoRegister()
{
	return UCameraAsset::StaticClass();
}
struct Z_Construct_UClass_UCameraAsset_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A complete camera asset.\n */" },
		{ "IncludePath", "Core/CameraAsset.h" },
		{ "ModuleRelativePath", "Public/Core/CameraAsset.h" },
		{ "ToolTip", "A complete camera asset." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraDirector_MetaData[] = {
		{ "Category", "Director" },
		{ "Comment", "/** The camera director to use in this camera. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/CameraAsset.h" },
		{ "ToolTip", "The camera director to use in this camera." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EnterTransitions_MetaData[] = {
		{ "Category", "Blending" },
		{ "Comment", "/** A list of default enter transitions for all the camera modes in this asset. */" },
		{ "ModuleRelativePath", "Public/Core/CameraAsset.h" },
		{ "ToolTip", "A list of default enter transitions for all the camera modes in this asset." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ExitTransitions_MetaData[] = {
		{ "Category", "Blending" },
		{ "Comment", "/** A list of default exit transitions for all the camera modes in this asset. */" },
		{ "ModuleRelativePath", "Public/Core/CameraAsset.h" },
		{ "ToolTip", "A list of default exit transitions for all the camera modes in this asset." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraDirector;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EnterTransitions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_EnterTransitions;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ExitTransitions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ExitTransitions;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraAsset>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UCameraAsset_Statics::NewProp_CameraDirector = { "CameraDirector", nullptr, (EPropertyFlags)0x0116000000080009, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraAsset, CameraDirector), Z_Construct_UClass_UCameraDirector_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraDirector_MetaData), NewProp_CameraDirector_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCameraAsset_Statics::NewProp_EnterTransitions_Inner = { "EnterTransitions", nullptr, (EPropertyFlags)0x0000008000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FCameraModeTransition, METADATA_PARAMS(0, nullptr) }; // 109385190
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UCameraAsset_Statics::NewProp_EnterTransitions = { "EnterTransitions", nullptr, (EPropertyFlags)0x0010008000000001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraAsset, EnterTransitions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EnterTransitions_MetaData), NewProp_EnterTransitions_MetaData) }; // 109385190
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCameraAsset_Statics::NewProp_ExitTransitions_Inner = { "ExitTransitions", nullptr, (EPropertyFlags)0x0000008000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FCameraModeTransition, METADATA_PARAMS(0, nullptr) }; // 109385190
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UCameraAsset_Statics::NewProp_ExitTransitions = { "ExitTransitions", nullptr, (EPropertyFlags)0x0010008000000001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraAsset, ExitTransitions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ExitTransitions_MetaData), NewProp_ExitTransitions_MetaData) }; // 109385190
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCameraAsset_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAsset_Statics::NewProp_CameraDirector,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAsset_Statics::NewProp_EnterTransitions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAsset_Statics::NewProp_EnterTransitions,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAsset_Statics::NewProp_ExitTransitions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAsset_Statics::NewProp_ExitTransitions,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAsset_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCameraAsset_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAsset_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraAsset_Statics::ClassParams = {
	&UCameraAsset::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCameraAsset_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAsset_Statics::PropPointers),
	0,
	0x008800A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAsset_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraAsset_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraAsset()
{
	if (!Z_Registration_Info_UClass_UCameraAsset.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraAsset.OuterSingleton, Z_Construct_UClass_UCameraAsset_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraAsset.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraAsset>()
{
	return UCameraAsset::StaticClass();
}
UCameraAsset::UCameraAsset(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraAsset);
UCameraAsset::~UCameraAsset() {}
// End Class UCameraAsset

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraAsset_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraAsset, UCameraAsset::StaticClass, TEXT("UCameraAsset"), &Z_Registration_Info_UClass_UCameraAsset, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraAsset), 2089610072U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraAsset_h_3152526830(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraAsset_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraAsset_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
