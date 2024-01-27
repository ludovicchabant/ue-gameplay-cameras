// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/GameFramework/GameplayCameraActor.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCameraActor() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_USceneComponent_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_AGameplayCameraActor();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_AGameplayCameraActor_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class AGameplayCameraActor Function GetCameraComponent
struct Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics
{
	struct GameplayCameraActor_eventGetCameraComponent_Parms
	{
		UGameplayCameraComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintGetter", "" },
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCameraActor_eventGetCameraComponent_Parms, ReturnValue), Z_Construct_UClass_UGameplayCameraComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCameraActor, nullptr, "GetCameraComponent", nullptr, nullptr, Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::PropPointers), sizeof(Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::GameplayCameraActor_eventGetCameraComponent_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::Function_MetaDataParams), Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::GameplayCameraActor_eventGetCameraComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AGameplayCameraActor::execGetCameraComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UGameplayCameraComponent**)Z_Param__Result=P_THIS->GetCameraComponent();
	P_NATIVE_END;
}
// End Class AGameplayCameraActor Function GetCameraComponent

// Begin Class AGameplayCameraActor Function GetSceneComponent
struct Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics
{
	struct GameplayCameraActor_eventGetSceneComponent_Parms
	{
		USceneComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintGetter", "" },
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCameraActor_eventGetSceneComponent_Parms, ReturnValue), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCameraActor, nullptr, "GetSceneComponent", nullptr, nullptr, Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::PropPointers), sizeof(Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::GameplayCameraActor_eventGetSceneComponent_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::Function_MetaDataParams), Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::GameplayCameraActor_eventGetSceneComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AGameplayCameraActor::execGetSceneComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(USceneComponent**)Z_Param__Result=P_THIS->GetSceneComponent();
	P_NATIVE_END;
}
// End Class AGameplayCameraActor Function GetSceneComponent

// Begin Class AGameplayCameraActor
void AGameplayCameraActor::StaticRegisterNativesAGameplayCameraActor()
{
	UClass* Class = AGameplayCameraActor::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "GetCameraComponent", &AGameplayCameraActor::execGetCameraComponent },
		{ "GetSceneComponent", &AGameplayCameraActor::execGetSceneComponent },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AGameplayCameraActor);
UClass* Z_Construct_UClass_AGameplayCameraActor_NoRegister()
{
	return AGameplayCameraActor::StaticClass();
}
struct Z_Construct_UClass_AGameplayCameraActor_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Camera" },
		{ "Comment", "/**\n * An actor that can run a camera asset.\n */" },
		{ "HideCategories", "Input Rendering" },
		{ "IncludePath", "GameFramework/GameplayCameraActor.h" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraActor.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "An actor that can run a camera asset." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SceneComponent_MetaData[] = {
		{ "BlueprintGetter", "GetSceneComponent" },
		{ "Category", "Camera" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraComponent_MetaData[] = {
		{ "BlueprintGetter", "GetCameraComponent" },
		{ "Category", "Camera" },
		{ "EditInline", "true" },
		{ "ExposeFunctionCategories", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraActor.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_SceneComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraComponent;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_AGameplayCameraActor_GetCameraComponent, "GetCameraComponent" }, // 1267843943
		{ &Z_Construct_UFunction_AGameplayCameraActor_GetSceneComponent, "GetSceneComponent" }, // 190401487
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayCameraActor>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayCameraActor_Statics::NewProp_SceneComponent = { "SceneComponent", nullptr, (EPropertyFlags)0x01440000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AGameplayCameraActor, SceneComponent), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SceneComponent_MetaData), NewProp_SceneComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayCameraActor_Statics::NewProp_CameraComponent = { "CameraComponent", nullptr, (EPropertyFlags)0x01440000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AGameplayCameraActor, CameraComponent), Z_Construct_UClass_UGameplayCameraComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraComponent_MetaData), NewProp_CameraComponent_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AGameplayCameraActor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCameraActor_Statics::NewProp_SceneComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCameraActor_Statics::NewProp_CameraComponent,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraActor_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AGameplayCameraActor_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraActor_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AGameplayCameraActor_Statics::ClassParams = {
	&AGameplayCameraActor::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_AGameplayCameraActor_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraActor_Statics::PropPointers),
	0,
	0x008800A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraActor_Statics::Class_MetaDataParams), Z_Construct_UClass_AGameplayCameraActor_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AGameplayCameraActor()
{
	if (!Z_Registration_Info_UClass_AGameplayCameraActor.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AGameplayCameraActor.OuterSingleton, Z_Construct_UClass_AGameplayCameraActor_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AGameplayCameraActor.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<AGameplayCameraActor>()
{
	return AGameplayCameraActor::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayCameraActor);
AGameplayCameraActor::~AGameplayCameraActor() {}
// End Class AGameplayCameraActor

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraActor_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AGameplayCameraActor, AGameplayCameraActor::StaticClass, TEXT("AGameplayCameraActor"), &Z_Registration_Info_UClass_AGameplayCameraActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AGameplayCameraActor), 1959443424U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraActor_h_2892291291(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraActor_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraActor_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
