// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/GameFramework/GameplayCameraComponent.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCameraComponent() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_USceneComponent();
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UStaticMeshComponent_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraAsset_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraEvaluationContext();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraComponent();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraComponent_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraComponentEvaluationContext();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UGameplayCameraComponent Function ActivateCamera
struct Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics
{
	struct GameplayCameraComponent_eventActivateCamera_Parms
	{
		int32 PlayerIndex;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera" },
		{ "CPP_Default_PlayerIndex", "0" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_PlayerIndex;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::NewProp_PlayerIndex = { "PlayerIndex", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCameraComponent_eventActivateCamera_Parms, PlayerIndex), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::NewProp_PlayerIndex,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCameraComponent, nullptr, "ActivateCamera", nullptr, nullptr, Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::GameplayCameraComponent_eventActivateCamera_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::GameplayCameraComponent_eventActivateCamera_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCameraComponent::execActivateCamera)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_PlayerIndex);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ActivateCamera(Z_Param_PlayerIndex);
	P_NATIVE_END;
}
// End Class UGameplayCameraComponent Function ActivateCamera

// Begin Class UGameplayCameraComponent
void UGameplayCameraComponent::StaticRegisterNativesUGameplayCameraComponent()
{
	UClass* Class = UGameplayCameraComponent::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "ActivateCamera", &UGameplayCameraComponent::execActivateCamera },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGameplayCameraComponent);
UClass* Z_Construct_UClass_UGameplayCameraComponent_NoRegister()
{
	return UGameplayCameraComponent::StaticClass();
}
struct Z_Construct_UClass_UGameplayCameraComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Camera" },
		{ "Comment", "/**\n * A component that can run a camera asset inside its own camera evaluation context.\n */" },
		{ "HideCategories", "Mobility Rendering LOD Trigger PhysicsVolume" },
		{ "IncludePath", "GameFramework/GameplayCameraComponent.h" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A component that can run a camera asset inside its own camera evaluation context." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Camera_MetaData[] = {
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EvaluationContext_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
	};
#if WITH_EDITORONLY_DATA
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PreviewMesh_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PreviewMeshComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
	};
#endif // WITH_EDITORONLY_DATA
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Camera;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_EvaluationContext;
#if WITH_EDITORONLY_DATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PreviewMesh;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PreviewMeshComponent;
#endif // WITH_EDITORONLY_DATA
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayCameraComponent_ActivateCamera, "ActivateCamera" }, // 1917404453
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCameraComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_Camera = { "Camera", nullptr, (EPropertyFlags)0x0114000000000005, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraComponent, Camera), Z_Construct_UClass_UCameraAsset_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Camera_MetaData), NewProp_Camera_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_EvaluationContext = { "EvaluationContext", nullptr, (EPropertyFlags)0x0124080000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraComponent, EvaluationContext), Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EvaluationContext_MetaData), NewProp_EvaluationContext_MetaData) };
#if WITH_EDITORONLY_DATA
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_PreviewMesh = { "PreviewMesh", nullptr, (EPropertyFlags)0x0124080800000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraComponent, PreviewMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PreviewMesh_MetaData), NewProp_PreviewMesh_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_PreviewMeshComponent = { "PreviewMeshComponent", nullptr, (EPropertyFlags)0x0124080800080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGameplayCameraComponent, PreviewMeshComponent), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PreviewMeshComponent_MetaData), NewProp_PreviewMeshComponent_MetaData) };
#endif // WITH_EDITORONLY_DATA
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayCameraComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_Camera,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_EvaluationContext,
#if WITH_EDITORONLY_DATA
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_PreviewMesh,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCameraComponent_Statics::NewProp_PreviewMeshComponent,
#endif // WITH_EDITORONLY_DATA
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraComponent_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UGameplayCameraComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USceneComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCameraComponent_Statics::ClassParams = {
	&UGameplayCameraComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UGameplayCameraComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraComponent_Statics::PropPointers),
	0,
	0x00A800A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UGameplayCameraComponent_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGameplayCameraComponent()
{
	if (!Z_Registration_Info_UClass_UGameplayCameraComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGameplayCameraComponent.OuterSingleton, Z_Construct_UClass_UGameplayCameraComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGameplayCameraComponent.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UGameplayCameraComponent>()
{
	return UGameplayCameraComponent::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCameraComponent);
UGameplayCameraComponent::~UGameplayCameraComponent() {}
// End Class UGameplayCameraComponent

// Begin Class UGameplayCameraComponentEvaluationContext
void UGameplayCameraComponentEvaluationContext::StaticRegisterNativesUGameplayCameraComponentEvaluationContext()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGameplayCameraComponentEvaluationContext);
UClass* Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_NoRegister()
{
	return UGameplayCameraComponentEvaluationContext::StaticClass();
}
struct Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameFramework/GameplayCameraComponent.h" },
		{ "ModuleRelativePath", "Public/GameFramework/GameplayCameraComponent.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCameraComponentEvaluationContext>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraEvaluationContext,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics::ClassParams = {
	&UGameplayCameraComponentEvaluationContext::StaticClass,
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
	0x000800A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics::Class_MetaDataParams), Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGameplayCameraComponentEvaluationContext()
{
	if (!Z_Registration_Info_UClass_UGameplayCameraComponentEvaluationContext.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGameplayCameraComponentEvaluationContext.OuterSingleton, Z_Construct_UClass_UGameplayCameraComponentEvaluationContext_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGameplayCameraComponentEvaluationContext.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UGameplayCameraComponentEvaluationContext>()
{
	return UGameplayCameraComponentEvaluationContext::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCameraComponentEvaluationContext);
UGameplayCameraComponentEvaluationContext::~UGameplayCameraComponentEvaluationContext() {}
// End Class UGameplayCameraComponentEvaluationContext

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraComponent_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGameplayCameraComponent, UGameplayCameraComponent::StaticClass, TEXT("UGameplayCameraComponent"), &Z_Registration_Info_UClass_UGameplayCameraComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGameplayCameraComponent), 14517124U) },
		{ Z_Construct_UClass_UGameplayCameraComponentEvaluationContext, UGameplayCameraComponentEvaluationContext::StaticClass, TEXT("UGameplayCameraComponentEvaluationContext"), &Z_Registration_Info_UClass_UGameplayCameraComponentEvaluationContext, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGameplayCameraComponentEvaluationContext), 2518267221U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraComponent_h_930770508(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameFramework_GameplayCameraComponent_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
