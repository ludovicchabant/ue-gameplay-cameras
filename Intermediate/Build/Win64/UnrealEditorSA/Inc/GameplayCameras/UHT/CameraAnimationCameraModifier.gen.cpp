// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/CameraAnimationCameraModifier.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraAnimationCameraModifier() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FRotator();
ENGINE_API UClass* Z_Construct_UClass_APlayerCameraManager_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_APlayerController_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
ENGINE_API UClass* Z_Construct_UClass_UCameraModifier();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_ECameraShakePlaySpace();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraAnimationCameraModifier();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UCameraAnimationCameraModifier_NoRegister();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCamerasFunctionLibrary();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCamerasFunctionLibrary_NoRegister();
GAMEPLAYCAMERAS_API UEnum* Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType();
GAMEPLAYCAMERAS_API UEnum* Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FActiveCameraAnimationInfo();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraAnimationHandle();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraAnimationParams();
TEMPLATESEQUENCE_API UClass* Z_Construct_UClass_UCameraAnimationSequence_NoRegister();
TEMPLATESEQUENCE_API UClass* Z_Construct_UClass_UCameraAnimationSequenceCameraStandIn_NoRegister();
TEMPLATESEQUENCE_API UClass* Z_Construct_UClass_UCameraAnimationSequencePlayer_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Enum ECameraAnimationPlaySpace
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ECameraAnimationPlaySpace;
static UEnum* ECameraAnimationPlaySpace_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ECameraAnimationPlaySpace.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ECameraAnimationPlaySpace.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ECameraAnimationPlaySpace"));
	}
	return Z_Registration_Info_UEnum_ECameraAnimationPlaySpace.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ECameraAnimationPlaySpace>()
{
	return ECameraAnimationPlaySpace_StaticEnum();
}
struct Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "CameraLocal.Comment", "/** This anim is applied in camera space. */" },
		{ "CameraLocal.Name", "ECameraAnimationPlaySpace::CameraLocal" },
		{ "CameraLocal.ToolTip", "This anim is applied in camera space." },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "UserDefined.Comment", "/** This anim is applied in a user-specified space (defined by UserPlaySpaceMatrix). */" },
		{ "UserDefined.Name", "ECameraAnimationPlaySpace::UserDefined" },
		{ "UserDefined.ToolTip", "This anim is applied in a user-specified space (defined by UserPlaySpaceMatrix)." },
		{ "World.Comment", "/** This anim is applied in world space. */" },
		{ "World.Name", "ECameraAnimationPlaySpace::World" },
		{ "World.ToolTip", "This anim is applied in world space." },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ECameraAnimationPlaySpace::CameraLocal", (int64)ECameraAnimationPlaySpace::CameraLocal },
		{ "ECameraAnimationPlaySpace::World", (int64)ECameraAnimationPlaySpace::World },
		{ "ECameraAnimationPlaySpace::UserDefined", (int64)ECameraAnimationPlaySpace::UserDefined },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	"ECameraAnimationPlaySpace",
	"ECameraAnimationPlaySpace",
	Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace()
{
	if (!Z_Registration_Info_UEnum_ECameraAnimationPlaySpace.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ECameraAnimationPlaySpace.InnerSingleton, Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ECameraAnimationPlaySpace.InnerSingleton;
}
// End Enum ECameraAnimationPlaySpace

// Begin Enum ECameraAnimationEasingType
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ECameraAnimationEasingType;
static UEnum* ECameraAnimationEasingType_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ECameraAnimationEasingType.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ECameraAnimationEasingType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ECameraAnimationEasingType"));
	}
	return Z_Registration_Info_UEnum_ECameraAnimationEasingType.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UEnum* StaticEnum<ECameraAnimationEasingType>()
{
	return ECameraAnimationEasingType_StaticEnum();
}
struct Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Circular.Name", "ECameraAnimationEasingType::Circular" },
		{ "Cubic.Name", "ECameraAnimationEasingType::Cubic" },
		{ "Exponential.Name", "ECameraAnimationEasingType::Exponential" },
		{ "Linear.Name", "ECameraAnimationEasingType::Linear" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "Quadratic.Name", "ECameraAnimationEasingType::Quadratic" },
		{ "Quartic.Name", "ECameraAnimationEasingType::Quartic" },
		{ "Quintic.Name", "ECameraAnimationEasingType::Quintic" },
		{ "Sinusoidal.Name", "ECameraAnimationEasingType::Sinusoidal" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ECameraAnimationEasingType::Linear", (int64)ECameraAnimationEasingType::Linear },
		{ "ECameraAnimationEasingType::Sinusoidal", (int64)ECameraAnimationEasingType::Sinusoidal },
		{ "ECameraAnimationEasingType::Quadratic", (int64)ECameraAnimationEasingType::Quadratic },
		{ "ECameraAnimationEasingType::Cubic", (int64)ECameraAnimationEasingType::Cubic },
		{ "ECameraAnimationEasingType::Quartic", (int64)ECameraAnimationEasingType::Quartic },
		{ "ECameraAnimationEasingType::Quintic", (int64)ECameraAnimationEasingType::Quintic },
		{ "ECameraAnimationEasingType::Exponential", (int64)ECameraAnimationEasingType::Exponential },
		{ "ECameraAnimationEasingType::Circular", (int64)ECameraAnimationEasingType::Circular },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	"ECameraAnimationEasingType",
	"ECameraAnimationEasingType",
	Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType()
{
	if (!Z_Registration_Info_UEnum_ECameraAnimationEasingType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ECameraAnimationEasingType.InnerSingleton, Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ECameraAnimationEasingType.InnerSingleton;
}
// End Enum ECameraAnimationEasingType

// Begin ScriptStruct FCameraAnimationParams
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_CameraAnimationParams;
class UScriptStruct* FCameraAnimationParams::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_CameraAnimationParams.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_CameraAnimationParams.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FCameraAnimationParams, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("CameraAnimationParams"));
	}
	return Z_Registration_Info_UScriptStruct_CameraAnimationParams.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FCameraAnimationParams>()
{
	return FCameraAnimationParams::StaticStruct();
}
struct Z_Construct_UScriptStruct_FCameraAnimationParams_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Comment", "/** Parameter struct for adding new camera animations to UCameraAnimationCameraModifier */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Parameter struct for adding new camera animations to UCameraAnimationCameraModifier" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlayRate_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Time scale for playing the new camera animation */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Time scale for playing the new camera animation" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Scale_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Global scale to use for the new camera animation */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Global scale to use for the new camera animation" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EaseInType_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Ease-in function type */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Ease-in function type" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EaseInDuration_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Ease-in duration in seconds */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Ease-in duration in seconds" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EaseOutType_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Ease-out function type */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Ease-out function type" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EaseOutDuration_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Ease-out duration in seconds */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Ease-out duration in seconds" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bLoop_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Whether the camera animation should loop */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Whether the camera animation should loop" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StartOffset_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Offset, in frames, into the animation to start at */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Offset, in frames, into the animation to start at" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bRandomStartTime_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Whether the camera animation should have a random start time */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Whether the camera animation should have a random start time" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DurationOverride_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** Override the duration of the animation with a new duration (including blends) */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Override the duration of the animation with a new duration (including blends)" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlaySpace_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** The transform space to use for the new camera shake */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "The transform space to use for the new camera shake" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_UserPlaySpaceRot_MetaData[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** User space to use when PlaySpace is UserDefined */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "User space to use when PlaySpace is UserDefined" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_PlayRate;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Scale;
	static const UECodeGen_Private::FBytePropertyParams NewProp_EaseInType_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_EaseInType;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_EaseInDuration;
	static const UECodeGen_Private::FBytePropertyParams NewProp_EaseOutType_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_EaseOutType;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_EaseOutDuration;
	static void NewProp_bLoop_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bLoop;
	static const UECodeGen_Private::FIntPropertyParams NewProp_StartOffset;
	static void NewProp_bRandomStartTime_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bRandomStartTime;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DurationOverride;
	static const UECodeGen_Private::FBytePropertyParams NewProp_PlaySpace_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_PlaySpace;
	static const UECodeGen_Private::FStructPropertyParams NewProp_UserPlaySpaceRot;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FCameraAnimationParams>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_PlayRate = { "PlayRate", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, PlayRate), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlayRate_MetaData), NewProp_PlayRate_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_Scale = { "Scale", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, Scale), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Scale_MetaData), NewProp_Scale_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseInType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseInType = { "EaseInType", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, EaseInType), Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EaseInType_MetaData), NewProp_EaseInType_MetaData) }; // 942733231
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseInDuration = { "EaseInDuration", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, EaseInDuration), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EaseInDuration_MetaData), NewProp_EaseInDuration_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseOutType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseOutType = { "EaseOutType", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, EaseOutType), Z_Construct_UEnum_GameplayCameras_ECameraAnimationEasingType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EaseOutType_MetaData), NewProp_EaseOutType_MetaData) }; // 942733231
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseOutDuration = { "EaseOutDuration", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, EaseOutDuration), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EaseOutDuration_MetaData), NewProp_EaseOutDuration_MetaData) };
void Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bLoop_SetBit(void* Obj)
{
	((FCameraAnimationParams*)Obj)->bLoop = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bLoop = { "bLoop", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FCameraAnimationParams), &Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bLoop_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bLoop_MetaData), NewProp_bLoop_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_StartOffset = { "StartOffset", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, StartOffset), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StartOffset_MetaData), NewProp_StartOffset_MetaData) };
void Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bRandomStartTime_SetBit(void* Obj)
{
	((FCameraAnimationParams*)Obj)->bRandomStartTime = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bRandomStartTime = { "bRandomStartTime", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FCameraAnimationParams), &Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bRandomStartTime_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bRandomStartTime_MetaData), NewProp_bRandomStartTime_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_DurationOverride = { "DurationOverride", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, DurationOverride), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DurationOverride_MetaData), NewProp_DurationOverride_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_PlaySpace_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_PlaySpace = { "PlaySpace", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, PlaySpace), Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlaySpace_MetaData), NewProp_PlaySpace_MetaData) }; // 3192519144
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_UserPlaySpaceRot = { "UserPlaySpaceRot", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraAnimationParams, UserPlaySpaceRot), Z_Construct_UScriptStruct_FRotator, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_UserPlaySpaceRot_MetaData), NewProp_UserPlaySpaceRot_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_PlayRate,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_Scale,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseInType_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseInType,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseInDuration,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseOutType_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseOutType,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_EaseOutDuration,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bLoop,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_StartOffset,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_bRandomStartTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_DurationOverride,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_PlaySpace_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_PlaySpace,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewProp_UserPlaySpaceRot,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"CameraAnimationParams",
	Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::PropPointers),
	sizeof(FCameraAnimationParams),
	alignof(FCameraAnimationParams),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FCameraAnimationParams()
{
	if (!Z_Registration_Info_UScriptStruct_CameraAnimationParams.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_CameraAnimationParams.InnerSingleton, Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_CameraAnimationParams.InnerSingleton;
}
// End ScriptStruct FCameraAnimationParams

// Begin ScriptStruct FCameraAnimationHandle
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_CameraAnimationHandle;
class UScriptStruct* FCameraAnimationHandle::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_CameraAnimationHandle.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_CameraAnimationHandle.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FCameraAnimationHandle, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("CameraAnimationHandle"));
	}
	return Z_Registration_Info_UScriptStruct_CameraAnimationHandle.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FCameraAnimationHandle>()
{
	return FCameraAnimationHandle::StaticStruct();
}
struct Z_Construct_UScriptStruct_FCameraAnimationHandle_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Comment", "/** A handle to a camera animation running in UCameraAnimationCameraModifier */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "A handle to a camera animation running in UCameraAnimationCameraModifier" },
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FCameraAnimationHandle>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FCameraAnimationHandle_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"CameraAnimationHandle",
	nullptr,
	0,
	sizeof(FCameraAnimationHandle),
	alignof(FCameraAnimationHandle),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraAnimationHandle_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FCameraAnimationHandle_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FCameraAnimationHandle()
{
	if (!Z_Registration_Info_UScriptStruct_CameraAnimationHandle.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_CameraAnimationHandle.InnerSingleton, Z_Construct_UScriptStruct_FCameraAnimationHandle_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_CameraAnimationHandle.InnerSingleton;
}
// End ScriptStruct FCameraAnimationHandle

// Begin ScriptStruct FActiveCameraAnimationInfo
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo;
class UScriptStruct* FActiveCameraAnimationInfo::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FActiveCameraAnimationInfo, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("ActiveCameraAnimationInfo"));
	}
	return Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FActiveCameraAnimationInfo>()
{
	return FActiveCameraAnimationInfo::StaticStruct();
}
struct Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "Comment", "/**\n * Information about an active camera animation inside UCameraAnimationCameraModifier.\n */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Information about an active camera animation inside UCameraAnimationCameraModifier." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Sequence_MetaData[] = {
		{ "Comment", "/** The sequence to use for the animation. */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "The sequence to use for the animation." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Params_MetaData[] = {
		{ "Comment", "/** The parameters for playing the animation. */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "The parameters for playing the animation." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[] = {
		{ "Comment", "/** A reference handle for use with UCameraAnimationCameraModifier. */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "A reference handle for use with UCameraAnimationCameraModifier." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Player_MetaData[] = {
		{ "Comment", "/** The player for playing the animation. */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "The player for playing the animation." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CameraStandIn_MetaData[] = {
		{ "Comment", "/** Standin for the camera actor and components */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Standin for the camera actor and components" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EaseInCurrentTime_MetaData[] = {
		{ "Comment", "/** Current time into easing in */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Current time into easing in" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EaseOutCurrentTime_MetaData[] = {
		{ "Comment", "/** Current time into easing out */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Current time into easing out" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsEasingIn_MetaData[] = {
		{ "Comment", "/** Whether easing in is ongoing */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Whether easing in is ongoing" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsEasingOut_MetaData[] = {
		{ "Comment", "/** Whether easing out is ongoing */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Whether easing out is ongoing" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPtrPropertyParams NewProp_Sequence;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Params;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Handle;
	static const UECodeGen_Private::FObjectPtrPropertyParams NewProp_Player;
	static const UECodeGen_Private::FObjectPtrPropertyParams NewProp_CameraStandIn;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_EaseInCurrentTime;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_EaseOutCurrentTime;
	static void NewProp_bIsEasingIn_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsEasingIn;
	static void NewProp_bIsEasingOut_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsEasingOut;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FActiveCameraAnimationInfo>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FObjectPtrPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Sequence = { "Sequence", nullptr, (EPropertyFlags)0x0014000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, Sequence), Z_Construct_UClass_UCameraAnimationSequence_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Sequence_MetaData), NewProp_Sequence_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Params = { "Params", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, Params), Z_Construct_UScriptStruct_FCameraAnimationParams, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Params_MetaData), NewProp_Params_MetaData) }; // 3020326929
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, Handle), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Handle_MetaData), NewProp_Handle_MetaData) }; // 424093393
const UECodeGen_Private::FObjectPtrPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Player = { "Player", nullptr, (EPropertyFlags)0x0014000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, Player), Z_Construct_UClass_UCameraAnimationSequencePlayer_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Player_MetaData), NewProp_Player_MetaData) };
const UECodeGen_Private::FObjectPtrPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_CameraStandIn = { "CameraStandIn", nullptr, (EPropertyFlags)0x0014000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, CameraStandIn), Z_Construct_UClass_UCameraAnimationSequenceCameraStandIn_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CameraStandIn_MetaData), NewProp_CameraStandIn_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_EaseInCurrentTime = { "EaseInCurrentTime", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, EaseInCurrentTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EaseInCurrentTime_MetaData), NewProp_EaseInCurrentTime_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_EaseOutCurrentTime = { "EaseOutCurrentTime", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FActiveCameraAnimationInfo, EaseOutCurrentTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EaseOutCurrentTime_MetaData), NewProp_EaseOutCurrentTime_MetaData) };
void Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingIn_SetBit(void* Obj)
{
	((FActiveCameraAnimationInfo*)Obj)->bIsEasingIn = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingIn = { "bIsEasingIn", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FActiveCameraAnimationInfo), &Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingIn_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsEasingIn_MetaData), NewProp_bIsEasingIn_MetaData) };
void Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingOut_SetBit(void* Obj)
{
	((FActiveCameraAnimationInfo*)Obj)->bIsEasingOut = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingOut = { "bIsEasingOut", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FActiveCameraAnimationInfo), &Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingOut_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsEasingOut_MetaData), NewProp_bIsEasingOut_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Sequence,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Params,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Handle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_Player,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_CameraStandIn,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_EaseInCurrentTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_EaseOutCurrentTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingIn,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewProp_bIsEasingOut,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"ActiveCameraAnimationInfo",
	Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::PropPointers),
	sizeof(FActiveCameraAnimationInfo),
	alignof(FActiveCameraAnimationInfo),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FActiveCameraAnimationInfo()
{
	if (!Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo.InnerSingleton, Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo.InnerSingleton;
}
// End ScriptStruct FActiveCameraAnimationInfo

// Begin Class UCameraAnimationCameraModifier Function GetCameraAnimationCameraModifier
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics
{
	struct CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifier_Parms
	{
		const UObject* WorldContextObject;
		int32 PlayerIndex;
		UCameraAnimationCameraModifier* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "WorldContext", "WorldContextObject" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WorldContextObject_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WorldContextObject;
	static const UECodeGen_Private::FIntPropertyParams NewProp_PlayerIndex;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::NewProp_WorldContextObject = { "WorldContextObject", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifier_Parms, WorldContextObject), Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WorldContextObject_MetaData), NewProp_WorldContextObject_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::NewProp_PlayerIndex = { "PlayerIndex", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifier_Parms, PlayerIndex), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifier_Parms, ReturnValue), Z_Construct_UClass_UCameraAnimationCameraModifier_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::NewProp_WorldContextObject,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::NewProp_PlayerIndex,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "GetCameraAnimationCameraModifier", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifier_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execGetCameraAnimationCameraModifier)
{
	P_GET_OBJECT(UObject,Z_Param_WorldContextObject);
	P_GET_PROPERTY(FIntProperty,Z_Param_PlayerIndex);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UCameraAnimationCameraModifier**)Z_Param__Result=UCameraAnimationCameraModifier::GetCameraAnimationCameraModifier(Z_Param_WorldContextObject,Z_Param_PlayerIndex);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function GetCameraAnimationCameraModifier

