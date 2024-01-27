// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/GameFramework/GameplayCameraSystemComponent.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCameraSystemComponent() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_USceneComponent();
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UStaticMeshComponent_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraSystemEvaluator_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraSystemComponent();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraSystemComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UGameplayCameraSystemComponent Function GetCameraSystemEvaluator
struct Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics
{
	struct GameplayCameraSystemComponent_eventGetCameraSystemEvaluator_Parms
	{
		UCameraSystemEvaluator* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintGetter", "" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemComponent.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCameraSystemComponent_eventGetCameraSystemEvaluator_Parms, ReturnValue), Z_Construct_UClass_UCameraSystemEvaluator_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCameraSystemComponent, nullptr, "GetCameraSystemEvaluator", nullptr, nullptr, Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::GameplayCameraSystemComponent_eventGetCameraSystemEvaluator_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::GameplayCameraSystemComponent_eventGetCameraSystemEvaluator_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCameraSystemComponent::execGetCameraSystemEvaluator)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UCameraSystemEvaluator**)Z_Param__Result=P_THIS->GetCameraSystemEvaluator();
	P_NATIVE_END;
}
// End Class UGameplayCameraSystemComponent Function GetCameraSystemEvaluator

// Begin Class UGameplayCameraSystemComponent
void UGameplayCameraSystemComponent::StaticRegisterNativesUGameplayCameraSystemComponent()
{
	UClass* Class = UGameplayCameraSystemComponent::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "GetCameraSystemEvaluator", &UGameplayCameraSystemComponent::execGetCameraSystemEvaluator },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGameplayCameraSystemComponent);
UClass* Z_Construct_UClass_UGameplayCameraSystemComponent_NoRegister()
{
	return UGameplayCameraSystemComponent::StaticClass();
}
struct Z_Construct_UClass_UGameplayCameraSystemComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Camera" },
		{ "Comment", "/**\n * A component that hosts a camera system.\n */" },
		{ "HideCategories", "Mobility Rendering LOD Trigger PhysicsVolume" },
		{ "IncludePath", "GameFramework/GameplayCameraSystemComponent.h" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemComponent.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A component that hosts a camera system." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Evaluator_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemComponent.h" },
	};
#if WITH_EDITORONLY_DATA
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PreviewMesh_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PreviewMeshComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraSystemComponent.h" },
	};
#endif // WITH_EDITORONLY_DATA
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Evaluator;
#if WITH_EDITORONLY_DATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PreviewMesh;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PreviewMeshComponent;
#endif // WITH_EDITORONLY_DATA
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayCameraSystemComponent_GetCameraSystemEvaluator, "GetCameraSystemEvaluator" }, // 3932237585
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCameraSystemComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::NewProp_Evaluator = { "Evaluator", nullptr, (EPropertyFlags)0x0146000000082008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraSystemComponent, Evaluator), Z_Construct_UClass_UCameraSystemEvaluator_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Evaluator_MetaData), NewProp_Evaluator_MetaData) };
#if WITH_EDITORONLY_DATA
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::NewProp_PreviewMesh = { "PreviewMesh", nullptr, (EPropertyFlags)0x0144000800000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraSystemComponent, PreviewMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PreviewMesh_MetaData), NewProp_PreviewMesh_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::NewProp_PreviewMeshComponent = { "PreviewMeshComponent", nullptr, (EPropertyFlags)0x0144000800080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraSystemComponent, PreviewMeshComponent), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PreviewMeshComponent_MetaData), NewProp_PreviewMeshComponent_MetaData) };
#endif // WITH_EDITORONLY_DATA
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::NewProp_Evaluator,
#if WITH_EDITORONLY_DATA
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::NewProp_PreviewMesh,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::NewProp_PreviewMeshComponent,
#endif // WITH_EDITORONLY_DATA
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USceneComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::ClassParams = {
	&UGameplayCameraSystemComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::PropPointers),
	0,
	0x00A800A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGameplayCameraSystemComponent()
{
	if (!Z_Registration_Info_UClass_UGameplayCameraSystemComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGameplayCameraSystemComponent.OuterSingleton, Z_Construct_UClass_UGameplayCameraSystemComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGameplayCameraSystemComponent.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UGameplayCameraSystemComponent>()
{
	return UGameplayCameraSystemComponent::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCameraSystemComponent);
UGameplayCameraSystemComponent::~UGameplayCameraSystemComponent() {}
// End Class UGameplayCameraSystemComponent

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemComponent_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGameplayCameraSystemComponent, UGameplayCameraSystemComponent::StaticClass, TEXT("UGameplayCameraSystemComponent"), &Z_Registration_Info_UClass_UGameplayCameraSystemComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGameplayCameraSystemComponent), 3280917110U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemComponent_h_2011637583(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraSystemComponent_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
