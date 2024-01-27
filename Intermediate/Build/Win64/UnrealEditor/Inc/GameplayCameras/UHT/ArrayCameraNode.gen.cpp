// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Nodes/Common/ArrayCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeArrayCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UArrayCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UArrayCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UArrayCameraNode
void UArrayCameraNode::StaticRegisterNativesUArrayCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UArrayCameraNode);
UClass* Z_Construct_UClass_UArrayCameraNode_NoRegister()
{
	return UArrayCameraNode::StaticClass();
}
struct Z_Construct_UClass_UArrayCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A camera node that runs a list of other camera nodes.\n */" },
		{ "IncludePath", "Nodes/Common/ArrayCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Common/ArrayCameraNode.h" },
		{ "ToolTip", "A camera node that runs a list of other camera nodes." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Children_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** The camera nodes to run. */" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Nodes/Common/ArrayCameraNode.h" },
		{ "ToolTip", "The camera nodes to run." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Children_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Children;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UArrayCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UArrayCameraNode_Statics::NewProp_Children_Inner = { "Children", nullptr, (EPropertyFlags)0x0104000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UCameraNode_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UArrayCameraNode_Statics::NewProp_Children = { "Children", nullptr, (EPropertyFlags)0x0114008000000009, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UArrayCameraNode, Children), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Children_MetaData), NewProp_Children_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UArrayCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UArrayCameraNode_Statics::NewProp_Children_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UArrayCameraNode_Statics::NewProp_Children,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UArrayCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UArrayCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UArrayCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UArrayCameraNode_Statics::ClassParams = {
	&UArrayCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UArrayCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UArrayCameraNode_Statics::PropPointers),
	0,
	0x00A810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UArrayCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UArrayCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UArrayCameraNode()
{
	if (!Z_Registration_Info_UClass_UArrayCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UArrayCameraNode.OuterSingleton, Z_Construct_UClass_UArrayCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UArrayCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UArrayCameraNode>()
{
	return UArrayCameraNode::StaticClass();
}
UArrayCameraNode::UArrayCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UArrayCameraNode);
UArrayCameraNode::~UArrayCameraNode() {}
// End Class UArrayCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_ArrayCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UArrayCameraNode, UArrayCameraNode::StaticClass, TEXT("UArrayCameraNode"), &Z_Registration_Info_UClass_UArrayCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UArrayCameraNode), 3604829876U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_ArrayCameraNode_h_1359151403(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_ArrayCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_ArrayCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
