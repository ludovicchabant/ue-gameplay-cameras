// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraInstantiableObject();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UEnum* Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Enum ECameraNodeFlags
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ECameraNodeFlags;
static UEnum* ECameraNodeFlags_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ECameraNodeFlags.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ECameraNodeFlags.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ECameraNodeFlags"));
	}
	return Z_Registration_Info_UEnum_ECameraNodeFlags.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ECameraNodeFlags>()
{
	return ECameraNodeFlags_StaticEnum();
}
struct Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Core/CameraNode.h" },
		{ "None.Name", "ECameraNodeFlags::None" },
		{ "RequiresReset.Name", "ECameraNodeFlags::RequiresReset" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ECameraNodeFlags::None", (int64)ECameraNodeFlags::None },
		{ "ECameraNodeFlags::RequiresReset", (int64)ECameraNodeFlags::RequiresReset },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	"ECameraNodeFlags",
	"ECameraNodeFlags",
	Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags()
{
	if (!Z_Registration_Info_UEnum_ECameraNodeFlags.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ECameraNodeFlags.InnerSingleton, Z_Construct_UEnum_GameplayCameras_ECameraNodeFlags_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ECameraNodeFlags.InnerSingleton;
}
// End Enum ECameraNodeFlags

// Begin Class UCameraNode
void UCameraNode::StaticRegisterNativesUCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraNode);
UClass* Z_Construct_UClass_UCameraNode_NoRegister()
{
	return UCameraNode::StaticClass();
}
struct Z_Construct_UClass_UCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * The base class for a camera node.\n */" },
		{ "IncludePath", "Core/CameraNode.h" },
		{ "ModuleRelativePath", "Public/Core/CameraNode.h" },
		{ "ToolTip", "The base class for a camera node." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsEnabled_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** Specifies whether this node is enabled. */" },
		{ "ModuleRelativePath", "Public/Core/CameraNode.h" },
		{ "ToolTip", "Specifies whether this node is enabled." },
	};
#endif // WITH_METADATA
	static void NewProp_bIsEnabled_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsEnabled;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
void Z_Construct_UClass_UCameraNode_Statics::NewProp_bIsEnabled_SetBit(void* Obj)
{
	((UCameraNode*)Obj)->bIsEnabled = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UCameraNode_Statics::NewProp_bIsEnabled = { "bIsEnabled", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UCameraNode), &Z_Construct_UClass_UCameraNode_Statics::NewProp_bIsEnabled_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsEnabled_MetaData), NewProp_bIsEnabled_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraNode_Statics::NewProp_bIsEnabled,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraInstantiableObject,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraNode_Statics::ClassParams = {
	&UCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCameraNode_Statics::PropPointers),
	0,
	0x002810A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraNode()
{
	if (!Z_Registration_Info_UClass_UCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraNode.OuterSingleton, Z_Construct_UClass_UCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraNode>()
{
	return UCameraNode::StaticClass();
}
UCameraNode::UCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraNode);
UCameraNode::~UCameraNode() {}
// End Class UCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraNode_h_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ ECameraNodeFlags_StaticEnum, TEXT("ECameraNodeFlags"), &Z_Registration_Info_UEnum_ECameraNodeFlags, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3621236445U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraNode, UCameraNode::StaticClass, TEXT("UCameraNode"), &Z_Registration_Info_UClass_UCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraNode), 2258945525U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraNode_h_3668443314(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraNode_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraNode_h_Statics::EnumInfo));
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
