// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Shakes/PerlinNoiseCameraShakePattern.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePerlinNoiseCameraShakePattern() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UPerlinNoiseCameraShakePattern();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UPerlinNoiseCameraShakePattern_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleCameraShakePattern();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FPerlinNoiseShaker();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin ScriptStruct FPerlinNoiseShaker
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_PerlinNoiseShaker;
class UScriptStruct* FPerlinNoiseShaker::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_PerlinNoiseShaker.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_PerlinNoiseShaker.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FPerlinNoiseShaker, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("PerlinNoiseShaker"));
	}
	return Z_Registration_Info_UScriptStruct_PerlinNoiseShaker.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FPerlinNoiseShaker>()
{
	return FPerlinNoiseShaker::StaticStruct();
}
struct Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Comment", "/** A perlin noise shaker for a single number. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "A perlin noise shaker for a single number." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Amplitude_MetaData[] = {
		{ "Category", "PerlinNoise" },
		{ "Comment", "/** Amplitude of the perlin noise. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Amplitude of the perlin noise." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Frequency_MetaData[] = {
		{ "Category", "PerlinNoise" },
		{ "Comment", "/** Frequency of the sinusoidal oscillation. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Frequency of the sinusoidal oscillation." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Amplitude;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Frequency;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FPerlinNoiseShaker>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::NewProp_Amplitude = { "Amplitude", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPerlinNoiseShaker, Amplitude), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Amplitude_MetaData), NewProp_Amplitude_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::NewProp_Frequency = { "Frequency", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPerlinNoiseShaker, Frequency), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Frequency_MetaData), NewProp_Frequency_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::NewProp_Amplitude,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::NewProp_Frequency,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"PerlinNoiseShaker",
	Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::PropPointers),
	sizeof(FPerlinNoiseShaker),
	alignof(FPerlinNoiseShaker),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FPerlinNoiseShaker()
{
	if (!Z_Registration_Info_UScriptStruct_PerlinNoiseShaker.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_PerlinNoiseShaker.InnerSingleton, Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_PerlinNoiseShaker.InnerSingleton;
}
// End ScriptStruct FPerlinNoiseShaker

// Begin Class UPerlinNoiseCameraShakePattern
void UPerlinNoiseCameraShakePattern::StaticRegisterNativesUPerlinNoiseCameraShakePattern()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UPerlinNoiseCameraShakePattern);
UClass* Z_Construct_UClass_UPerlinNoiseCameraShakePattern_NoRegister()
{
	return UPerlinNoiseCameraShakePattern::StaticClass();
}
struct Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "AutoExpandCategories", "Location,Rotation,FOV,Timing" },
		{ "Comment", "/**\n * A camera shake that uses Perlin noise to shake the camera.\n */" },
		{ "IncludePath", "Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A camera shake that uses Perlin noise to shake the camera." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LocationAmplitudeMultiplier_MetaData[] = {
		{ "Category", "Location" },
		{ "Comment", "/** Amplitude multiplier for all location shake */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Amplitude multiplier for all location shake" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LocationFrequencyMultiplier_MetaData[] = {
		{ "Category", "Location" },
		{ "Comment", "/** Frequency multiplier for all location shake */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Frequency multiplier for all location shake" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_X_MetaData[] = {
		{ "Category", "Location" },
		{ "Comment", "/** Shake in the X axis. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Shake in the X axis." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Y_MetaData[] = {
		{ "Category", "Location" },
		{ "Comment", "/** Shake in the Y axis. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Shake in the Y axis." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Z_MetaData[] = {
		{ "Category", "Location" },
		{ "Comment", "/** Shake in the Z axis. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Shake in the Z axis." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RotationAmplitudeMultiplier_MetaData[] = {
		{ "Category", "Rotation" },
		{ "Comment", "/** Amplitude multiplier for all rotation shake */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Amplitude multiplier for all rotation shake" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RotationFrequencyMultiplier_MetaData[] = {
		{ "Category", "Rotation" },
		{ "Comment", "/** Frequency multiplier for all rotation shake */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Frequency multiplier for all rotation shake" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Pitch_MetaData[] = {
		{ "Category", "Rotation" },
		{ "Comment", "/** Pitch shake. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Pitch shake." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Yaw_MetaData[] = {
		{ "Category", "Rotation" },
		{ "Comment", "/** Yaw shake. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Yaw shake." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Roll_MetaData[] = {
		{ "Category", "Rotation" },
		{ "Comment", "/** Roll shake. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "Roll shake." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FOV_MetaData[] = {
		{ "Category", "FOV" },
		{ "Comment", "/** FOV shake. */" },
		{ "ModuleRelativePath", "Public/Shakes/PerlinNoiseCameraShakePattern.h" },
		{ "ToolTip", "FOV shake." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_LocationAmplitudeMultiplier;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_LocationFrequencyMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_X;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Y;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Z;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_RotationAmplitudeMultiplier;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_RotationFrequencyMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Pitch;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Yaw;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Roll;
	static const UECodeGen_Private::FStructPropertyParams NewProp_FOV;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPerlinNoiseCameraShakePattern>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_LocationAmplitudeMultiplier = { "LocationAmplitudeMultiplier", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, LocationAmplitudeMultiplier), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LocationAmplitudeMultiplier_MetaData), NewProp_LocationAmplitudeMultiplier_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_LocationFrequencyMultiplier = { "LocationFrequencyMultiplier", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, LocationFrequencyMultiplier), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LocationFrequencyMultiplier_MetaData), NewProp_LocationFrequencyMultiplier_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_X = { "X", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, X), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_X_MetaData), NewProp_X_MetaData) }; // 888531711
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Y = { "Y", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, Y), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Y_MetaData), NewProp_Y_MetaData) }; // 888531711
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Z = { "Z", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, Z), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Z_MetaData), NewProp_Z_MetaData) }; // 888531711
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_RotationAmplitudeMultiplier = { "RotationAmplitudeMultiplier", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, RotationAmplitudeMultiplier), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RotationAmplitudeMultiplier_MetaData), NewProp_RotationAmplitudeMultiplier_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_RotationFrequencyMultiplier = { "RotationFrequencyMultiplier", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, RotationFrequencyMultiplier), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RotationFrequencyMultiplier_MetaData), NewProp_RotationFrequencyMultiplier_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Pitch = { "Pitch", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, Pitch), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Pitch_MetaData), NewProp_Pitch_MetaData) }; // 888531711
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Yaw = { "Yaw", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, Yaw), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Yaw_MetaData), NewProp_Yaw_MetaData) }; // 888531711
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Roll = { "Roll", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, Roll), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Roll_MetaData), NewProp_Roll_MetaData) }; // 888531711
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_FOV = { "FOV", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UPerlinNoiseCameraShakePattern, FOV), Z_Construct_UScriptStruct_FPerlinNoiseShaker, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FOV_MetaData), NewProp_FOV_MetaData) }; // 888531711
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_LocationAmplitudeMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_LocationFrequencyMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_X,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Y,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Z,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_RotationAmplitudeMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_RotationFrequencyMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Pitch,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Yaw,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_Roll,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::NewProp_FOV,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USimpleCameraShakePattern,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::ClassParams = {
	&UPerlinNoiseCameraShakePattern::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::PropPointers),
	0,
	0x001010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::Class_MetaDataParams), Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UPerlinNoiseCameraShakePattern()
{
	if (!Z_Registration_Info_UClass_UPerlinNoiseCameraShakePattern.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UPerlinNoiseCameraShakePattern.OuterSingleton, Z_Construct_UClass_UPerlinNoiseCameraShakePattern_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UPerlinNoiseCameraShakePattern.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UPerlinNoiseCameraShakePattern>()
{
	return UPerlinNoiseCameraShakePattern::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UPerlinNoiseCameraShakePattern);
UPerlinNoiseCameraShakePattern::~UPerlinNoiseCameraShakePattern() {}
// End Class UPerlinNoiseCameraShakePattern

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_PerlinNoiseCameraShakePattern_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FPerlinNoiseShaker::StaticStruct, Z_Construct_UScriptStruct_FPerlinNoiseShaker_Statics::NewStructOps, TEXT("PerlinNoiseShaker"), &Z_Registration_Info_UScriptStruct_PerlinNoiseShaker, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FPerlinNoiseShaker), 888531711U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UPerlinNoiseCameraShakePattern, UPerlinNoiseCameraShakePattern::StaticClass, TEXT("UPerlinNoiseCameraShakePattern"), &Z_Registration_Info_UClass_UPerlinNoiseCameraShakePattern, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UPerlinNoiseCameraShakePattern), 4268339101U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_PerlinNoiseCameraShakePattern_h_3745935851(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_PerlinNoiseCameraShakePattern_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_PerlinNoiseCameraShakePattern_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_PerlinNoiseCameraShakePattern_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Shakes_PerlinNoiseCameraShakePattern_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
