// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCamerasEditor/Private/AssetTools/AssetDefinition_CameraMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAssetDefinition_CameraMode() {}

// Begin Cross Module References
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_CameraMode();
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_CameraMode_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UAssetDefinitionDefault();
UPackage* Z_Construct_UPackage__Script_GameplayCamerasEditor();
// End Cross Module References

// Begin Class UAssetDefinition_CameraMode
void UAssetDefinition_CameraMode::StaticRegisterNativesUAssetDefinition_CameraMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAssetDefinition_CameraMode);
UClass* Z_Construct_UClass_UAssetDefinition_CameraMode_NoRegister()
{
	return UAssetDefinition_CameraMode::StaticClass();
}
struct Z_Construct_UClass_UAssetDefinition_CameraMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "AssetTools/AssetDefinition_CameraMode.h" },
		{ "ModuleRelativePath", "Private/AssetTools/AssetDefinition_CameraMode.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAssetDefinition_CameraMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UAssetDefinition_CameraMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAssetDefinitionDefault,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCamerasEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAssetDefinition_CameraMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAssetDefinition_CameraMode_Statics::ClassParams = {
	&UAssetDefinition_CameraMode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAssetDefinition_CameraMode_Statics::Class_MetaDataParams), Z_Construct_UClass_UAssetDefinition_CameraMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAssetDefinition_CameraMode()
{
	if (!Z_Registration_Info_UClass_UAssetDefinition_CameraMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAssetDefinition_CameraMode.OuterSingleton, Z_Construct_UClass_UAssetDefinition_CameraMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAssetDefinition_CameraMode.OuterSingleton;
}
template<> GAMEPLAYCAMERASEDITOR_API UClass* StaticClass<UAssetDefinition_CameraMode>()
{
	return UAssetDefinition_CameraMode::StaticClass();
}
UAssetDefinition_CameraMode::UAssetDefinition_CameraMode() {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAssetDefinition_CameraMode);
UAssetDefinition_CameraMode::~UAssetDefinition_CameraMode() {}
// End Class UAssetDefinition_CameraMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAssetDefinition_CameraMode, UAssetDefinition_CameraMode::StaticClass, TEXT("UAssetDefinition_CameraMode"), &Z_Registration_Info_UClass_UAssetDefinition_CameraMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAssetDefinition_CameraMode), 37683959U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraMode_h_1167354626(TEXT("/Script/GameplayCamerasEditor"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
