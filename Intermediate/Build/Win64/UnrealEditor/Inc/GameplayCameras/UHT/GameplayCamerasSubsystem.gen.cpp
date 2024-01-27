// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/GameplayCamerasSubsystem.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Animations/CameraAnimationCameraModifier.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCamerasSubsystem() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_APlayerController_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UWorldSubsystem();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCamerasSubsystem();
GAMEPLAYCAMERAS_API UClass* Z_Construct_UClass_UGameplayCamerasSubsystem_NoRegister();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraAnimationHandle();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraAnimationParams();
TEMPLATESEQUENCE_API UClass* Z_Construct_UClass_UCameraAnimationSequence_NoRegister();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin Class UGameplayCamerasSubsystem Function IsCameraAnimationActive
struct Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics
{
	struct GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms
	{
		APlayerController* PlayerController;
		FCameraAnimationHandle Handle;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Returns whether the given camera animation is playing.\n\x09 * @param PlayerController The player controller on which to play the animation.\n\x09 * @param Handle\x09\x09""A handle to a previously started camera animation.\n\x09 * @return\x09\x09\x09\x09Whether the corresponding camera animation is playing or not.\n\x09 */" },
		{ "ModuleRelativePath", "Public/GameplayCamerasSubsystem.h" },
		{ "ToolTip", "Returns whether the given camera animation is playing.\n@param PlayerController The player controller on which to play the animation.\n@param Handle                A handle to a previously started camera animation.\n@return                              Whether the corresponding camera animation is playing or not." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerController;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Handle;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_PlayerController = { "PlayerController", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms, PlayerController), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms, Handle), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Handle_MetaData), NewProp_Handle_MetaData) }; // 3211866552
void Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms), &Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_PlayerController,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_Handle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasSubsystem, nullptr, "IsCameraAnimationActive", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::GameplayCamerasSubsystem_eventIsCameraAnimationActive_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasSubsystem::execIsCameraAnimationActive)
{
	P_GET_OBJECT(APlayerController,Z_Param_PlayerController);
	P_GET_STRUCT_REF(FCameraAnimationHandle,Z_Param_Out_Handle);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->IsCameraAnimationActive(Z_Param_PlayerController,Z_Param_Out_Handle);
	P_NATIVE_END;
}
// End Class UGameplayCamerasSubsystem Function IsCameraAnimationActive

// Begin Class UGameplayCamerasSubsystem Function PlayCameraAnimation
struct Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics
{
	struct GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms
	{
		APlayerController* PlayerController;
		UCameraAnimationSequence* Sequence;
		FCameraAnimationParams Params;
		FCameraAnimationHandle ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Play a new camera animation sequence.\n\x09 * @param PlayerController The player controller on which to play the animation.\n\x09 * @param Sequence\x09\x09The sequence to use for the new camera animation.\n\x09 * @param Params\x09\x09The parameters for the new camera animation instance.\n\x09 */" },
		{ "ModuleRelativePath", "Public/GameplayCamerasSubsystem.h" },
		{ "ToolTip", "Play a new camera animation sequence.\n@param PlayerController The player controller on which to play the animation.\n@param Sequence              The sequence to use for the new camera animation.\n@param Params                The parameters for the new camera animation instance." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerController;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Sequence;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Params;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_PlayerController = { "PlayerController", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms, PlayerController), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_Sequence = { "Sequence", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms, Sequence), Z_Construct_UClass_UCameraAnimationSequence_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_Params = { "Params", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms, Params), Z_Construct_UScriptStruct_FCameraAnimationParams, METADATA_PARAMS(0, nullptr) }; // 3638752999
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms, ReturnValue), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(0, nullptr) }; // 3211866552
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_PlayerController,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_Sequence,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_Params,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasSubsystem, nullptr, "PlayCameraAnimation", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::GameplayCamerasSubsystem_eventPlayCameraAnimation_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasSubsystem::execPlayCameraAnimation)
{
	P_GET_OBJECT(APlayerController,Z_Param_PlayerController);
	P_GET_OBJECT(UCameraAnimationSequence,Z_Param_Sequence);
	P_GET_STRUCT(FCameraAnimationParams,Z_Param_Params);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FCameraAnimationHandle*)Z_Param__Result=P_THIS->PlayCameraAnimation(Z_Param_PlayerController,Z_Param_Sequence,Z_Param_Params);
	P_NATIVE_END;
}
// End Class UGameplayCamerasSubsystem Function PlayCameraAnimation