// Begin Class UCameraAnimationCameraModifier Function GetCameraAnimationCameraModifierFromID
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics
{
	struct CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromID_Parms
	{
		const UObject* WorldContextObject;
		int32 ControllerID;
		UCameraAnimationCameraModifier* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "WorldContext", "WorldContextObject" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WorldContextObject_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WorldContextObject;
	static const UECodeGen_Private::FIntPropertyParams NewProp_ControllerID;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::NewProp_WorldContextObject = { "WorldContextObject", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromID_Parms, WorldContextObject), Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WorldContextObject_MetaData), NewProp_WorldContextObject_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::NewProp_ControllerID = { "ControllerID", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromID_Parms, ControllerID), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromID_Parms, ReturnValue), Z_Construct_UClass_UCameraAnimationCameraModifier_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::NewProp_WorldContextObject,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::NewProp_ControllerID,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "GetCameraAnimationCameraModifierFromID", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromID_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromID_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execGetCameraAnimationCameraModifierFromID)
{
	P_GET_OBJECT(UObject,Z_Param_WorldContextObject);
	P_GET_PROPERTY(FIntProperty,Z_Param_ControllerID);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UCameraAnimationCameraModifier**)Z_Param__Result=UCameraAnimationCameraModifier::GetCameraAnimationCameraModifierFromID(Z_Param_WorldContextObject,Z_Param_ControllerID);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function GetCameraAnimationCameraModifierFromID

// Begin Class UCameraAnimationCameraModifier Function GetCameraAnimationCameraModifierFromPlayerController
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics
{
	struct CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromPlayerController_Parms
	{
		const APlayerController* PlayerController;
		UCameraAnimationCameraModifier* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PlayerController_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerController;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::NewProp_PlayerController = { "PlayerController", nullptr, (EPropertyFlags)0x0010000000000082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromPlayerController_Parms, PlayerController), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PlayerController_MetaData), NewProp_PlayerController_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromPlayerController_Parms, ReturnValue), Z_Construct_UClass_UCameraAnimationCameraModifier_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::NewProp_PlayerController,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "GetCameraAnimationCameraModifierFromPlayerController", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromPlayerController_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::CameraAnimationCameraModifier_eventGetCameraAnimationCameraModifierFromPlayerController_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execGetCameraAnimationCameraModifierFromPlayerController)
{
	P_GET_OBJECT(APlayerController,Z_Param_PlayerController);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UCameraAnimationCameraModifier**)Z_Param__Result=UCameraAnimationCameraModifier::GetCameraAnimationCameraModifierFromPlayerController(Z_Param_PlayerController);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function GetCameraAnimationCameraModifierFromPlayerController

