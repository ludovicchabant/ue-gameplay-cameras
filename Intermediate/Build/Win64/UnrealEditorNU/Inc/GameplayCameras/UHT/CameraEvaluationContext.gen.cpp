// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraEvaluationContext.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraEvaluationContext() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraAsset_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraEvaluationContext();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraEvaluationContext_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UCameraEvaluationContext
void UCameraEvaluationContext::StaticRegisterNativesUCameraEvaluationContext()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraEvaluationContext);
UClass* Z_Construct_UClass_UCameraEvaluationContext_NoRegister()
{
	return UCameraEvaluationContext::StaticClass();
}
struct Z_Construct_UClass_UCameraEvaluationContext_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Base class for providing a context to running camera modes.\n */" },
		{ "IncludePath", "Core/CameraEvaluationContext.h" },
		{ "ModuleRelativePath", "Public/Core/CameraEvaluationContext.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "Base class for providing a context to running camera modes." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraAsset_MetaData[] = {
		{ "Comment", "/** The camera asset hosted in this context. */" },
		{ "ModuleRelativePath", "Public/Core/CameraEvaluationContext.h" },
		{ "ToolTip", "The camera asset hosted in this context." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraAsset;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraEvaluationContext>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UCameraEvaluationContext_Statics::NewProp_CameraAsset = { "CameraAsset", nullptr, (EPropertyFlags)0x0124080000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraEvaluationContext, CameraAsset), Z_Construct_UClass_UCameraAsset_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraAsset_MetaData), NewProp_CameraAsset_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCameraEvaluationContext_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraEvaluationContext_Statics::NewProp_CameraAsset,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraEvaluationContext_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCameraEvaluationContext_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraEvaluationContext_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraEvaluationContext_Statics::ClassParams = {
	&UCameraEvaluationContext::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCameraEvaluationContext_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCameraEvaluationContext_Statics::PropPointers),
	0,
	0x000800A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraEvaluationContext_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraEvaluationContext_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraEvaluationContext()
{
	if (!Z_Registration_Info_UClass_UCameraEvaluationContext.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraEvaluationContext.OuterSingleton, Z_Construct_UClass_UCameraEvaluationContext_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraEvaluationContext.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraEvaluationContext>()
{
	return UCameraEvaluationContext::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraEvaluationContext);
UCameraEvaluationContext::~UCameraEvaluationContext() {}
// End Class UCameraEvaluationContext

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraEvaluationContext_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraEvaluationContext, UCameraEvaluationContext::StaticClass, TEXT("UCameraEvaluationContext"), &Z_Registration_Info_UClass_UCameraEvaluationContext, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraEvaluationContext), 2766734003U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraEvaluationContext_h_4234416996(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraEvaluationContext_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraEvaluationContext_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