// Begin Class UGameplayCamerasSubsystem Function StopAllCameraAnimations
struct Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics
{
	struct GameplayCamerasSubsystem_eventStopAllCameraAnimations_Parms
	{
		APlayerController* PlayerController;
		bool bImmediate;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Stop all camera animation instances.\n\x09 * @param PlayerController The player controller on which to play the animation.\n\x09 * @param bImmediate\x09True to stop it right now and ignore blend out, false to let it blend out as indicated.\n\x09 */" },
		{ "CPP_Default_bImmediate", "false" },
		{ "ModuleRelativePath", "Public/GameplayCamerasSubsystem.h" },
		{ "ToolTip", "Stop all camera animation instances.\n@param PlayerController The player controller on which to play the animation.\n@param bImmediate    True to stop it right now and ignore blend out, false to let it blend out as indicated." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerController;
	static void NewProp_bImmediate_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bImmediate;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::NewProp_PlayerController = { "PlayerController", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventStopAllCameraAnimations_Parms, PlayerController), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::NewProp_bImmediate_SetBit(void* Obj)
{
	((GameplayCamerasSubsystem_eventStopAllCameraAnimations_Parms*)Obj)->bImmediate = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::NewProp_bImmediate = { "bImmediate", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(GameplayCamerasSubsystem_eventStopAllCameraAnimations_Parms), &Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::NewProp_bImmediate_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::NewProp_PlayerController,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::NewProp_bImmediate,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasSubsystem, nullptr, "StopAllCameraAnimations", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::GameplayCamerasSubsystem_eventStopAllCameraAnimations_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::GameplayCamerasSubsystem_eventStopAllCameraAnimations_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasSubsystem::execStopAllCameraAnimations)
{
	P_GET_OBJECT(APlayerController,Z_Param_PlayerController);
	P_GET_UBOOL(Z_Param_bImmediate);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopAllCameraAnimations(Z_Param_PlayerController,Z_Param_bImmediate);
	P_NATIVE_END;
}
// End Class UGameplayCamerasSubsystem Function StopAllCameraAnimations

// Begin Class UGameplayCamerasSubsystem Function StopAllCameraAnimationsOf
struct Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics
{
	struct GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms
	{
		APlayerController* PlayerController;
		UCameraAnimationSequence* Sequence;
		bool bImmediate;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/**\n\x09 * Stop playing all instances of the given camera animation sequence.\n\x09 * @param PlayerController The player controller on which to play the animation.\n\x09 * @param Sequence\x09\x09The sequence of which to stop all instances.\n\x09 * @param bImmediate\x09True to stop it right now and ignore blend out, false to let it blend out as indicated.\n\x09 */" },
		{ "CPP_Default_bImmediate", "false" },
		{ "ModuleRelativePath", "Public/GameplayCamerasSubsystem.h" },
		{ "ToolTip", "Stop playing all instances of the given camera animation sequence.\n@param PlayerController The player controller on which to play the animation.\n@param Sequence              The sequence of which to stop all instances.\n@param bImmediate    True to stop it right now and ignore blend out, false to let it blend out as indicated." },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerController;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Sequence;
	static void NewProp_bImmediate_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bImmediate;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_PlayerController = { "PlayerController", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms, PlayerController), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_Sequence = { "Sequence", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms, Sequence), Z_Construct_UClass_UCameraAnimationSequence_NoRegister, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate_SetBit(void* Obj)
{
	((GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms*)Obj)->bImmediate = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate = { "bImmediate", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms), &Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_PlayerController,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_Sequence,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::NewProp_bImmediate,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasSubsystem, nullptr, "StopAllCameraAnimationsOf", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::GameplayCamerasSubsystem_eventStopAllCameraAnimationsOf_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasSubsystem::execStopAllCameraAnimationsOf)
{
	P_GET_OBJECT(APlayerController,Z_Param_PlayerController);
	P_GET_OBJECT(UCameraAnimationSequence,Z_Param_Sequence);
	P_GET_UBOOL(Z_Param_bImmediate);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopAllCameraAnimationsOf(Z_Param_PlayerController,Z_Param_Sequence,Z_Param_bImmediate);
	P_NATIVE_END;
}
// End Class UGameplayCamerasSubsystem Function StopAllCameraAnimationsOf

// Begin Class UGameplayCamerasSubsystem Function StopCameraAnimation
struct Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics
{
	struct GameplayCamerasSubsystem_eventStopCameraAnimation_Parms
	{
		APlayerController* PlayerController;
		FCameraAnimationHandle Handle;
		bool bImmediate;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Camera Animation" },
		{ "Comment", "/** \n\x09 * Stops the given camera animation instance.\n\x09 * @param PlayerController The player controller on which to play the animation.\n\x09 * @param Hanlde\x09\x09""A handle to a previously started camera animation.\n\x09 * @param bImmediate\x09True to stop it right now and ignore blend out, false to let it blend out as indicated.\n\x09 */" },
		{ "CPP_Default_bImmediate", "false" },
		{ "ModuleRelativePath", "Public/GameplayCamerasSubsystem.h" },
		{ "ToolTip", "Stops the given camera animation instance.\n@param PlayerController The player controller on which to play the animation.\n@param Hanlde                A handle to a previously started camera animation.\n@param bImmediate    True to stop it right now and ignore blend out, false to let it blend out as indicated." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PlayerController;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Handle;
	static void NewProp_bImmediate_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bImmediate;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_PlayerController = { "PlayerController", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventStopCameraAnimation_Parms, PlayerController), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GameplayCamerasSubsystem_eventStopCameraAnimation_Parms, Handle), Z_Construct_UScriptStruct_FCameraAnimationHandle, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Handle_MetaData), NewProp_Handle_MetaData) }; // 3211866552
void Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_bImmediate_SetBit(void* Obj)
{
	((GameplayCamerasSubsystem_eventStopCameraAnimation_Parms*)Obj)->bImmediate = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_bImmediate = { "bImmediate", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(GameplayCamerasSubsystem_eventStopCameraAnimation_Parms), &Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_bImmediate_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_PlayerController,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_Handle,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::NewProp_bImmediate,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCamerasSubsystem, nullptr, "StopCameraAnimation", nullptr, nullptr, Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::PropPointers), sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::GameplayCamerasSubsystem_eventStopCameraAnimation_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::GameplayCamerasSubsystem_eventStopCameraAnimation_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGameplayCamerasSubsystem::execStopCameraAnimation)
{
	P_GET_OBJECT(APlayerController,Z_Param_PlayerController);
	P_GET_STRUCT_REF(FCameraAnimationHandle,Z_Param_Out_Handle);
	P_GET_UBOOL(Z_Param_bImmediate);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopCameraAnimation(Z_Param_PlayerController,Z_Param_Out_Handle,Z_Param_bImmediate);
	P_NATIVE_END;
}
// End Class UGameplayCamerasSubsystem Function StopCameraAnimation

// Begin Class UGameplayCamerasSubsystem
void UGameplayCamerasSubsystem::StaticRegisterNativesUGameplayCamerasSubsystem()
{
	UClass* Class = UGameplayCamerasSubsystem::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "IsCameraAnimationActive", &UGameplayCamerasSubsystem::execIsCameraAnimationActive },
		{ "PlayCameraAnimation", &UGameplayCamerasSubsystem::execPlayCameraAnimation },
		{ "StopAllCameraAnimations", &UGameplayCamerasSubsystem::execStopAllCameraAnimations },
		{ "StopAllCameraAnimationsOf", &UGameplayCamerasSubsystem::execStopAllCameraAnimationsOf },
		{ "StopCameraAnimation", &UGameplayCamerasSubsystem::execStopCameraAnimation },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGameplayCamerasSubsystem);
UClass* Z_Construct_UClass_UGameplayCamerasSubsystem_NoRegister()
{
	return UGameplayCamerasSubsystem::StaticClass();
}
struct Z_Construct_UClass_UGameplayCamerasSubsystem_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * World subsystem that holds global objects for handling camera animation sequences.\n */" },
		{ "IncludePath", "GameplayCamerasSubsystem.h" },
		{ "ModuleRelativePath", "Public/GameplayCamerasSubsystem.h" },
		{ "ToolTip", "World subsystem that holds global objects for handling camera animation sequences." },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayCamerasSubsystem_IsCameraAnimationActive, "IsCameraAnimationActive" }, // 827020466
		{ &Z_Construct_UFunction_UGameplayCamerasSubsystem_PlayCameraAnimation, "PlayCameraAnimation" }, // 4270234399
		{ &Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimations, "StopAllCameraAnimations" }, // 111965300
		{ &Z_Construct_UFunction_UGameplayCamerasSubsystem_StopAllCameraAnimationsOf, "StopAllCameraAnimationsOf" }, // 3103441063
		{ &Z_Construct_UFunction_UGameplayCamerasSubsystem_StopCameraAnimation, "StopCameraAnimation" }, // 3723892644
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCamerasSubsystem>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UGameplayCamerasSubsystem_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UWorldSubsystem,
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCamerasSubsystem_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCamerasSubsystem_Statics::ClassParams = {
	&UGameplayCamerasSubsystem::StaticClass,
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
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGameplayCamerasSubsystem_Statics::Class_MetaDataParams), Z_Construct_UClass_UGameplayCamerasSubsystem_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UGameplayCamerasSubsystem()
{
	if (!Z_Registration_Info_UClass_UGameplayCamerasSubsystem.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGameplayCamerasSubsystem.OuterSingleton, Z_Construct_UClass_UGameplayCamerasSubsystem_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGameplayCamerasSubsystem.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UClass* StaticClass<UGameplayCamerasSubsystem>()
{
	return UGameplayCamerasSubsystem::StaticClass();
}
UGameplayCamerasSubsystem::UGameplayCamerasSubsystem() {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCamerasSubsystem);
UGameplayCamerasSubsystem::~UGameplayCamerasSubsystem() {}
// End Class UGameplayCamerasSubsystem

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGameplayCamerasSubsystem, UGameplayCamerasSubsystem::StaticClass, TEXT("UGameplayCamerasSubsystem"), &Z_Registration_Info_UClass_UGameplayCamerasSubsystem, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGameplayCamerasSubsystem), 463912768U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_4270337269(TEXT("/Script/GameplayCameras"),
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_GameplayCamerasSubsystem_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
