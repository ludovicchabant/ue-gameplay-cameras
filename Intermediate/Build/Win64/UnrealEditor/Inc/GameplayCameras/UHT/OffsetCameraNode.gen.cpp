// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Nodes/Common/OffsetCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeOffsetCameraNode() {}

// Begin Cross Module References
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector3d();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UOffsetCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UOffsetCameraNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UOffsetCameraNode
void UOffsetCameraNode::StaticRegisterNativesUOffsetCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UOffsetCameraNode);
UClass* Z_Construct_UClass_UOffsetCameraNode_NoRegister()
{
	return UOffsetCameraNode::StaticClass();
}
struct Z_Construct_UClass_UOffsetCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A camera node that offsets the location of the camera.\n */" },
		{ "IncludePath", "Nodes/Common/OffsetCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Common/OffsetCameraNode.h" },
		{ "ToolTip", "A camera node that offsets the location of the camera." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Offset_MetaData[] = {
		{ "Category", "Common" },
		{ "Comment", "/** The offset to apply to the camera, in local space. */" },
		{ "ModuleRelativePath", "Public/Nodes/Common/OffsetCameraNode.h" },
		{ "ToolTip", "The offset to apply to the camera, in local space." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_Offset;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UOffsetCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UOffsetCameraNode_Statics::NewProp_Offset = { "Offset", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UOffsetCameraNode, Offset), Z_Construct_UScriptStruct_FVector3d, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Offset_MetaData), NewProp_Offset_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UOffsetCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UOffsetCameraNode_Statics::NewProp_Offset,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UOffsetCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UOffsetCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UOffsetCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UOffsetCameraNode_Statics::ClassParams = {
	&UOffsetCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UOffsetCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UOffsetCameraNode_Statics::PropPointers),
	0,
	0x002810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UOffsetCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UOffsetCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UOffsetCameraNode()
{
	if (!Z_Registration_Info_UClass_UOffsetCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UOffsetCameraNode.OuterSingleton, Z_Construct_UClass_UOffsetCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UOffsetCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UOffsetCameraNode>()
{
	return UOffsetCameraNode::StaticClass();
}
UOffsetCameraNode::UOffsetCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UOffsetCameraNode);
UOffsetCameraNode::~UOffsetCameraNode() {}
// End Class UOffsetCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_OffsetCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UOffsetCameraNode, UOffsetCameraNode::StaticClass, TEXT("UOffsetCameraNode"), &Z_Registration_Info_UClass_UOffsetCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UOffsetCameraNode), 4215552215U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_OffsetCameraNode_h_1353802148(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_OffsetCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Common_OffsetCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
