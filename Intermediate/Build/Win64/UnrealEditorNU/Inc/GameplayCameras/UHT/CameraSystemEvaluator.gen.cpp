// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraSystemEvaluator.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraSystemEvaluator() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraSystemEvaluator();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraSystemEvaluator_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_URootCameraNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UCameraSystemEvaluator
void UCameraSystemEvaluator::StaticRegisterNativesUCameraSystemEvaluator()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraSystemEvaluator);
UClass* Z_Construct_UClass_UCameraSystemEvaluator_NoRegister()
{
	return UCameraSystemEvaluator::StaticClass();
}
struct Z_Construct_UClass_UCameraSystemEvaluator_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * The main camera system evaluator class.\n */" },
		{ "IncludePath", "Core/CameraSystemEvaluator.h" },
		{ "ModuleRelativePath", "Public/Core/CameraSystemEvaluator.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "The main camera system evaluator class." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RootNode_MetaData[] = {
		{ "Comment", "/** The root camera node. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/CameraSystemEvaluator.h" },
		{ "ToolTip", "The root camera node." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RootNode;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraSystemEvaluator>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UCameraSystemEvaluator_Statics::NewProp_RootNode = { "RootNode", nullptr, (EPropertyFlags)0x0144000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraSystemEvaluator, RootNode), Z_Construct_UClass_URootCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RootNode_MetaData), NewProp_RootNode_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCameraSystemEvaluator_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraSystemEvaluator_Statics::NewProp_RootNode,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraSystemEvaluator_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCameraSystemEvaluator_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraSystemEvaluator_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraSystemEvaluator_Statics::ClassParams = {
	&UCameraSystemEvaluator::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCameraSystemEvaluator_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCameraSystemEvaluator_Statics::PropPointers),
	0,
	0x008800A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraSystemEvaluator_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraSystemEvaluator_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraSystemEvaluator()
{
	if (!Z_Registration_Info_UClass_UCameraSystemEvaluator.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraSystemEvaluator.OuterSingleton, Z_Construct_UClass_UCameraSystemEvaluator_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraSystemEvaluator.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraSystemEvaluator>()
{
	return UCameraSystemEvaluator::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraSystemEvaluator);
UCameraSystemEvaluator::~UCameraSystemEvaluator() {}
// End Class UCameraSystemEvaluator

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraSystemEvaluator_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraSystemEvaluator, UCameraSystemEvaluator::StaticClass, TEXT("UCameraSystemEvaluator"), &Z_Registration_Info_UClass_UCameraSystemEvaluator, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraSystemEvaluator), 3312146645U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraSystemEvaluator_h_131338666(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraSystemEvaluator_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraSystemEvaluator_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
