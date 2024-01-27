// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Private/Core/BlendStackRootCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBlendStackRootCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendStackRootCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendStackRootCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UBlendStackRootCameraNode
void UBlendStackRootCameraNode::StaticRegisterNativesUBlendStackRootCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UBlendStackRootCameraNode);
UClass* Z_Construct_UClass_UBlendStackRootCameraNode_NoRegister()
{
	return UBlendStackRootCameraNode::StaticClass();
}
struct Z_Construct_UClass_UBlendStackRootCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Root camera node for running a camera mode in a blend stack.\n * This camera node wraps both the camera mode's root node, and the\n * blend node used to blend it.\n */" },
		{ "IncludePath", "Core/BlendStackRootCameraNode.h" },
		{ "ModuleRelativePath", "Private/Core/BlendStackRootCameraNode.h" },
		{ "ToolTip", "Root camera node for running a camera mode in a blend stack.\nThis camera node wraps both the camera mode's root node, and the\nblend node used to blend it." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Blend_MetaData[] = {
		{ "Comment", "/** The blend to use on the camera mode. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Private/Core/BlendStackRootCameraNode.h" },
		{ "ToolTip", "The blend to use on the camera mode." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RootNode_MetaData[] = {
		{ "Comment", "/** The root of the instantied camera node tree. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Private/Core/BlendStackRootCameraNode.h" },
		{ "ToolTip", "The root of the instantied camera node tree." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Blend;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RootNode;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBlendStackRootCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UBlendStackRootCameraNode_Statics::NewProp_Blend = { "Blend", nullptr, (EPropertyFlags)0x0114000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlendStackRootCameraNode, Blend), Z_Construct_UClass_UBlendCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Blend_MetaData), NewProp_Blend_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UBlendStackRootCameraNode_Statics::NewProp_RootNode = { "RootNode", nullptr, (EPropertyFlags)0x0114000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlendStackRootCameraNode, RootNode), Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RootNode_MetaData), NewProp_RootNode_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBlendStackRootCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlendStackRootCameraNode_Statics::NewProp_Blend,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlendStackRootCameraNode_Statics::NewProp_RootNode,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackRootCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UBlendStackRootCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackRootCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBlendStackRootCameraNode_Statics::ClassParams = {
	&UBlendStackRootCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UBlendStackRootCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackRootCameraNode_Statics::PropPointers),
	0,
	0x00A810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackRootCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UBlendStackRootCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UBlendStackRootCameraNode()
{
	if (!Z_Registration_Info_UClass_UBlendStackRootCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBlendStackRootCameraNode.OuterSingleton, Z_Construct_UClass_UBlendStackRootCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBlendStackRootCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UBlendStackRootCameraNode>()
{
	return UBlendStackRootCameraNode::StaticClass();
}
UBlendStackRootCameraNode::UBlendStackRootCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UBlendStackRootCameraNode);
UBlendStackRootCameraNode::~UBlendStackRootCameraNode() {}
// End Class UBlendStackRootCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Core_BlendStackRootCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBlendStackRootCameraNode, UBlendStackRootCameraNode::StaticClass, TEXT("UBlendStackRootCameraNode"), &Z_Registration_Info_UClass_UBlendStackRootCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBlendStackRootCameraNode), 803338623U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Core_BlendStackRootCameraNode_h_144729590(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Core_BlendStackRootCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Private_Core_BlendStackRootCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
