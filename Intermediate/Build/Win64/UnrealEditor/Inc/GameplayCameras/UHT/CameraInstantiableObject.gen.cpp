// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraInstantiableObject.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraInstantiableObject() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraInstantiableObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraInstantiableObject_NoRegister();
GAMEPLAYCAMERAS_API UEnum* Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Enum ECameraNodeInstantiationState
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ECameraNodeInstantiationState;
static UEnum* ECameraNodeInstantiationState_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ECameraNodeInstantiationState.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ECameraNodeInstantiationState.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ECameraNodeInstantiationState"));
	}
	return Z_Registration_Info_UEnum_ECameraNodeInstantiationState.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ECameraNodeInstantiationState>()
{
	return ECameraNodeInstantiationState_StaticEnum();
}
struct Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Comment", "/**\n *\n */" },
		{ "HasInstantiations.Comment", "/**\n *\n */" },
		{ "HasInstantiations.Name", "ECameraNodeInstantiationState::HasInstantiations" },
		{ "IsInstantiated.Comment", "/**\n *\n */" },
		{ "IsInstantiated.Name", "ECameraNodeInstantiationState::IsInstantiated" },
		{ "ModuleRelativePath", "Public/Core/CameraInstantiableObject.h" },
		{ "None.Comment", "/**\n *\n */" },
		{ "None.Name", "ECameraNodeInstantiationState::None" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ECameraNodeInstantiationState::None", (int64)ECameraNodeInstantiationState::None },
		{ "ECameraNodeInstantiationState::HasInstantiations", (int64)ECameraNodeInstantiationState::HasInstantiations },
		{ "ECameraNodeInstantiationState::IsInstantiated", (int64)ECameraNodeInstantiationState::IsInstantiated },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	"ECameraNodeInstantiationState",
	"ECameraNodeInstantiationState",
	Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState()
{
	if (!Z_Registration_Info_UEnum_ECameraNodeInstantiationState.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ECameraNodeInstantiationState.InnerSingleton, Z_Construct_UEnum_GameplayCameras_ECameraNodeInstantiationState_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ECameraNodeInstantiationState.InnerSingleton;
}
// End Enum ECameraNodeInstantiationState

// Begin Class UCameraInstantiableObject
void UCameraInstantiableObject::StaticRegisterNativesUCameraInstantiableObject()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraInstantiableObject);
UClass* Z_Construct_UClass_UCameraInstantiableObject_NoRegister()
{
	return UCameraInstantiableObject::StaticClass();
}
struct Z_Construct_UClass_UCameraInstantiableObject_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n *\n */" },
		{ "IncludePath", "Core/CameraInstantiableObject.h" },
		{ "ModuleRelativePath", "Public/Core/CameraInstantiableObject.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraInstantiableObject>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UCameraInstantiableObject_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraInstantiableObject_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraInstantiableObject_Statics::ClassParams = {
	&UCameraInstantiableObject::StaticClass,
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
	0x000800A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraInstantiableObject_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraInstantiableObject_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraInstantiableObject()
{
	if (!Z_Registration_Info_UClass_UCameraInstantiableObject.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraInstantiableObject.OuterSingleton, Z_Construct_UClass_UCameraInstantiableObject_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraInstantiableObject.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraInstantiableObject>()
{
	return UCameraInstantiableObject::StaticClass();
}
UCameraInstantiableObject::UCameraInstantiableObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraInstantiableObject);
UCameraInstantiableObject::~UCameraInstantiableObject() {}
// End Class UCameraInstantiableObject

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ ECameraNodeInstantiationState_StaticEnum, TEXT("ECameraNodeInstantiationState"), &Z_Registration_Info_UEnum_ECameraNodeInstantiationState, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2264294728U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraInstantiableObject, UCameraInstantiableObject::StaticClass, TEXT("UCameraInstantiableObject"), &Z_Registration_Info_UClass_UCameraInstantiableObject, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraInstantiableObject), 1284614683U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_3316548164(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraInstantiableObject_h_Statics::EnumInfo));
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
