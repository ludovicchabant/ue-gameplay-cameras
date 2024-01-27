// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCamerasEditor/Private/AssetTools/AssetDefinition_CameraAsset.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAssetDefinition_CameraAsset() {}

// Begin Cross Module References
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_CameraAsset();
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_CameraAsset_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UAssetDefinitionDefault();
UPackage* Z_Construct_UPackage__Script_GameplayCamerasEditor();
// End Cross Module References

// Begin Class UAssetDefinition_CameraAsset
void UAssetDefinition_CameraAsset::StaticRegisterNativesUAssetDefinition_CameraAsset()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAssetDefinition_CameraAsset);
UClass* Z_Construct_UClass_UAssetDefinition_CameraAsset_NoRegister()
{
	return UAssetDefinition_CameraAsset::StaticClass();
}
struct Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "AssetTools/AssetDefinition_CameraAsset.h" },
		{ "ModuleRelativePath", "Private/AssetTools/AssetDefinition_CameraAsset.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAssetDefinition_CameraAsset>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAssetDefinitionDefault,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCamerasEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics::ClassParams = {
	&UAssetDefinition_CameraAsset::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics::Class_MetaDataParams), Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAssetDefinition_CameraAsset()
{
	if (!Z_Registration_Info_UClass_UAssetDefinition_CameraAsset.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAssetDefinition_CameraAsset.OuterSingleton, Z_Construct_UClass_UAssetDefinition_CameraAsset_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAssetDefinition_CameraAsset.OuterSingleton;
}
template<> GAMEPLAYCAMERASEDITOR_API UClass* StaticClass<UAssetDefinition_CameraAsset>()
{
	return UAssetDefinition_CameraAsset::StaticClass();
}
UAssetDefinition_CameraAsset::UAssetDefinition_CameraAsset() {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAssetDefinition_CameraAsset);
UAssetDefinition_CameraAsset::~UAssetDefinition_CameraAsset() {}
// End Class UAssetDefinition_CameraAsset

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraAsset_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAssetDefinition_CameraAsset, UAssetDefinition_CameraAsset::StaticClass, TEXT("UAssetDefinition_CameraAsset"), &Z_Registration_Info_UClass_UAssetDefinition_CameraAsset, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAssetDefinition_CameraAsset), 1531103228U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraAsset_h_3085030687(TEXT("/Script/GameplayCamerasEditor"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraAsset_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_AssetTools_AssetDefinition_CameraAsset_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
