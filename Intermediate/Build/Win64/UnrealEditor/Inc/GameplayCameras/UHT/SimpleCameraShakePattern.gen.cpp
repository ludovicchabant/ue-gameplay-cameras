// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Shakes/SimpleCameraShakePattern.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSimpleCameraShakePattern() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_UCameraShakePattern();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleCameraShakePattern();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleCameraShakePattern_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class USimpleCameraShakePattern
void USimpleCameraShakePattern::StaticRegisterNativesUSimpleCameraShakePattern()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USimpleCameraShakePattern);
UClass* Z_Construct_UClass_USimpleCameraShakePattern_NoRegister()
{
	return USimpleCameraShakePattern::StaticClass();
}
struct Z_Construct_UClass_USimpleCameraShakePattern_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A base class for a simple camera shake.\n */" },
		{ "IncludePath", "Shakes/SimpleCameraShakePattern.h" },
		{ "ModuleRelativePath", "Public/Shakes/SimpleCameraShakePattern.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A base class for a simple camera shake." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Duration_MetaData[] = {
		{ "Category", "Timing" },
		{ "Comment", "/** Duration in seconds of this shake. Zero or less means infinite. */" },
		{ "ModuleRelativePath", "Public/Shakes/SimpleCameraShakePattern.h" },
		{ "ToolTip", "Duration in seconds of this shake. Zero or less means infinite." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlendInTime_MetaData[] = {
		{ "Category", "Timing" },
		{ "Comment", "/** Blend-in time for this shake. Zero or less means no blend-in. */" },
		{ "ModuleRelativePath", "Public/Shakes/SimpleCameraShakePattern.h" },
		{ "ToolTip", "Blend-in time for this shake. Zero or less means no blend-in." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlendOutTime_MetaData[] = {
		{ "Category", "Timing" },
		{ "Comment", "/** Blend-out time for this shake. Zero or less means no blend-out. */" },
		{ "ModuleRelativePath", "Public/Shakes/SimpleCameraShakePattern.h" },
		{ "ToolTip", "Blend-out time for this shake. Zero or less means no blend-out." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Duration;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BlendInTime;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BlendOutTime;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USimpleCameraShakePattern>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USimpleCameraShakePattern_Statics::NewProp_Duration = { "Duration", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USimpleCameraShakePattern, Duration), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Duration_MetaData), NewProp_Duration_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USimpleCameraShakePattern_Statics::NewProp_BlendInTime = { "BlendInTime", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USimpleCameraShakePattern, BlendInTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlendInTime_MetaData), NewProp_BlendInTime_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USimpleCameraShakePattern_Statics::NewProp_BlendOutTime = { "BlendOutTime", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USimpleCameraShakePattern, BlendOutTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlendOutTime_MetaData), NewProp_BlendOutTime_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USimpleCameraShakePattern_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USimpleCameraShakePattern_Statics::NewProp_Duration,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USimpleCameraShakePattern_Statics::NewProp_BlendInTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USimpleCameraShakePattern_Statics::NewProp_BlendOutTime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleCameraShakePattern_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_USimpleCameraShakePattern_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraShakePattern,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleCameraShakePattern_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USimpleCameraShakePattern_Statics::ClassParams = {
	&USimpleCameraShakePattern::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_USimpleCameraShakePattern_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_USimpleCameraShakePattern_Statics::PropPointers),
	0,
	0x001010A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleCameraShakePattern_Statics::Class_MetaDataParams), Z_Construct_UClass_USimpleCameraShakePattern_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USimpleCameraShakePattern()
{
	if (!Z_Registration_Info_UClass_USimpleCameraShakePattern.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USimpleCameraShakePattern.OuterSingleton, Z_Construct_UClass_USimpleCameraShakePattern_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USimpleCameraShakePattern.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<USimpleCameraShakePattern>()
{
	return USimpleCameraShakePattern::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(USimpleCameraShakePattern);
USimpleCameraShakePattern::~USimpleCameraShakePattern() {}
// End Class USimpleCameraShakePattern

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_SimpleCameraShakePattern_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USimpleCameraShakePattern, USimpleCameraShakePattern::StaticClass, TEXT("USimpleCameraShakePattern"), &Z_Registration_Info_UClass_USimpleCameraShakePattern, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USimpleCameraShakePattern), 1500437101U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_SimpleCameraShakePattern_h_1455781063(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_SimpleCameraShakePattern_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_SimpleCameraShakePattern_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
