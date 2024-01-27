// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Nodes/Blends/SmoothBlendCameraNode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSmoothBlendCameraNode() {}

// Begin Cross Module References
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USimpleFixedTimeBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USmoothBlendCameraNode();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_USmoothBlendCameraNode_NoRegister();
GAMEPLAYCAMERAS_API UEnum* Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Enum ESmoothCameraBlendType
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ESmoothCameraBlendType;
static UEnum* ESmoothCameraBlendType_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ESmoothCameraBlendType.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ESmoothCameraBlendType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ESmoothCameraBlendType"));
	}
	return Z_Registration_Info_UEnum_ESmoothCameraBlendType.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ESmoothCameraBlendType>()
{
	return ESmoothCameraBlendType_StaticEnum();
}
struct Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Comment", "/**\n * The smoothstep type.\n */" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/SmoothBlendCameraNode.h" },
		{ "SmootherStep.Name", "ESmoothCameraBlendType::SmootherStep" },
		{ "SmoothStep.Name", "ESmoothCameraBlendType::SmoothStep" },
		{ "ToolTip", "The smoothstep type." },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ESmoothCameraBlendType::SmoothStep", (int64)ESmoothCameraBlendType::SmoothStep },
		{ "ESmoothCameraBlendType::SmootherStep", (int64)ESmoothCameraBlendType::SmootherStep },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	"ESmoothCameraBlendType",
	"ESmoothCameraBlendType",
	Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType()
{
	if (!Z_Registration_Info_UEnum_ESmoothCameraBlendType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ESmoothCameraBlendType.InnerSingleton, Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ESmoothCameraBlendType.InnerSingleton;
}
// End Enum ESmoothCameraBlendType

// Begin Class USmoothBlendCameraNode
void USmoothBlendCameraNode::StaticRegisterNativesUSmoothBlendCameraNode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(USmoothBlendCameraNode);
UClass* Z_Construct_UClass_USmoothBlendCameraNode_NoRegister()
{
	return USmoothBlendCameraNode::StaticClass();
}
struct Z_Construct_UClass_USmoothBlendCameraNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A blend camera mode that implements the smoothstep and smoothersteps algorithms.\n */" },
		{ "IncludePath", "Nodes/Blends/SmoothBlendCameraNode.h" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/SmoothBlendCameraNode.h" },
		{ "ToolTip", "A blend camera mode that implements the smoothstep and smoothersteps algorithms." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlendType_MetaData[] = {
		{ "Category", "Blending" },
		{ "Comment", "/** The type of algorithm to use. */" },
		{ "ModuleRelativePath", "Public/Nodes/Blends/SmoothBlendCameraNode.h" },
		{ "ToolTip", "The type of algorithm to use." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_BlendType_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_BlendType;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USmoothBlendCameraNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_USmoothBlendCameraNode_Statics::NewProp_BlendType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_USmoothBlendCameraNode_Statics::NewProp_BlendType = { "BlendType", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USmoothBlendCameraNode, BlendType), Z_Construct_UEnum_GameplayCameras_ESmoothCameraBlendType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlendType_MetaData), NewProp_BlendType_MetaData) }; // 4009144560
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USmoothBlendCameraNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USmoothBlendCameraNode_Statics::NewProp_BlendType_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USmoothBlendCameraNode_Statics::NewProp_BlendType,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USmoothBlendCameraNode_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_USmoothBlendCameraNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USimpleFixedTimeBlendCameraNode,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USmoothBlendCameraNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USmoothBlendCameraNode_Statics::ClassParams = {
	&USmoothBlendCameraNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_USmoothBlendCameraNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_USmoothBlendCameraNode_Statics::PropPointers),
	0,
	0x002810A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USmoothBlendCameraNode_Statics::Class_MetaDataParams), Z_Construct_UClass_USmoothBlendCameraNode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_USmoothBlendCameraNode()
{
	if (!Z_Registration_Info_UClass_USmoothBlendCameraNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USmoothBlendCameraNode.OuterSingleton, Z_Construct_UClass_USmoothBlendCameraNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USmoothBlendCameraNode.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<USmoothBlendCameraNode>()
{
	return USmoothBlendCameraNode::StaticClass();
}
USmoothBlendCameraNode::USmoothBlendCameraNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(USmoothBlendCameraNode);
USmoothBlendCameraNode::~USmoothBlendCameraNode() {}
// End Class USmoothBlendCameraNode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ ESmoothCameraBlendType_StaticEnum, TEXT("ESmoothCameraBlendType"), &Z_Registration_Info_UEnum_ESmoothCameraBlendType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 4009144560U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USmoothBlendCameraNode, USmoothBlendCameraNode::StaticClass, TEXT("USmoothBlendCameraNode"), &Z_Registration_Info_UClass_USmoothBlendCameraNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USmoothBlendCameraNode), 472055425U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_1333577519(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Nodes_Blends_SmoothBlendCameraNode_h_Statics::EnumInfo));
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
