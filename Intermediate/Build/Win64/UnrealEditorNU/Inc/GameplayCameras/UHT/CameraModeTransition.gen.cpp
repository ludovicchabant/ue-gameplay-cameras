// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraModeTransition.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraModeTransition() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraModeTransitionCondition();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraModeTransitionCondition_NoRegister();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraModeTransition();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UCameraModeTransitionCondition
void UCameraModeTransitionCondition::StaticRegisterNativesUCameraModeTransitionCondition()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraModeTransitionCondition);
UClass* Z_Construct_UClass_UCameraModeTransitionCondition_NoRegister()
{
	return UCameraModeTransitionCondition::StaticClass();
}
struct Z_Construct_UClass_UCameraModeTransitionCondition_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Base class for a camera transition condition.\n */" },
		{ "IncludePath", "Core/CameraModeTransition.h" },
		{ "ModuleRelativePath", "Public/Core/CameraModeTransition.h" },
		{ "ToolTip", "Base class for a camera transition condition." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraModeTransitionCondition>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UCameraModeTransitionCondition_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraModeTransitionCondition_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraModeTransitionCondition_Statics::ClassParams = {
	&UCameraModeTransitionCondition::StaticClass,
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
	0x002800A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraModeTransitionCondition_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraModeTransitionCondition_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraModeTransitionCondition()
{
	if (!Z_Registration_Info_UClass_UCameraModeTransitionCondition.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraModeTransitionCondition.OuterSingleton, Z_Construct_UClass_UCameraModeTransitionCondition_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraModeTransitionCondition.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraModeTransitionCondition>()
{
	return UCameraModeTransitionCondition::StaticClass();
}
UCameraModeTransitionCondition::UCameraModeTransitionCondition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraModeTransitionCondition);
UCameraModeTransitionCondition::~UCameraModeTransitionCondition() {}
// End Class UCameraModeTransitionCondition

// Begin ScriptStruct FCameraModeTransition
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_CameraModeTransition;
class UScriptStruct* FCameraModeTransition::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_CameraModeTransition.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_CameraModeTransition.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FCameraModeTransition, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("CameraModeTransition"));
	}
	return Z_Registration_Info_UScriptStruct_CameraModeTransition.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FCameraModeTransition>()
{
	return FCameraModeTransition::StaticStruct();
}
struct Z_Construct_UScriptStruct_FCameraModeTransition_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "Comment", "/**\n * A camera transition.\n */" },
		{ "ModuleRelativePath", "Public/Core/CameraModeTransition.h" },
		{ "ToolTip", "A camera transition." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Conditions_Inner_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** The list of conditions that must pass for this transition to be used. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/CameraModeTransition.h" },
		{ "ToolTip", "The list of conditions that must pass for this transition to be used." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Conditions_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** The list of conditions that must pass for this transition to be used. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/CameraModeTransition.h" },
		{ "ToolTip", "The list of conditions that must pass for this transition to be used." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Blend_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** The blend to use to blend a given camera mode in or out. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/CameraModeTransition.h" },
		{ "ToolTip", "The blend to use to blend a given camera mode in or out." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Conditions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Conditions;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Blend;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FCameraModeTransition>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewProp_Conditions_Inner = { "Conditions", nullptr, (EPropertyFlags)0x0106000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UCameraModeTransitionCondition_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Conditions_Inner_MetaData), NewProp_Conditions_Inner_MetaData) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewProp_Conditions = { "Conditions", nullptr, (EPropertyFlags)0x0114008000000009, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraModeTransition, Conditions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Conditions_MetaData), NewProp_Conditions_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewProp_Blend = { "Blend", nullptr, (EPropertyFlags)0x0116000000080009, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraModeTransition, Blend), Z_Construct_UClass_UBlendCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Blend_MetaData), NewProp_Blend_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FCameraModeTransition_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewProp_Conditions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewProp_Conditions,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewProp_Blend,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraModeTransition_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FCameraModeTransition_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"CameraModeTransition",
	Z_Construct_UScriptStruct_FCameraModeTransition_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraModeTransition_Statics::PropPointers),
	sizeof(FCameraModeTransition),
	alignof(FCameraModeTransition),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000005),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraModeTransition_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FCameraModeTransition_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FCameraModeTransition()
{
	if (!Z_Registration_Info_UScriptStruct_CameraModeTransition.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_CameraModeTransition.InnerSingleton, Z_Construct_UScriptStruct_FCameraModeTransition_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_CameraModeTransition.InnerSingleton;
}
// End ScriptStruct FCameraModeTransition

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraModeTransition_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FCameraModeTransition::StaticStruct, Z_Construct_UScriptStruct_FCameraModeTransition_Statics::NewStructOps, TEXT("CameraModeTransition"), &Z_Registration_Info_UScriptStruct_CameraModeTransition, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FCameraModeTransition), 109385190U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraModeTransitionCondition, UCameraModeTransitionCondition::StaticClass, TEXT("UCameraModeTransitionCondition"), &Z_Registration_Info_UClass_UCameraModeTransitionCondition, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraModeTransitionCondition), 1273618551U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraModeTransition_h_2479901075(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraModeTransition_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraModeTransition_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraModeTransition_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraModeTransition_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
