// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Nodes/Blends/PopBlendCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePopBlendCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UPopBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UPopBlendCameraNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UPopBlendCameraNode
void UPopBlendCameraNode::StaticRegisterNativesUPopBlendCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UPopBlendCameraNode);
UClass* Z_Construct_UClass_UPopBlendCameraNode_NoRegister()
{
	return UPopBlendCameraNode::StaticClass();
}
struct Z_Construct_UClass_UPopBlendCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A blend node that creates a camera cut (i.e. it doesn't blend at all).\n */" },
		{ "IncludePath", "Nodes/Blends/PopBlendCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/PopBlendCameraNode.h" },
		{ "ToolTip", "A blend node that creates a camera cut (i.e. it doesn't blend at all)." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPopBlendCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UPopBlendCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlendCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UPopBlendCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UPopBlendCameraNode_Statics::ClassParams = {
	&UPopBlendCameraNode::StaticClass,
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
	0x002810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UPopBlendCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UPopBlendCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UPopBlendCameraNode()
{
	if (!Z_Registration_Info_UClass_UPopBlendCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UPopBlendCameraNode.OuterSingleton, Z_Construct_UClass_UPopBlendCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UPopBlendCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UPopBlendCameraNode>()
{
	return UPopBlendCameraNode::StaticClass();
}
UPopBlendCameraNode::UPopBlendCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UPopBlendCameraNode);
UPopBlendCameraNode::~UPopBlendCameraNode() {}
// End Class UPopBlendCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_PopBlendCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UPopBlendCameraNode, UPopBlendCameraNode::StaticClass, TEXT("UPopBlendCameraNode"), &Z_Registration_Info_UClass_UPopBlendCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UPopBlendCameraNode), 2970132163U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_PopBlendCameraNode_h_338679885(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_PopBlendCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_PopBlendCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
