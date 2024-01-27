// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Directors/SingleCameraDirector.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSingleCameraDirector() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraDirector();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraMode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USingleCameraDirector();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USingleCameraDirector_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class USingleCameraDirector
void USingleCameraDirector::StaticRegisterNativesUSingleCameraDirector()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USingleCameraDirector);
UClass* Z_Construct_UClass_USingleCameraDirector_NoRegister()
{
	return USingleCameraDirector::StaticClass();
}
struct Z_Construct_UClass_USingleCameraDirector_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A simple camera director that only ever returns one single camera mode.\n */" },
		{ "IncludePath", "Directors/SingleCameraDirector.h" },
		{ "ModuleRelativePath", "Public/Directors/SingleCameraDirector.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A simple camera director that only ever returns one single camera mode." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraMode_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** The camera mode to run every frame. */" },
		{ "ModuleRelativePath", "Public/Directors/SingleCameraDirector.h" },
		{ "ToolTip", "The camera mode to run every frame." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraMode;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USingleCameraDirector>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USingleCameraDirector_Statics::NewProp_CameraMode = { "CameraMode", nullptr, (EPropertyFlags)0x0114000000000001, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USingleCameraDirector, CameraMode), Z_Construct_UClass_UCameraMode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraMode_MetaData), NewProp_CameraMode_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USingleCameraDirector_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USingleCameraDirector_Statics::NewProp_CameraMode,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USingleCameraDirector_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_USingleCameraDirector_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraDirector,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USingleCameraDirector_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USingleCameraDirector_Statics::ClassParams = {
	&USingleCameraDirector::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_USingleCameraDirector_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_USingleCameraDirector_Statics::PropPointers),
	0,
	0x002010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USingleCameraDirector_Statics::Class_MetaDataParams), Z_Construct_UClass_USingleCameraDirector_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USingleCameraDirector()
{
	if (!Z_Registration_Info_UClass_USingleCameraDirector.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USingleCameraDirector.OuterSingleton, Z_Construct_UClass_USingleCameraDirector_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USingleCameraDirector.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<USingleCameraDirector>()
{
	return USingleCameraDirector::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(USingleCameraDirector);
USingleCameraDirector::~USingleCameraDirector() {}
// End Class USingleCameraDirector

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USingleCameraDirector, USingleCameraDirector::StaticClass, TEXT("USingleCameraDirector"), &Z_Registration_Info_UClass_USingleCameraDirector, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USingleCameraDirector), 3594975801U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_3883372241(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Directors_SingleCameraDirector_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