// Begin Class UCameraAnimationCameraModifier Function IsCameraAnimationActive
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics
{
	struct CameraAnimationCameraModifier_eventIsCameraAnimationActive_Parms
	{
		FCameraAnimationHandle Handle;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Returns whether the given camera animation is playing.\n\x09 * @param Handle\x09\x09""A handle to a previously started camera animation.\n\x09 * @return\x09\x09\x09\x09Whether the corresponding camera animation is playing or not.\n\x09 */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Returns whether the given camera animation is playing.\n@param Handle                A handle to a previously started camera animation.\n@return                              Whether the corresponding camera animation is playing or not." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_Handle;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventIsCameraAnimationActive_Parms, Handle), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Handle_MetaData), NewProp_Handle_MetaData) }; // 424093393
void Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((CameraAnimationCameraModifier_eventIsCameraAnimationActive_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CameraAnimationCameraModifier_eventIsCameraAnimationActive_Parms), &Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::NewProp_Handle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "IsCameraAnimationActive", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::CameraAnimationCameraModifier_eventIsCameraAnimationActive_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::CameraAnimationCameraModifier_eventIsCameraAnimationActive_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execIsCameraAnimationActive)
{
	P_GET_STRUCT_REF(FCameraAnimationHandle,Z_Param_Out_Handle);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->IsCameraAnimationActive(Z_Param_Out_Handle);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function IsCameraAnimationActive

// Begin Class UCameraAnimationCameraModifier Function PlayCameraAnimation
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics
{
	struct CameraAnimationCameraModifier_eventPlayCameraAnimation_Parms
	{
		UCameraAnimationSequence* Sequence;
		FCameraAnimationParams Params;
		FCameraAnimationHandle ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Play a new camera animation sequence.\n\x09 * @param Sequence\x09\x09The sequence to use for the new camera animation.\n\x09 * @param Params\x09\x09The parameters for the new camera animation instance.\n\x09 */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Play a new camera animation sequence.\n@param Sequence              The sequence to use for the new camera animation.\n@param Params                The parameters for the new camera animation instance." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Sequence;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Params;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::NewProp_Sequence = { "Sequence", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventPlayCameraAnimation_Parms, Sequence), Z_Construct_UClass_UCameraAnimationSequence_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::NewProp_Params = { "Params", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventPlayCameraAnimation_Parms, Params), Z_Construct_UScriptStruct_FCameraAnimationParams, METADATA_PARAMS(0, nullptr) }; // 3020326929
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventPlayCameraAnimation_Parms, ReturnValue), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(0, nullptr) }; // 424093393
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::NewProp_Sequence,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::NewProp_Params,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "PlayCameraAnimation", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::CameraAnimationCameraModifier_eventPlayCameraAnimation_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::CameraAnimationCameraModifier_eventPlayCameraAnimation_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execPlayCameraAnimation)
{
	P_GET_OBJECT(UCameraAnimationSequence,Z_Param_Sequence);
	P_GET_STRUCT(FCameraAnimationParams,Z_Param_Params);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FCameraAnimationHandle*)Z_Param__Result=P_THIS->PlayCameraAnimation(Z_Param_Sequence,Z_Param_Params);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function PlayCameraAnimation

// Begin Class UCameraAnimationCameraModifier Function StopAllCameraAnimations
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics
{
	struct CameraAnimationCameraModifier_eventStopAllCameraAnimations_Parms
	{
		bool bImmediate;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Stop all camera animation instances.\n\x09 * @param bImmediate\x09True to stop it right now and ignore blend out, false to let it blend out as indicated.\n\x09 */" },
		{ "CPP_Default_bImmediate", "false" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Stop all camera animation instances.\n@param bImmediate    True to stop it right now and ignore blend out, false to let it blend out as indicated." },
	};
#endif // WITH_METADATA
	static void NewProp_bImmediate_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bImmediate;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
void Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::NewProp_bImmediate_SetBit(void* Obj)
{
	((CameraAnimationCameraModifier_eventStopAllCameraAnimations_Parms*)Obj)->bImmediate = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::NewProp_bImmediate = { "bImmediate", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CameraAnimationCameraModifier_eventStopAllCameraAnimations_Parms), &Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::NewProp_bImmediate_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::NewProp_bImmediate,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "StopAllCameraAnimations", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::CameraAnimationCameraModifier_eventStopAllCameraAnimations_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020400, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::CameraAnimationCameraModifier_eventStopAllCameraAnimations_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execStopAllCameraAnimations)
{
	P_GET_UBOOL(Z_Param_bImmediate);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopAllCameraAnimations(Z_Param_bImmediate);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function StopAllCameraAnimations

// Begin Class UCameraAnimationCameraModifier Function StopAllCameraAnimationsOf
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics
{
	struct CameraAnimationCameraModifier_eventStopAllCameraAnimationsOf_Parms
	{
		UCameraAnimationSequence* Sequence;
		bool bImmediate;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Stop playing all instances of the given camera animation sequence.\n\x09 * @param Sequence\x09\x09The sequence of which to stop all instances.\n\x09 * @param bImmediate\x09True to stop it right now and ignore blend out, false to let it blend out as indicated.\n\x09 */" },
		{ "CPP_Default_bImmediate", "false" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Stop playing all instances of the given camera animation sequence.\n@param Sequence              The sequence of which to stop all instances.\n@param bImmediate    True to stop it right now and ignore blend out, false to let it blend out as indicated." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Sequence;
	static void NewProp_bImmediate_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bImmediate;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::NewProp_Sequence = { "Sequence", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventStopAllCameraAnimationsOf_Parms, Sequence), Z_Construct_UClass_UCameraAnimationSequence_NoRegister, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate_SetBit(void* Obj)
{
	((CameraAnimationCameraModifier_eventStopAllCameraAnimationsOf_Parms*)Obj)->bImmediate = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate = { "bImmediate", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CameraAnimationCameraModifier_eventStopAllCameraAnimationsOf_Parms), &Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::NewProp_Sequence,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "StopAllCameraAnimationsOf", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::CameraAnimationCameraModifier_eventStopAllCameraAnimationsOf_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::CameraAnimationCameraModifier_eventStopAllCameraAnimationsOf_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execStopAllCameraAnimationsOf)
{
	P_GET_OBJECT(UCameraAnimationSequence,Z_Param_Sequence);
	P_GET_UBOOL(Z_Param_bImmediate);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopAllCameraAnimationsOf(Z_Param_Sequence,Z_Param_bImmediate);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function StopAllCameraAnimationsOf

// Begin Class UCameraAnimationCameraModifier Function StopCameraAnimation
struct Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics
{
	struct CameraAnimationCameraModifier_eventStopCameraAnimation_Parms
	{
		FCameraAnimationHandle Handle;
		bool bImmediate;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** \n\x09 * Stops the given camera animation instance.\n\x09 * @param Hanlde\x09\x09""A handle to a previously started camera animation.\n\x09 * @param bImmediate\x09True to stop it right now and ignore blend out, false to let it blend out as indicated.\n\x09 */" },
		{ "CPP_Default_bImmediate", "false" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Stops the given camera animation instance.\n@param Hanlde                A handle to a previously started camera animation.\n@param bImmediate    True to stop it right now and ignore blend out, false to let it blend out as indicated." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_Handle;
	static void NewProp_bImmediate_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bImmediate;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CameraAnimationCameraModifier_eventStopCameraAnimation_Parms, Handle), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Handle_MetaData), NewProp_Handle_MetaData) }; // 424093393
void Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::NewProp_bImmediate_SetBit(void* Obj)
{
	((CameraAnimationCameraModifier_eventStopCameraAnimation_Parms*)Obj)->bImmediate = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::NewProp_bImmediate = { "bImmediate", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(CameraAnimationCameraModifier_eventStopCameraAnimation_Parms), &Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::NewProp_bImmediate_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::NewProp_Handle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::NewProp_bImmediate,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UCameraAnimationCameraModifier, nullptr, "StopCameraAnimation", nullptr, nullptr, Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::PropPointers), sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::CameraAnimationCameraModifier_eventStopCameraAnimation_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::CameraAnimationCameraModifier_eventStopCameraAnimation_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCameraAnimationCameraModifier::execStopCameraAnimation)
{
	P_GET_STRUCT_REF(FCameraAnimationHandle,Z_Param_Out_Handle);
	P_GET_UBOOL(Z_Param_bImmediate);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopCameraAnimation(Z_Param_Out_Handle,Z_Param_bImmediate);
	P_NATIVE_END;
}
// End Class UCameraAnimationCameraModifier Function StopCameraAnimation

// Begin Class UCameraAnimationCameraModifier
void UCameraAnimationCameraModifier::StaticRegisterNativesUCameraAnimationCameraModifier()
{
	UClass* Class = UCameraAnimationCameraModifier::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "GetCameraAnimationCameraModifier", &UCameraAnimationCameraModifier::execGetCameraAnimationCameraModifier },
		{ "GetCameraAnimationCameraModifierFromID", &UCameraAnimationCameraModifier::execGetCameraAnimationCameraModifierFromID },
		{ "GetCameraAnimationCameraModifierFromPlayerController", &UCameraAnimationCameraModifier::execGetCameraAnimationCameraModifierFromPlayerController },
		{ "IsCameraAnimationActive", &UCameraAnimationCameraModifier::execIsCameraAnimationActive },
		{ "PlayCameraAnimation", &UCameraAnimationCameraModifier::execPlayCameraAnimation },
		{ "StopAllCameraAnimations", &UCameraAnimationCameraModifier::execStopAllCameraAnimations },
		{ "StopAllCameraAnimationsOf", &UCameraAnimationCameraModifier::execStopAllCameraAnimationsOf },
		{ "StopCameraAnimation", &UCameraAnimationCameraModifier::execStopCameraAnimation },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCameraAnimationCameraModifier);
UClass* Z_Construct_UClass_UCameraAnimationCameraModifier_NoRegister()
{
	return UCameraAnimationCameraModifier::StaticClass();
}
struct Z_Construct_UClass_UCameraAnimationCameraModifier_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * A camera modifier that plays camera animation sequences.\n */" },
		{ "IncludePath", "CameraAnimationCameraModifier.h" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "A camera modifier that plays camera animation sequences." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ActiveAnimations_MetaData[] = {
		{ "Comment", "/** List of active camera animation instances */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "List of active camera animation instances" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NextInstanceSerialNumber_MetaData[] = {
		{ "Comment", "/** Next serial number to use for a camera animation instance */" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Next serial number to use for a camera animation instance" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_ActiveAnimations_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ActiveAnimations;
	static const UECodeGen_Private::FUInt16PropertyParams NewProp_NextInstanceSerialNumber;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifier, "GetCameraAnimationCameraModifier" }, // 941707073
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromID, "GetCameraAnimationCameraModifierFromID" }, // 855676930
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_GetCameraAnimationCameraModifierFromPlayerController, "GetCameraAnimationCameraModifierFromPlayerController" }, // 2379592626
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_IsCameraAnimationActive, "IsCameraAnimationActive" }, // 2110785117
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_PlayCameraAnimation, "PlayCameraAnimation" }, // 3247959511
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimations, "StopAllCameraAnimations" }, // 857218620
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_StopAllCameraAnimationsOf, "StopAllCameraAnimationsOf" }, // 3884194620
		{ &Z_Construct_UFunction_UCameraAnimationCameraModifier_StopCameraAnimation, "StopCameraAnimation" }, // 1373851361
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCameraAnimationCameraModifier>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::NewProp_ActiveAnimations_Inner = { "ActiveAnimations", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FActiveCameraAnimationInfo, METADATA_PARAMS(0, nullptr) }; // 595632790
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::NewProp_ActiveAnimations = { "ActiveAnimations", nullptr, (EPropertyFlags)0x0020080000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraAnimationCameraModifier, ActiveAnimations), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ActiveAnimations_MetaData), NewProp_ActiveAnimations_MetaData) }; // 595632790
const UECodeGen_Private::FUInt16PropertyParams Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::NewProp_NextInstanceSerialNumber = { "NextInstanceSerialNumber", nullptr, (EPropertyFlags)0x0020080000000000, UECodeGen_Private::EPropertyGenFlags::UInt16, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCameraAnimationCameraModifier, NextInstanceSerialNumber), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NextInstanceSerialNumber_MetaData), NewProp_NextInstanceSerialNumber_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::NewProp_ActiveAnimations_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::NewProp_ActiveAnimations,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::NewProp_NextInstanceSerialNumber,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UCameraModifier,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::ClassParams = {
	&UCameraAnimationCameraModifier::StaticClass,
	"Camera",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::Class_MetaDataParams), Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UCameraAnimationCameraModifier()
{
	if (!Z_Registration_Info_UClass_UCameraAnimationCameraModifier.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCameraAnimationCameraModifier.OuterSingleton, Z_Construct_UClass_UCameraAnimationCameraModifier_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCameraAnimationCameraModifier.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UCameraAnimationCameraModifier>()
{
	return UCameraAnimationCameraModifier::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UCameraAnimationCameraModifier);
UCameraAnimationCameraModifier::~UCameraAnimationCameraModifier() {}
// End Class UCameraAnimationCameraModifier

// Begin Class UGameplayCamerasFunctionLibrary Function Conv_CameraAnimationCameraModifier
struct Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics
{
	struct GameplayCamerasFunctionLibrary_eventConv_CameraAnimationCameraModifier_Parms
	{
		APlayerCameraManager* PlayerCameraManager;
		UCameraAnimationCameraModifier* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintAutocast", "" },
		{ "Category", "Camera Animation" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerCameraManager;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::NewProp_PlayerCameraManager = { "PlayerCameraManager", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasFunctionLibrary_eventConv_CameraAnimationCameraModifier_Parms, PlayerCameraManager), Z_Construct_UClass_APlayerCameraManager_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasFunctionLibrary_eventConv_CameraAnimationCameraModifier_Parms, ReturnValue), Z_Construct_UClass_UCameraAnimationCameraModifier_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::NewProp_PlayerCameraManager,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasFunctionLibrary, nullptr, "Conv_CameraAnimationCameraModifier", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::GameplayCamerasFunctionLibrary_eventConv_CameraAnimationCameraModifier_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::GameplayCamerasFunctionLibrary_eventConv_CameraAnimationCameraModifier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasFunctionLibrary::execConv_CameraAnimationCameraModifier)
{
	P_GET_OBJECT(APlayerCameraManager,Z_Param_PlayerCameraManager);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UCameraAnimationCameraModifier**)Z_Param__Result=UGameplayCamerasFunctionLibrary::Conv_CameraAnimationCameraModifier(Z_Param_PlayerCameraManager);
	P_NATIVE_END;
}
// End Class UGameplayCamerasFunctionLibrary Function Conv_CameraAnimationCameraModifier

// Begin Class UGameplayCamerasFunctionLibrary Function Conv_CameraAnimationPlaySpace
struct Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics
{
	struct GameplayCamerasFunctionLibrary_eventConv_CameraAnimationPlaySpace_Parms
	{
		ECameraShakePlaySpace CameraShakePlaySpace;
		ECameraAnimationPlaySpace ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintAutocast", "" },
		{ "Category", "Camera Animation" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FBytePropertyParams NewProp_CameraShakePlaySpace_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_CameraShakePlaySpace;
	static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_CameraShakePlaySpace_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_CameraShakePlaySpace = { "CameraShakePlaySpace", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasFunctionLibrary_eventConv_CameraAnimationPlaySpace_Parms, CameraShakePlaySpace), Z_Construct_UEnum_Engine_ECameraShakePlaySpace, METADATA_PARAMS(0, nullptr) }; // 2661961121
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasFunctionLibrary_eventConv_CameraAnimationPlaySpace_Parms, ReturnValue), Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace, METADATA_PARAMS(0, nullptr) }; // 3192519144
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_CameraShakePlaySpace_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_CameraShakePlaySpace,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_ReturnValue_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasFunctionLibrary, nullptr, "Conv_CameraAnimationPlaySpace", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::GameplayCamerasFunctionLibrary_eventConv_CameraAnimationPlaySpace_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::GameplayCamerasFunctionLibrary_eventConv_CameraAnimationPlaySpace_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasFunctionLibrary::execConv_CameraAnimationPlaySpace)
{
	P_GET_ENUM(ECameraShakePlaySpace,Z_Param_CameraShakePlaySpace);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(ECameraAnimationPlaySpace*)Z_Param__Result=UGameplayCamerasFunctionLibrary::Conv_CameraAnimationPlaySpace(ECameraShakePlaySpace(Z_Param_CameraShakePlaySpace));
	P_NATIVE_END;
}
// End Class UGameplayCamerasFunctionLibrary Function Conv_CameraAnimationPlaySpace

// Begin Class UGameplayCamerasFunctionLibrary Function Conv_CameraShakePlaySpace
struct Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics
{
	struct GameplayCamerasFunctionLibrary_eventConv_CameraShakePlaySpace_Parms
	{
		ECameraAnimationPlaySpace CameraAnimationPlaySpace;
		ECameraShakePlaySpace ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "BlueprintAutocast", "" },
		{ "Category", "Camera Animation" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FBytePropertyParams NewProp_CameraAnimationPlaySpace_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_CameraAnimationPlaySpace;
	static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_CameraAnimationPlaySpace_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_CameraAnimationPlaySpace = { "CameraAnimationPlaySpace", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasFunctionLibrary_eventConv_CameraShakePlaySpace_Parms, CameraAnimationPlaySpace), Z_Construct_UEnum_GameplayCameras_ECameraAnimationPlaySpace, METADATA_PARAMS(0, nullptr) }; // 3192519144
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasFunctionLibrary_eventConv_CameraShakePlaySpace_Parms, ReturnValue), Z_Construct_UEnum_Engine_ECameraShakePlaySpace, METADATA_PARAMS(0, nullptr) }; // 2661961121
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_CameraAnimationPlaySpace_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_CameraAnimationPlaySpace,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_ReturnValue_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasFunctionLibrary, nullptr, "Conv_CameraShakePlaySpace", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::GameplayCamerasFunctionLibrary_eventConv_CameraShakePlaySpace_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::GameplayCamerasFunctionLibrary_eventConv_CameraShakePlaySpace_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasFunctionLibrary::execConv_CameraShakePlaySpace)
{
	P_GET_ENUM(ECameraAnimationPlaySpace,Z_Param_CameraAnimationPlaySpace);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(ECameraShakePlaySpace*)Z_Param__Result=UGameplayCamerasFunctionLibrary::Conv_CameraShakePlaySpace(ECameraAnimationPlaySpace(Z_Param_CameraAnimationPlaySpace));
	P_NATIVE_END;
}
// End Class UGameplayCamerasFunctionLibrary Function Conv_CameraShakePlaySpace

