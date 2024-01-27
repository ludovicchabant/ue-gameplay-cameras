// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/DefaultRootCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDefaultRootCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UDefaultRootCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UDefaultRootCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_URootCameraNode();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UDefaultRootCameraNode
void UDefaultRootCameraNode::StaticRegisterNativesUDefaultRootCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UDefaultRootCameraNode);
UClass* Z_Construct_UClass_UDefaultRootCameraNode_NoRegister()
{
	return UDefaultRootCameraNode::StaticClass();
}
struct Z_Construct_UClass_UDefaultRootCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * The default implementation of a root camera node.\n */" },
		{ "IncludePath", "Core/DefaultRootCameraNode.h" },
		{ "ModuleRelativePath", "Public/Core/DefaultRootCameraNode.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "The default implementation of a root camera node." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseLayer_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/DefaultRootCameraNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MainLayer_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/DefaultRootCameraNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GlobalLayer_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/DefaultRootCameraNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_VisualLayer_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Core/DefaultRootCameraNode.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BaseLayer;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MainLayer;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_GlobalLayer;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_VisualLayer;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDefaultRootCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_BaseLayer = { "BaseLayer", nullptr, (EPropertyFlags)0x0146000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDefaultRootCameraNode, BaseLayer), Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseLayer_MetaData), NewProp_BaseLayer_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_MainLayer = { "MainLayer", nullptr, (EPropertyFlags)0x0146000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDefaultRootCameraNode, MainLayer), Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MainLayer_MetaData), NewProp_MainLayer_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_GlobalLayer = { "GlobalLayer", nullptr, (EPropertyFlags)0x0146000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDefaultRootCameraNode, GlobalLayer), Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GlobalLayer_MetaData), NewProp_GlobalLayer_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_VisualLayer = { "VisualLayer", nullptr, (EPropertyFlags)0x0146000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDefaultRootCameraNode, VisualLayer), Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_VisualLayer_MetaData), NewProp_VisualLayer_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UDefaultRootCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_BaseLayer,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_MainLayer,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_GlobalLayer,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDefaultRootCameraNode_Statics::NewProp_VisualLayer,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDefaultRootCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UDefaultRootCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_URootCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDefaultRootCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UDefaultRootCameraNode_Statics::ClassParams = {
	&UDefaultRootCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UDefaultRootCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UDefaultRootCameraNode_Statics::PropPointers),
	0,
	0x00A810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDefaultRootCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UDefaultRootCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UDefaultRootCameraNode()
{
	if (!Z_Registration_Info_UClass_UDefaultRootCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDefaultRootCameraNode.OuterSingleton, Z_Construct_UClass_UDefaultRootCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UDefaultRootCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UDefaultRootCameraNode>()
{
	return UDefaultRootCameraNode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UDefaultRootCameraNode);
UDefaultRootCameraNode::~UDefaultRootCameraNode() {}
// End Class UDefaultRootCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_DefaultRootCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UDefaultRootCameraNode, UDefaultRootCameraNode::StaticClass, TEXT("UDefaultRootCameraNode"), &Z_Registration_Info_UClass_UDefaultRootCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDefaultRootCameraNode), 759901550U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_DefaultRootCameraNode_h_1065436909(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_DefaultRootCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_DefaultRootCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
