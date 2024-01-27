// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Nodes/Blends/SimpleBlendCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSimpleBlendCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleBlendCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleFixedTimeBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class USimpleBlendCameraNode
void USimpleBlendCameraNode::StaticRegisterNativesUSimpleBlendCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USimpleBlendCameraNode);
UClass* Z_Construct_UClass_USimpleBlendCameraNode_NoRegister()
{
	return USimpleBlendCameraNode::StaticClass();
}
struct Z_Construct_UClass_USimpleBlendCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Base class for a blend camera node that uses a simple scalar factor.\n */" },
		{ "IncludePath", "Nodes/Blends/SimpleBlendCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/SimpleBlendCameraNode.h" },
		{ "ToolTip", "Base class for a blend camera node that uses a simple scalar factor." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USimpleBlendCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_USimpleBlendCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlendCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleBlendCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USimpleBlendCameraNode_Statics::ClassParams = {
	&USimpleBlendCameraNode::StaticClass,
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
	0x002810A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleBlendCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_USimpleBlendCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USimpleBlendCameraNode()
{
	if (!Z_Registration_Info_UClass_USimpleBlendCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USimpleBlendCameraNode.OuterSingleton, Z_Construct_UClass_USimpleBlendCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USimpleBlendCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<USimpleBlendCameraNode>()
{
	return USimpleBlendCameraNode::StaticClass();
}
USimpleBlendCameraNode::USimpleBlendCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(USimpleBlendCameraNode);
USimpleBlendCameraNode::~USimpleBlendCameraNode() {}
// End Class USimpleBlendCameraNode

// Begin Class USimpleFixedTimeBlendCameraNode
void USimpleFixedTimeBlendCameraNode::StaticRegisterNativesUSimpleFixedTimeBlendCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USimpleFixedTimeBlendCameraNode);
UClass* Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_NoRegister()
{
	return USimpleFixedTimeBlendCameraNode::StaticClass();
}
struct Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Base class for a blend camera node that uses a simple scalar factor over a fixed time.\n */" },
		{ "IncludePath", "Nodes/Blends/SimpleBlendCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/SimpleBlendCameraNode.h" },
		{ "ToolTip", "Base class for a blend camera node that uses a simple scalar factor over a fixed time." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlendTime_MetaData[] = {
		{ "Category", "Blending" },
		{ "Comment", "/** Duration of the blend. */" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/SimpleBlendCameraNode.h" },
		{ "ToolTip", "Duration of the blend." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BlendTime;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USimpleFixedTimeBlendCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::NewProp_BlendTime = { "BlendTime", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USimpleFixedTimeBlendCameraNode, BlendTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlendTime_MetaData), NewProp_BlendTime_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::NewProp_BlendTime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USimpleBlendCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::ClassParams = {
	&USimpleFixedTimeBlendCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::PropPointers),
	0,
	0x002810A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USimpleFixedTimeBlendCameraNode()
{
	if (!Z_Registration_Info_UClass_USimpleFixedTimeBlendCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USimpleFixedTimeBlendCameraNode.OuterSingleton, Z_Construct_UClass_USimpleFixedTimeBlendCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USimpleFixedTimeBlendCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<USimpleFixedTimeBlendCameraNode>()
{
	return USimpleFixedTimeBlendCameraNode::StaticClass();
}
USimpleFixedTimeBlendCameraNode::USimpleFixedTimeBlendCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(USimpleFixedTimeBlendCameraNode);
USimpleFixedTimeBlendCameraNode::~USimpleFixedTimeBlendCameraNode() {}
// End Class USimpleFixedTimeBlendCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SimpleBlendCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USimpleBlendCameraNode, USimpleBlendCameraNode::StaticClass, TEXT("USimpleBlendCameraNode"), &Z_Registration_Info_UClass_USimpleBlendCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USimpleBlendCameraNode), 1737362819U) },
		{ Z_Construct_UClass_USimpleFixedTimeBlendCameraNode, USimpleFixedTimeBlendCameraNode::StaticClass, TEXT("USimpleFixedTimeBlendCameraNode"), &Z_Registration_Info_UClass_USimpleFixedTimeBlendCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USimpleFixedTimeBlendCameraNode), 1864479173U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SimpleBlendCameraNode_h_218783159(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SimpleBlendCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SimpleBlendCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