// Begin Class UGameplayCamerasFunctionLibrary
void UGameplayCamerasFunctionLibrary::StaticRegisterNativesUGameplayCamerasFunctionLibrary()
{
	UClass* Class = UGameplayCamerasFunctionLibrary::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "Conv_CameraAnimationCameraModifier", &UGameplayCamerasFunctionLibrary::execConv_CameraAnimationCameraModifier },
		{ "Conv_CameraAnimationPlaySpace", &UGameplayCamerasFunctionLibrary::execConv_CameraAnimationPlaySpace },
		{ "Conv_CameraShakePlaySpace", &UGameplayCamerasFunctionLibrary::execConv_CameraShakePlaySpace },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGameplayCamerasFunctionLibrary);
UClass* Z_Construct_UClass_UGameplayCamerasFunctionLibrary_NoRegister()
{
	return UGameplayCamerasFunctionLibrary::StaticClass();
}
struct Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Blueprint function library for autocasting a player camera manager into the camera animation camera modifier.\n * This prevents breaking Blueprints now that APlayerCameraManager::StartCameraShake returns the base class.\n */" },
		{ "IncludePath", "CameraAnimationCameraModifier.h" },
		{ "ModuleRelativePath", "Public/CameraAnimationCameraModifier.h" },
		{ "ToolTip", "Blueprint function library for autocasting a player camera manager into the camera animation camera modifier.\nThis prevents breaking Blueprints now that APlayerCameraManager::StartCameraShake returns the base class." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationCameraModifier, "Conv_CameraAnimationCameraModifier" }, // 2384375779
		{ &Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraAnimationPlaySpace, "Conv_CameraAnimationPlaySpace" }, // 3918073853
		{ &Z_Construct_UFunction_UGameplayCamerasFunctionLibrary_Conv_CameraShakePlaySpace, "Conv_CameraShakePlaySpace" }, // 1489706306
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCamerasFunctionLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics::ClassParams = {
	&UGameplayCamerasFunctionLibrary::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics::Class_MetaDataParams), Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGameplayCamerasFunctionLibrary()
{
	if (!Z_Registration_Info_UClass_UGameplayCamerasFunctionLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGameplayCamerasFunctionLibrary.OuterSingleton, Z_Construct_UClass_UGameplayCamerasFunctionLibrary_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGameplayCamerasFunctionLibrary.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UGameplayCamerasFunctionLibrary>()
{
	return UGameplayCamerasFunctionLibrary::StaticClass();
}
UGameplayCamerasFunctionLibrary::UGameplayCamerasFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCamerasFunctionLibrary);
UGameplayCamerasFunctionLibrary::~UGameplayCamerasFunctionLibrary() {}
// End Class UGameplayCamerasFunctionLibrary

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ ECameraAnimationPlaySpace_StaticEnum, TEXT("ECameraAnimationPlaySpace"), &Z_Registration_Info_UEnum_ECameraAnimationPlaySpace, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3192519144U) },
		{ ECameraAnimationEasingType_StaticEnum, TEXT("ECameraAnimationEasingType"), &Z_Registration_Info_UEnum_ECameraAnimationEasingType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 942733231U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FCameraAnimationParams::StaticStruct, Z_Construct_UScriptStruct_FCameraAnimationParams_Statics::NewStructOps, TEXT("CameraAnimationParams"), &Z_Registration_Info_UScriptStruct_CameraAnimationParams, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FCameraAnimationParams), 3020326929U) },
		{ FCameraAnimationHandle::StaticStruct, Z_Construct_UScriptStruct_FCameraAnimationHandle_Statics::NewStructOps, TEXT("CameraAnimationHandle"), &Z_Registration_Info_UScriptStruct_CameraAnimationHandle, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FCameraAnimationHandle), 424093393U) },
		{ FActiveCameraAnimationInfo::StaticStruct, Z_Construct_UScriptStruct_FActiveCameraAnimationInfo_Statics::NewStructOps, TEXT("ActiveCameraAnimationInfo"), &Z_Registration_Info_UScriptStruct_ActiveCameraAnimationInfo, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FActiveCameraAnimationInfo), 595632790U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCameraAnimationCameraModifier, UCameraAnimationCameraModifier::StaticClass, TEXT("UCameraAnimationCameraModifier"), &Z_Registration_Info_UClass_UCameraAnimationCameraModifier, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCameraAnimationCameraModifier), 1554688073U) },
		{ Z_Construct_UClass_UGameplayCamerasFunctionLibrary, UGameplayCamerasFunctionLibrary::StaticClass, TEXT("UGameplayCamerasFunctionLibrary"), &Z_Registration_Info_UClass_UGameplayCamerasFunctionLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGameplayCamerasFunctionLibrary), 725583407U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_705856003(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics::ScriptStructInfo),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_CameraAnimationCameraModifier_h_Statics::EnumInfo));
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
