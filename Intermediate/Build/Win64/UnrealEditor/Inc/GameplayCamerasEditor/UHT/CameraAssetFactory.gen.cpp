// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCamerasEditor/Private/Factories/CameraAssetFactory.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraAssetFactory() {}

// Begin Cross Module References
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UCameraAssetFactory();
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UCameraAssetFactory_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UFactory();
UPackage* Z_Construct_UPackage__Script_GameplayCamerasEditor();
// End Cross Module References

// Begin Class UCameraAssetFactory
void UCameraAssetFactory::StaticRegisterNativesUCameraAssetFactory()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraAssetFactory);
UClass* Z_Construct_UClass_UCameraAssetFactory_NoRegister()
{
	return UCameraAssetFactory::StaticClass();
}
struct Z_Construct_UClass_UCameraAssetFactory_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Implements a factory for UCameraAsset objects.\n */" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "Factories/CameraAssetFactory.h" },
		{ "ModuleRelativePath", "Private/Factories/CameraAssetFactory.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "Implements a factory for UCameraAsset objects." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraAssetFactory>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UCameraAssetFactory_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UFactory,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCamerasEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAssetFactory_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraAssetFactory_Statics::ClassParams = {
	&UCameraAssetFactory::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAssetFactory_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraAssetFactory_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraAssetFactory()
{
	if (!Z_Registration_Info_UClass_UCameraAssetFactory.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraAssetFactory.OuterSingleton, Z_Construct_UClass_UCameraAssetFactory_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraAssetFactory.OuterSingleton;
}
template<> GAMEPLAYCAMERASEDITOR_API UClass* StaticClass<UCameraAssetFactory>()
{
	return UCameraAssetFactory::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraAssetFactory);
UCameraAssetFactory::~UCameraAssetFactory() {}
// End Class UCameraAssetFactory

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraAssetFactory_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraAssetFactory, UCameraAssetFactory::StaticClass, TEXT("UCameraAssetFactory"), &Z_Registration_Info_UClass_UCameraAssetFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraAssetFactory), 635418042U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraAssetFactory_h_2062079464(TEXT("/Script/GameplayCamerasEditor"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraAssetFactory_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraAssetFactory_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
