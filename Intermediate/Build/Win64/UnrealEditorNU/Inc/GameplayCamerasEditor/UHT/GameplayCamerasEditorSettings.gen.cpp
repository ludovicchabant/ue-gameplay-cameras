// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCamerasEditor/Public/GameplayCamerasEditorSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCamerasEditorSettings() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UGameplayCamerasEditorSettings();
GAMEPLAYCAMERASEDITOR_API UClass* Z_Construct_UClass_UGameplayCamerasEditorSettings_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCamerasEditor();
// End Cross Module References

// Begin Class UGameplayCamerasEditorSettings
void UGameplayCamerasEditorSettings::StaticRegisterNativesUGameplayCamerasEditorSettings()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGameplayCamerasEditorSettings);
UClass* Z_Construct_UClass_UGameplayCamerasEditorSettings_NoRegister()
{
	return UGameplayCamerasEditorSettings::StaticClass();
}
struct Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayCamerasEditorSettings.h" },
		{ "ModuleRelativePath", "Public/GameplayCamerasEditorSettings.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCamerasEditorSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCamerasEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics::ClassParams = {
	&UGameplayCamerasEditorSettings::StaticClass,
	"EditorPerProjectUserSettings",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGameplayCamerasEditorSettings()
{
	if (!Z_Registration_Info_UClass_UGameplayCamerasEditorSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGameplayCamerasEditorSettings.OuterSingleton, Z_Construct_UClass_UGameplayCamerasEditorSettings_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGameplayCamerasEditorSettings.OuterSingleton;
}
template<> GAMEPLAYCAMERASEDITOR_API UClass* StaticClass<UGameplayCamerasEditorSettings>()
{
	return UGameplayCamerasEditorSettings::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCamerasEditorSettings);
UGameplayCamerasEditorSettings::~UGameplayCamerasEditorSettings() {}
// End Class UGameplayCamerasEditorSettings

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGameplayCamerasEditorSettings, UGameplayCamerasEditorSettings::StaticClass, TEXT("UGameplayCamerasEditorSettings"), &Z_Registration_Info_UClass_UGameplayCamerasEditorSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGameplayCamerasEditorSettings), 3129700956U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_1376036177(TEXT("/Script/GameplayCamerasEditor"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCamerasEditor_Public_GameplayCamerasEditorSettings_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
