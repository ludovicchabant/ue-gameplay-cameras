// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Nodes/Blends/LinearBlendCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeLinearBlendCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_ULinearBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_ULinearBlendCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleFixedTimeBlendCameraNode();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class ULinearBlendCameraNode
void ULinearBlendCameraNode::StaticRegisterNativesULinearBlendCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ULinearBlendCameraNode);
UClass* Z_Construct_UClass_ULinearBlendCameraNode_NoRegister()
{
	return ULinearBlendCameraNode::StaticClass();
}
struct Z_Construct_UClass_ULinearBlendCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Linear blend node.\n */" },
		{ "IncludePath", "Nodes/Blends/LinearBlendCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/LinearBlendCameraNode.h" },
		{ "ToolTip", "Linear blend node." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ULinearBlendCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ULinearBlendCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USimpleFixedTimeBlendCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ULinearBlendCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ULinearBlendCameraNode_Statics::ClassParams = {
	&ULinearBlendCameraNode::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ULinearBlendCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_ULinearBlendCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ULinearBlendCameraNode()
{
	if (!Z_Registration_Info_UClass_ULinearBlendCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ULinearBlendCameraNode.OuterSingleton, Z_Construct_UClass_ULinearBlendCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ULinearBlendCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<ULinearBlendCameraNode>()
{
	return ULinearBlendCameraNode::StaticClass();
}
ULinearBlendCameraNode::ULinearBlendCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(ULinearBlendCameraNode);
ULinearBlendCameraNode::~ULinearBlendCameraNode() {}
// End Class ULinearBlendCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ULinearBlendCameraNode, ULinearBlendCameraNode::StaticClass, TEXT("ULinearBlendCameraNode"), &Z_Registration_Info_UClass_ULinearBlendCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ULinearBlendCameraNode), 2114480345U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_997474197(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_LinearBlendCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
