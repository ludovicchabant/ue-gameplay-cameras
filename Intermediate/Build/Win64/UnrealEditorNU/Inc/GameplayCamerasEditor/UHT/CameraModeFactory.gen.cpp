// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCamerasEditor/Private/Factories/CameraModeFactory.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraModeFactory() {}

// Begin Cross Module References
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UCameraModeFactory();
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UCameraModeFactory_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UFactory();
UPackage* Z_Construct_UPackage__Script_GameplayCamerasEditor();
// End Cross Module References

// Begin Class UCameraModeFactory
void UCameraModeFactory::StaticRegisterNativesUCameraModeFactory()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraModeFactory);
UClass* Z_Construct_UClass_UCameraModeFactory_NoRegister()
{
	return UCameraModeFactory::StaticClass();
}
struct Z_Construct_UClass_UCameraModeFactory_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Implements a factory for UCameraMode objects.\n */" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "Factories/CameraModeFactory.h" },
		{ "ModuleRelativePath", "Private/Factories/CameraModeFactory.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "Implements a factory for UCameraMode objects." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraModeFactory>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UCameraModeFactory_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UFactory,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCamerasEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraModeFactory_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraModeFactory_Statics::ClassParams = {
	&UCameraModeFactory::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraModeFactory_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraModeFactory_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraModeFactory()
{
	if (!Z_Registration_Info_UClass_UCameraModeFactory.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraModeFactory.OuterSingleton, Z_Construct_UClass_UCameraModeFactory_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraModeFactory.OuterSingleton;
}
template<> GAMEPLAYCAMERASEDITOR_API UClass* StaticClass<UCameraModeFactory>()
{
	return UCameraModeFactory::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraModeFactory);
UCameraModeFactory::~UCameraModeFactory() {}
// End Class UCameraModeFactory

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraModeFactory_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraModeFactory, UCameraModeFactory::StaticClass, TEXT("UCameraModeFactory"), &Z_Registration_Info_UClass_UCameraModeFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraModeFactory), 3050394146U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraModeFactory_h_2936650782(TEXT("/Script/GameplayCamerasEditor"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraModeFactory_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Private_Factories_CameraModeFactory_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
