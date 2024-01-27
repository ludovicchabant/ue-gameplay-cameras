// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/GameFramework/GameplayCameraSystemActor.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCameraSystemActor() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_EAutoReceiveInput();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_AGameplayCameraSystemActor();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_AGameplayCameraSystemActor_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraSystemComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class AGameplayCameraSystemActor Function ActivateForPlayer
struct Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics
{
	struct GameplayCameraSystemActor_eventActivateForPlayer_Parms
	{
		int32 PlayerIndex;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemActor.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_PlayerIndex;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::NewProp_PlayerIndex = { "PlayerIndex", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCameraSystemActor_eventActivateForPlayer_Parms, PlayerIndex), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::NewProp_PlayerIndex,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCameraSystemActor, nullptr, "ActivateForPlayer", nullptr, nullptr, Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::PropPointers), sizeof(Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::GameplayCameraSystemActor_eventActivateForPlayer_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::Function_MetaDataParams), Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::GameplayCameraSystemActor_eventActivateForPlayer_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AGameplayCameraSystemActor::execActivateForPlayer)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_PlayerIndex);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ActivateForPlayer(Z_Param_PlayerIndex);
	P_NATIVE_END;
}
// End Class AGameplayCameraSystemActor Function ActivateForPlayer

// Begin Class AGameplayCameraSystemActor Function GetCameraSystemComponent
struct Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics
{
	struct GameplayCameraSystemActor_eventGetCameraSystemComponent_Parms
	{
		UGameplayCameraSystemComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintGetter", "" },
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCameraSystemActor_eventGetCameraSystemComponent_Parms, ReturnValue), Z_Construct_UClass_UGameplayCameraSystemComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCameraSystemActor, nullptr, "GetCameraSystemComponent", nullptr, nullptr, Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::PropPointers), sizeof(Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::GameplayCameraSystemActor_eventGetCameraSystemComponent_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::Function_MetaDataParams), Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::GameplayCameraSystemActor_eventGetCameraSystemComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AGameplayCameraSystemActor::execGetCameraSystemComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UGameplayCameraSystemComponent**)Z_Param__Result=P_THIS->GetCameraSystemComponent();
	P_NATIVE_END;
}
// End Class AGameplayCameraSystemActor Function GetCameraSystemComponent

// Begin Class AGameplayCameraSystemActor
void AGameplayCameraSystemActor::StaticRegisterNativesAGameplayCameraSystemActor()
{
	UClass* Class = AGameplayCameraSystemActor::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "ActivateForPlayer", &AGameplayCameraSystemActor::execActivateForPlayer },
		{ "GetCameraSystemComponent", &AGameplayCameraSystemActor::execGetCameraSystemComponent },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AGameplayCameraSystemActor);
UClass* Z_Construct_UClass_AGameplayCameraSystemActor_NoRegister()
{
	return AGameplayCameraSystemActor::StaticClass();
}
struct Z_Construct_UClass_AGameplayCameraSystemActor_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Camera" },
		{ "Comment", "/**\n * An actor that hosts a camera system.\n */" },
		{ "HideCategories", "Input Rendering" },
		{ "IncludePath", "GameFramework/GameplayCameraSystemActor.h" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemActor.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "An actor that hosts a camera system." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraSystemComponent_MetaData[] = {
		{ "BlueprintGetter", "GetCameraSystemComponent" },
		{ "Category", "Camera" },
		{ "EditInline", "true" },
		{ "ExposeFunctionCategories", "CameraSystem" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AutoActivateForPlayer_MetaData[] = {
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemActor.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CameraSystemComponent;
	static const UECodeGen_Private::FBytePropertyParams NewProp_AutoActivateForPlayer;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_AGameplayCameraSystemActor_ActivateForPlayer, "ActivateForPlayer" }, // 566790533
		{ &Z_Construct_UFunction_AGameplayCameraSystemActor_GetCameraSystemComponent, "GetCameraSystemComponent" }, // 87555954
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayCameraSystemActor>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayCameraSystemActor_Statics::NewProp_CameraSystemComponent = { "CameraSystemComponent", nullptr, (EPropertyFlags)0x01440000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AGameplayCameraSystemActor, CameraSystemComponent), Z_Construct_UClass_UGameplayCameraSystemComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraSystemComponent_MetaData), NewProp_CameraSystemComponent_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_AGameplayCameraSystemActor_Statics::NewProp_AutoActivateForPlayer = { "AutoActivateForPlayer", nullptr, (EPropertyFlags)0x0040000000000001, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AGameplayCameraSystemActor, AutoActivateForPlayer), Z_Construct_UEnum_Engine_EAutoReceiveInput, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AutoActivateForPlayer_MetaData), NewProp_AutoActivateForPlayer_MetaData) }; // 1625585255
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AGameplayCameraSystemActor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCameraSystemActor_Statics::NewProp_CameraSystemComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCameraSystemActor_Statics::NewProp_AutoActivateForPlayer,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraSystemActor_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AGameplayCameraSystemActor_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraSystemActor_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AGameplayCameraSystemActor_Statics::ClassParams = {
	&AGameplayCameraSystemActor::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_AGameplayCameraSystemActor_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraSystemActor_Statics::PropPointers),
	0,
	0x008800A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AGameplayCameraSystemActor_Statics::Class_MetaDataParams), Z_Construct_UClass_AGameplayCameraSystemActor_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AGameplayCameraSystemActor()
{
	if (!Z_Registration_Info_UClass_AGameplayCameraSystemActor.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AGameplayCameraSystemActor.OuterSingleton, Z_Construct_UClass_AGameplayCameraSystemActor_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AGameplayCameraSystemActor.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<AGameplayCameraSystemActor>()
{
	return AGameplayCameraSystemActor::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayCameraSystemActor);
AGameplayCameraSystemActor::~AGameplayCameraSystemActor() {}
// End Class AGameplayCameraSystemActor

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemActor_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AGameplayCameraSystemActor, AGameplayCameraSystemActor::StaticClass, TEXT("AGameplayCameraSystemActor"), &Z_Registration_Info_UClass_AGameplayCameraSystemActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AGameplayCameraSystemActor), 3394342574U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemActor_h_2243507171(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemActor_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemActor_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
