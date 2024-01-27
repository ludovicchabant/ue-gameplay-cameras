// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../Plugins/Cameras/GameplayCameras/Source/GameplayCameras/Public/Core/CameraPose.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCameraPose() {}

// Begin Cross Module References
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FRotator3d();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector3d();
GAMEPLAYCAMERAS_API UScriptStruct* Z_Construct_UScriptStruct_FCameraPose();
UPackage* Z_Construct_UPackage__Script_GameplayCameras();
// End Cross Module References

// Begin ScriptStruct FCameraPose
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_CameraPose;
class UScriptStruct* FCameraPose::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_CameraPose.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_CameraPose.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FCameraPose, (UObject*)Z_Construct_UPackage__Script_GameplayCameras(), TEXT("CameraPose"));
	}
	return Z_Registration_Info_UScriptStruct_CameraPose.OuterSingleton;
}
template<> GAMEPLAYCAMERAS_API UScriptStruct* StaticStruct<FCameraPose>()
{
	return FCameraPose::StaticStruct();
}
struct Z_Construct_UScriptStruct_FCameraPose_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "Comment", "/**\n * Structure describing the state of a camera.\n *\n * Fields are private and can only be accessed via the getters and setters.\n * The ChangedFlags structure keeps track of which fields were changed via the setters.\n */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "Structure describing the state of a camera.\n\nFields are private and can only be accessed via the getters and setters.\nThe ChangedFlags structure keeps track of which fields were changed via the setters." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Location_MetaData[] = {
		{ "Comment", "/** The location of the camera in the world */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The location of the camera in the world" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Rotation_MetaData[] = {
		{ "Comment", "/** The rotation of the camera in the world */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The rotation of the camera in the world" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetDistance_MetaData[] = {
		{ "Comment", "/** Distance to the target */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "Distance to the target" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FieldOfView_MetaData[] = {
		{ "Comment", "/**\n\x09 * The horizontal field of view of the camera, in degrees \n\x09 * If zero or less, focal length is used instead\n\x09 */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The horizontal field of view of the camera, in degrees\nIf zero or less, focal length is used instead" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AspectRatio_MetaData[] = {
		{ "Comment", "/**\n\x09 * The aspect ratio of the camera\n\x09 * If zero or less, the sensor width and height are used instead\n\x09 */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The aspect ratio of the camera\nIf zero or less, the sensor width and height are used instead" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FocalLength_MetaData[] = {
		{ "Comment", "/**\n\x09 * The focal length of the camera's lens, in millimeters\n\x09 * If zero or less, field of view is used instead\n\x09 */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The focal length of the camera's lens, in millimeters\nIf zero or less, field of view is used instead" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Aperture_MetaData[] = {
		{ "Comment", "/** The aperture of the camera's lens, in f-stops */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The aperture of the camera's lens, in f-stops" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FocusDistance_MetaData[] = {
		{ "Comment", "/** The focus distance of the camera's lens, in world units */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The focus distance of the camera's lens, in world units" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SensorWidth_MetaData[] = {
		{ "Comment", "/** The width of the camera's sensor, in millimeters */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The width of the camera's sensor, in millimeters" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SensorHeight_MetaData[] = {
		{ "Comment", "/** The height of the camera's sensor, in millimeters */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The height of the camera's sensor, in millimeters" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SqueezeFactor_MetaData[] = {
		{ "Comment", "/** Squeeze factor for anamorphic lenses */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "Squeeze factor for anamorphic lenses" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NearClippingPlane_MetaData[] = {
		{ "Comment", "/** The distance to the near clipping plane, in world units */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The distance to the near clipping plane, in world units" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FarClippingPlane_MetaData[] = {
		{ "Comment", "/** The distance to the far clipping plane, in world units */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "The distance to the far clipping plane, in world units" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bConstrainAspectRatio_MetaData[] = {
		{ "Comment", "/** Whether to constrain aspect ratio */" },
		{ "ModuleRelativePath", "Public/Core/CameraPose.h" },
		{ "ToolTip", "Whether to constrain aspect ratio" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_Location;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Rotation;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_TargetDistance;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_FieldOfView;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_AspectRatio;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_FocalLength;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Aperture;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_FocusDistance;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SensorWidth;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SensorHeight;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SqueezeFactor;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_NearClippingPlane;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_FarClippingPlane;
	static void NewProp_bConstrainAspectRatio_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bConstrainAspectRatio;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FCameraPose>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_Location = { "Location", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, Location), Z_Construct_UScriptStruct_FVector3d, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Location_MetaData), NewProp_Location_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_Rotation = { "Rotation", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, Rotation), Z_Construct_UScriptStruct_FRotator3d, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Rotation_MetaData), NewProp_Rotation_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_TargetDistance = { "TargetDistance", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, TargetDistance), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetDistance_MetaData), NewProp_TargetDistance_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FieldOfView = { "FieldOfView", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, FieldOfView), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FieldOfView_MetaData), NewProp_FieldOfView_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_AspectRatio = { "AspectRatio", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, AspectRatio), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AspectRatio_MetaData), NewProp_AspectRatio_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FocalLength = { "FocalLength", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, FocalLength), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FocalLength_MetaData), NewProp_FocalLength_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_Aperture = { "Aperture", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, Aperture), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Aperture_MetaData), NewProp_Aperture_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FocusDistance = { "FocusDistance", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, FocusDistance), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FocusDistance_MetaData), NewProp_FocusDistance_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_SensorWidth = { "SensorWidth", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, SensorWidth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SensorWidth_MetaData), NewProp_SensorWidth_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_SensorHeight = { "SensorHeight", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, SensorHeight), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SensorHeight_MetaData), NewProp_SensorHeight_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_SqueezeFactor = { "SqueezeFactor", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, SqueezeFactor), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SqueezeFactor_MetaData), NewProp_SqueezeFactor_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_NearClippingPlane = { "NearClippingPlane", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, NearClippingPlane), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NearClippingPlane_MetaData), NewProp_NearClippingPlane_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FarClippingPlane = { "FarClippingPlane", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCameraPose, FarClippingPlane), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FarClippingPlane_MetaData), NewProp_FarClippingPlane_MetaData) };
void Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_bConstrainAspectRatio_SetBit(void* Obj)
{
	((FCameraPose*)Obj)->bConstrainAspectRatio = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_bConstrainAspectRatio = { "bConstrainAspectRatio", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FCameraPose), &Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_bConstrainAspectRatio_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bConstrainAspectRatio_MetaData), NewProp_bConstrainAspectRatio_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FCameraPose_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_Location,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_Rotation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_TargetDistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FieldOfView,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_AspectRatio,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FocalLength,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_Aperture,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FocusDistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_SensorWidth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_SensorHeight,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_SqueezeFactor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_NearClippingPlane,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_FarClippingPlane,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCameraPose_Statics::NewProp_bConstrainAspectRatio,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraPose_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FCameraPose_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayCameras,
	nullptr,
	&NewStructOps,
	"CameraPose",
	Z_Construct_UScriptStruct_FCameraPose_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraPose_Statics::PropPointers),
	sizeof(FCameraPose),
	alignof(FCameraPose),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCameraPose_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FCameraPose_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FCameraPose()
{
	if (!Z_Registration_Info_UScriptStruct_CameraPose.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_CameraPose.InnerSingleton, Z_Construct_UScriptStruct_FCameraPose_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_CameraPose.InnerSingleton;
}
// End ScriptStruct FCameraPose

// Begin Registration
struct Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraPose_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FCameraPose::StaticStruct, Z_Construct_UScriptStruct_FCameraPose_Statics::NewStructOps, TEXT("CameraPose"), &Z_Registration_Info_UScriptStruct_CameraPose, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FCameraPose), 3650097308U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraPose_h_3906811594(TEXT("/Script/GameplayCameras"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraPose_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Engine_Plugins_Cameras_GameplayCameras_Source_GameplayCameras_Public_Core_CameraPose_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
