// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Math/PerlinNoise.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePerlinNoise() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FMultiOctavePerlinNoise();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FPerlinNoise();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin ScriptStruct FPerlinNoise
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_PerlinNoise;
class UScriptStruct* FPerlinNoise::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_PerlinNoise.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_PerlinNoise.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FPerlinNoise, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("PerlinNoise"));
	}
	return Z_Registration_Info_UScriptStruct_PerlinNoise.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FPerlinNoise>()
{
	return FPerlinNoise::StaticStruct();
}
struct Z_Construct_UScriptStruct_FPerlinNoise_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SamplePeriod_MetaData[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CurrentTime_MetaData[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CurrentSample_MetaData[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LastSample_MetaData[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SamplePeriod;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_CurrentTime;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_CurrentSample;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_LastSample;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FPerlinNoise>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_SamplePeriod = { "SamplePeriod", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPerlinNoise, SamplePeriod), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SamplePeriod_MetaData), NewProp_SamplePeriod_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_CurrentTime = { "CurrentTime", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPerlinNoise, CurrentTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CurrentTime_MetaData), NewProp_CurrentTime_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_CurrentSample = { "CurrentSample", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPerlinNoise, CurrentSample), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CurrentSample_MetaData), NewProp_CurrentSample_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_LastSample = { "LastSample", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPerlinNoise, LastSample), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LastSample_MetaData), NewProp_LastSample_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FPerlinNoise_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_SamplePeriod,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_CurrentTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_CurrentSample,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewProp_LastSample,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPerlinNoise_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FPerlinNoise_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"PerlinNoise",
	Z_Construct_UScriptStruct_FPerlinNoise_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPerlinNoise_Statics::PropPointers),
	sizeof(FPerlinNoise),
	alignof(FPerlinNoise),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPerlinNoise_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FPerlinNoise_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FPerlinNoise()
{
	if (!Z_Registration_Info_UScriptStruct_PerlinNoise.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_PerlinNoise.InnerSingleton, Z_Construct_UScriptStruct_FPerlinNoise_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_PerlinNoise.InnerSingleton;
}
// End ScriptStruct FPerlinNoise

// Begin ScriptStruct FMultiOctavePerlinNoise
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise;
class UScriptStruct* FMultiOctavePerlinNoise::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FMultiOctavePerlinNoise, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("MultiOctavePerlinNoise"));
	}
	return Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FMultiOctavePerlinNoise>()
{
	return FMultiOctavePerlinNoise::StaticStruct();
}
struct Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseSampleFrequency_MetaData[] = {
		{ "ModuleRelativePath", "Public/Math/PerlinNoise.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseSampleFrequency;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FMultiOctavePerlinNoise>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::NewProp_BaseSampleFrequency = { "BaseSampleFrequency", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FMultiOctavePerlinNoise, BaseSampleFrequency), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseSampleFrequency_MetaData), NewProp_BaseSampleFrequency_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::NewProp_BaseSampleFrequency,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"MultiOctavePerlinNoise",
	Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::PropPointers),
	sizeof(FMultiOctavePerlinNoise),
	alignof(FMultiOctavePerlinNoise),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FMultiOctavePerlinNoise()
{
	if (!Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise.InnerSingleton, Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise.InnerSingleton;
}
// End ScriptStruct FMultiOctavePerlinNoise

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Math_PerlinNoise_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FPerlinNoise::StaticStruct, Z_Construct_UScriptStruct_FPerlinNoise_Statics::NewStructOps, TEXT("PerlinNoise"), &Z_Registration_Info_UScriptStruct_PerlinNoise, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FPerlinNoise), 2397151312U) },
		{ FMultiOctavePerlinNoise::StaticStruct, Z_Construct_UScriptStruct_FMultiOctavePerlinNoise_Statics::NewStructOps, TEXT("MultiOctavePerlinNoise"), &Z_Registration_Info_UScriptStruct_MultiOctavePerlinNoise, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FMultiOctavePerlinNoise), 3305520255U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Math_PerlinNoise_h_3877629000(TEXT("/Script/GameplayCameras"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Math_PerlinNoise_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Math_PerlinNoise_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
