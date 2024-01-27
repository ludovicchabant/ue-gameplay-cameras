// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/RootCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRootCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_URootCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_URootCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UEnum* Z_Construct_UEnum_GameplayCameras_ECameraModeLayer();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Enum ECameraModeLayer
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ECameraModeLayer;
static UEnum* ECameraModeLayer_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ECameraModeLayer.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ECameraModeLayer.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayCameras_ECameraModeLayer, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ECameraModeLayer"));
	}
	return Z_Registration_Info_UEnum_ECameraModeLayer.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ECameraModeLayer>()
{
	return ECameraModeLayer_StaticEnum();
}
struct Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Base.Name", "ECameraModeLayer::Base" },
		{ "Comment", "/**\n * Defines evaluation layers for camera modes.\n */" },
		{ "Global.Name", "ECameraModeLayer::Global" },
		{ "Main.Name", "ECameraModeLayer::Main" },
		{ "ModuleRelativePath", "Public/Core/RootCameraNode.h" },
		{ "ToolTip", "Defines evaluation layers for camera modes." },
		{ "User0.Name", "ECameraModeLayer::User0" },
		{ "User1.Name", "ECameraModeLayer::User1" },
		{ "User2.Name", "ECameraModeLayer::User2" },
		{ "Visual.Name", "ECameraModeLayer::Visual" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ECameraModeLayer::Base", (int64)ECameraModeLayer::Base },
		{ "ECameraModeLayer::Main", (int64)ECameraModeLayer::Main },
		{ "ECameraModeLayer::Global", (int64)ECameraModeLayer::Global },
		{ "ECameraModeLayer::Visual", (int64)ECameraModeLayer::Visual },
		{ "ECameraModeLayer::User0", (int64)ECameraModeLayer::User0 },
		{ "ECameraModeLayer::User1", (int64)ECameraModeLayer::User1 },
		{ "ECameraModeLayer::User2", (int64)ECameraModeLayer::User2 },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	"ECameraModeLayer",
	"ECameraModeLayer",
	Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayCameras_ECameraModeLayer()
{
	if (!Z_Registration_Info_UEnum_ECameraModeLayer.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ECameraModeLayer.InnerSingleton, Z_Construct_UEnum_GameplayCameras_ECameraModeLayer_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ECameraModeLayer.InnerSingleton;
}
// End Enum ECameraModeLayer

// Begin Class URootCameraNode
void URootCameraNode::StaticRegisterNativesURootCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(URootCameraNode);
UClass* Z_Construct_UClass_URootCameraNode_NoRegister()
{
	return URootCameraNode::StaticClass();
}
struct Z_Construct_UClass_URootCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * The base class for a camera node that can act as the root of the\n * camera system evaluation.\n */" },
		{ "IncludePath", "Core/RootCameraNode.h" },
		{ "ModuleRelativePath", "Public/Core/RootCameraNode.h" },
		{ "ToolTip", "The base class for a camera node that can act as the root of the\ncamera system evaluation." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<URootCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_URootCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_URootCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_URootCameraNode_Statics::ClassParams = {
	&URootCameraNode::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_URootCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_URootCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_URootCameraNode()
{
	if (!Z_Registration_Info_UClass_URootCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_URootCameraNode.OuterSingleton, Z_Construct_UClass_URootCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_URootCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<URootCameraNode>()
{
	return URootCameraNode::StaticClass();
}
URootCameraNode::URootCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(URootCameraNode);
URootCameraNode::~URootCameraNode() {}
// End Class URootCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_RootCameraNode_h_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ ECameraModeLayer_StaticEnum, TEXT("ECameraModeLayer"), &Z_Registration_Info_UEnum_ECameraModeLayer, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2541064786U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_URootCameraNode, URootCameraNode::StaticClass, TEXT("URootCameraNode"), &Z_Registration_Info_UClass_URootCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(URootCameraNode), 1926402885U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_RootCameraNode_h_1014959280(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_RootCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_RootCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_RootCameraNode_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_RootCameraNode_h_Statics::EnumInfo));
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
