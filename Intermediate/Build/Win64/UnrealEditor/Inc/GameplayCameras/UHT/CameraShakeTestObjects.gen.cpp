// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Private/Tests/CameraShakeTestObjects.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraShakeTestObjects() {}

// Begin Cross Module References
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FRotator();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
ENGINE_API UClass* Z_Construct_UClass_UCameraShakeBase();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UConstantCameraShakePattern();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UConstantCameraShakePattern_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleCameraShakePattern();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UTestCameraShake();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UTestCameraShake_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UTestCameraShake
void UTestCameraShake::StaticRegisterNativesUTestCameraShake()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTestCameraShake);
UClass* Z_Construct_UClass_UTestCameraShake_NoRegister()
{
	return UTestCameraShake::StaticClass();
}
struct Z_Construct_UClass_UTestCameraShake_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "Tests/CameraShakeTestObjects.h" },
		{ "IsBlueprintBase", "false" },
		{ "ModuleRelativePath", "Private/Tests/CameraShakeTestObjects.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTestCameraShake>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTestCameraShake_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraShakeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTestCameraShake_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTestCameraShake_Statics::ClassParams = {
	&UTestCameraShake::StaticClass,
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
	0x048010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTestCameraShake_Statics::Class_MetaDataParams), Z_Construct_UClass_UTestCameraShake_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTestCameraShake()
{
	if (!Z_Registration_Info_UClass_UTestCameraShake.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTestCameraShake.OuterSingleton, Z_Construct_UClass_UTestCameraShake_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTestCameraShake.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UTestCameraShake>()
{
	return UTestCameraShake::StaticClass();
}
UTestCameraShake::UTestCameraShake(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTestCameraShake);
UTestCameraShake::~UTestCameraShake() {}
// End Class UTestCameraShake

// Begin Class UConstantCameraShakePattern
void UConstantCameraShakePattern::StaticRegisterNativesUConstantCameraShakePattern()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UConstantCameraShakePattern);
UClass* Z_Construct_UClass_UConstantCameraShakePattern_NoRegister()
{
	return UConstantCameraShakePattern::StaticClass();
}
struct Z_Construct_UClass_UConstantCameraShakePattern_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "Tests/CameraShakeTestObjects.h" },
		{ "IsBlueprintBase", "false" },
		{ "ModuleRelativePath", "Private/Tests/CameraShakeTestObjects.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LocationOffset_MetaData[] = {
		{ "ModuleRelativePath", "Private/Tests/CameraShakeTestObjects.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RotationOffset_MetaData[] = {
		{ "ModuleRelativePath", "Private/Tests/CameraShakeTestObjects.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_LocationOffset;
	static const UECodeGen_Private::FStructPropertyParams NewProp_RotationOffset;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UConstantCameraShakePattern>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UConstantCameraShakePattern_Statics::NewProp_LocationOffset = { "LocationOffset", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UConstantCameraShakePattern, LocationOffset), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LocationOffset_MetaData), NewProp_LocationOffset_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UConstantCameraShakePattern_Statics::NewProp_RotationOffset = { "RotationOffset", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UConstantCameraShakePattern, RotationOffset), Z_Construct_UScriptStruct_FRotator, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RotationOffset_MetaData), NewProp_RotationOffset_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UConstantCameraShakePattern_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UConstantCameraShakePattern_Statics::NewProp_LocationOffset,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UConstantCameraShakePattern_Statics::NewProp_RotationOffset,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UConstantCameraShakePattern_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UConstantCameraShakePattern_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USimpleCameraShakePattern,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UConstantCameraShakePattern_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UConstantCameraShakePattern_Statics::ClassParams = {
	&UConstantCameraShakePattern::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UConstantCameraShakePattern_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UConstantCameraShakePattern_Statics::PropPointers),
	0,
	0x040010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UConstantCameraShakePattern_Statics::Class_MetaDataParams), Z_Construct_UClass_UConstantCameraShakePattern_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UConstantCameraShakePattern()
{
	if (!Z_Registration_Info_UClass_UConstantCameraShakePattern.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UConstantCameraShakePattern.OuterSingleton, Z_Construct_UClass_UConstantCameraShakePattern_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UConstantCameraShakePattern.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UConstantCameraShakePattern>()
{
	return UConstantCameraShakePattern::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UConstantCameraShakePattern);
UConstantCameraShakePattern::~UConstantCameraShakePattern() {}
// End Class UConstantCameraShakePattern

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Tests_CameraShakeTestObjects_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTestCameraShake, UTestCameraShake::StaticClass, TEXT("UTestCameraShake"), &Z_Registration_Info_UClass_UTestCameraShake, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTestCameraShake), 2102652702U) },
		{ Z_Construct_UClass_UConstantCameraShakePattern, UConstantCameraShakePattern::StaticClass, TEXT("UConstantCameraShakePattern"), &Z_Registration_Info_UClass_UConstantCameraShakePattern, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UConstantCameraShakePattern), 2730554581U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Tests_CameraShakeTestObjects_h_1920759671(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Tests_CameraShakeTestObjects_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Tests_CameraShakeTestObjects_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
