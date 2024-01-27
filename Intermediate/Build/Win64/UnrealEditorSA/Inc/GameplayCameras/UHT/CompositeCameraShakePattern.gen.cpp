// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/CompositeCameraShakePattern.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCompositeCameraShakePattern() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_UCameraShakePattern();
ENGINE_API UClass* Z_Construct_UClass_UCameraShakePattern_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCompositeCameraShakePattern();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCompositeCameraShakePattern_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UCompositeCameraShakePattern
void UCompositeCameraShakePattern::StaticRegisterNativesUCompositeCameraShakePattern()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCompositeCameraShakePattern);
UClass* Z_Construct_UClass_UCompositeCameraShakePattern_NoRegister()
{
	return UCompositeCameraShakePattern::StaticClass();
}
struct Z_Construct_UClass_UCompositeCameraShakePattern_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "AutoExpandCategories", "CameraShake" },
		{ "Comment", "/**\n * A base class for a simple camera shake.\n */" },
		{ "IncludePath", "CompositeCameraShakePattern.h" },
		{ "ModuleRelativePath", "Public/CompositeCameraShakePattern.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A base class for a simple camera shake." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ChildPatterns_Inner_MetaData[] = {
		{ "Category", "CameraShake" },
		{ "Comment", "/** The list of child shake patterns */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/CompositeCameraShakePattern.h" },
		{ "ToolTip", "The list of child shake patterns" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ChildPatterns_MetaData[] = {
		{ "Category", "CameraShake" },
		{ "Comment", "/** The list of child shake patterns */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/CompositeCameraShakePattern.h" },
		{ "ToolTip", "The list of child shake patterns" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPtrPropertyParams NewProp_ChildPatterns_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ChildPatterns;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCompositeCameraShakePattern>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPtrPropertyParams Z_Construct_UClass_UCompositeCameraShakePattern_Statics::NewProp_ChildPatterns_Inner = { "ChildPatterns", nullptr, (EPropertyFlags)0x0006000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UCameraShakePattern_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ChildPatterns_Inner_MetaData), NewProp_ChildPatterns_Inner_MetaData) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UCompositeCameraShakePattern_Statics::NewProp_ChildPatterns = { "ChildPatterns", nullptr, (EPropertyFlags)0x0014008000000009, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCompositeCameraShakePattern, ChildPatterns), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ChildPatterns_MetaData), NewProp_ChildPatterns_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCompositeCameraShakePattern_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCompositeCameraShakePattern_Statics::NewProp_ChildPatterns_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCompositeCameraShakePattern_Statics::NewProp_ChildPatterns,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCompositeCameraShakePattern_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCompositeCameraShakePattern_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraShakePattern,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCompositeCameraShakePattern_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCompositeCameraShakePattern_Statics::ClassParams = {
	&UCompositeCameraShakePattern::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCompositeCameraShakePattern_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCompositeCameraShakePattern_Statics::PropPointers),
	0,
	0x009010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCompositeCameraShakePattern_Statics::Class_MetaDataParams), Z_Construct_UClass_UCompositeCameraShakePattern_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCompositeCameraShakePattern()
{
	if (!Z_Registration_Info_UClass_UCompositeCameraShakePattern.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCompositeCameraShakePattern.OuterSingleton, Z_Construct_UClass_UCompositeCameraShakePattern_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCompositeCameraShakePattern.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCompositeCameraShakePattern>()
{
	return UCompositeCameraShakePattern::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCompositeCameraShakePattern);
UCompositeCameraShakePattern::~UCompositeCameraShakePattern() {}
// End Class UCompositeCameraShakePattern

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CompositeCameraShakePattern_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCompositeCameraShakePattern, UCompositeCameraShakePattern::StaticClass, TEXT("UCompositeCameraShakePattern"), &Z_Registration_Info_UClass_UCompositeCameraShakePattern, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCompositeCameraShakePattern), 195915986U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CompositeCameraShakePattern_h_4207712149(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CompositeCameraShakePattern_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CompositeCameraShakePattern_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
