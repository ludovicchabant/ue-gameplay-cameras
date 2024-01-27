// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/BlendStackCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBlendStackCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendStackCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UBlendStackCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraNode();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UBlendStackCameraNode
void UBlendStackCameraNode::StaticRegisterNativesUBlendStackCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UBlendStackCameraNode);
UClass* Z_Construct_UClass_UBlendStackCameraNode_NoRegister()
{
	return UBlendStackCameraNode::StaticClass();
}
struct Z_Construct_UClass_UBlendStackCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A blend stack implemented as a camera node.\n */" },
		{ "IncludePath", "Core/BlendStackCameraNode.h" },
		{ "ModuleRelativePath", "Public/Core/BlendStackCameraNode.h" },
		{ "ToolTip", "A blend stack implemented as a camera node." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bAutoPop_MetaData[] = {
		{ "Comment", "/** \n\x09 * Whether to automatically pop camera modes out of the stack when another mode\n\x09 * has reached 100% blend above them.\n\x09 */" },
		{ "ModuleRelativePath", "Public/Core/BlendStackCameraNode.h" },
		{ "ToolTip", "Whether to automatically pop camera modes out of the stack when another mode\nhas reached 100% blend above them." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bBlendFirstCameraMode_MetaData[] = {
		{ "Comment", "/**\n\x09 * Whether to blend-in the first camera mode when the stack is previously empty.\n\x09 */" },
		{ "ModuleRelativePath", "Public/Core/BlendStackCameraNode.h" },
		{ "ToolTip", "Whether to blend-in the first camera mode when the stack is previously empty." },
	};
#endif // WITH_METADATA
	static void NewProp_bAutoPop_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bAutoPop;
	static void NewProp_bBlendFirstCameraMode_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bBlendFirstCameraMode;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBlendStackCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
void Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bAutoPop_SetBit(void* Obj)
{
	((UBlendStackCameraNode*)Obj)->bAutoPop = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bAutoPop = { "bAutoPop", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UBlendStackCameraNode), &Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bAutoPop_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bAutoPop_MetaData), NewProp_bAutoPop_MetaData) };
void Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bBlendFirstCameraMode_SetBit(void* Obj)
{
	((UBlendStackCameraNode*)Obj)->bBlendFirstCameraMode = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bBlendFirstCameraMode = { "bBlendFirstCameraMode", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UBlendStackCameraNode), &Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bBlendFirstCameraMode_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bBlendFirstCameraMode_MetaData), NewProp_bBlendFirstCameraMode_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBlendStackCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bAutoPop,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlendStackCameraNode_Statics::NewProp_bBlendFirstCameraMode,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UBlendStackCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBlendStackCameraNode_Statics::ClassParams = {
	&UBlendStackCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UBlendStackCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackCameraNode_Statics::PropPointers),
	0,
	0x002810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBlendStackCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UBlendStackCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UBlendStackCameraNode()
{
	if (!Z_Registration_Info_UClass_UBlendStackCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBlendStackCameraNode.OuterSingleton, Z_Construct_UClass_UBlendStackCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBlendStackCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UBlendStackCameraNode>()
{
	return UBlendStackCameraNode::StaticClass();
}
UBlendStackCameraNode::UBlendStackCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UBlendStackCameraNode);
UBlendStackCameraNode::~UBlendStackCameraNode() {}
// End Class UBlendStackCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_BlendStackCameraNode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBlendStackCameraNode, UBlendStackCameraNode::StaticClass, TEXT("UBlendStackCameraNode"), &Z_Registration_Info_UClass_UBlendStackCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBlendStackCameraNode), 814369789U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_BlendStackCameraNode_h_1346649932(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_BlendStackCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_BlendStackCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
