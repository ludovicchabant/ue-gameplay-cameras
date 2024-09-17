// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Engine/EngineTypes.h"
#include "Math/MathFwd.h"
#include "Math/Transform.h"

#include "CameraPose.generated.h"

class FArchive;
struct FPostProcessSettings;
enum EAspectRatioAxisConstraint : int;

#define UE_CAMERA_POSE_FOR_TRANSFORM_PROPERTIES()\
	UE_CAMERA_POSE_FOR_PROPERTY(FVector, Location)\
	UE_CAMERA_POSE_FOR_PROPERTY(FRotator3d, Rotation)

#define UE_CAMERA_POSE_FOR_INTERPOLABLE_PROPERTIES()\
	UE_CAMERA_POSE_FOR_PROPERTY(double, TargetDistance)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  Aperture)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  ShutterSpeed)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  FocusDistance)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  SensorWidth)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  SensorHeight)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  ISO)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  SqueezeFactor)\
	UE_CAMERA_POSE_FOR_PROPERTY(int32,  DiaphragmBladeCount)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  PhysicalCameraBlendWeight)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  NearClippingPlane)\
	UE_CAMERA_POSE_FOR_PROPERTY(float,  FarClippingPlane)

#define UE_CAMERA_POSE_FOR_FOV_PROPERTIES()\
	UE_CAMERA_POSE_FOR_PROPERTY(float, FieldOfView)\
	UE_CAMERA_POSE_FOR_PROPERTY(float, FocalLength)

#define UE_CAMERA_POSE_FOR_FLIPPING_PROPERTIES()\
	UE_CAMERA_POSE_FOR_PROPERTY(bool, EnablePhysicalCamera)\
	UE_CAMERA_POSE_FOR_PROPERTY(bool, ConstrainAspectRatio)\
	UE_CAMERA_POSE_FOR_PROPERTY(bool, OverrideAspectRatioAxisConstraint)\
	UE_CAMERA_POSE_FOR_PROPERTY(EAspectRatioAxisConstraint, AspectRatioAxisConstraint)

#define UE_CAMERA_POSE_FOR_ALL_PROPERTIES()\
	UE_CAMERA_POSE_FOR_TRANSFORM_PROPERTIES()\
	UE_CAMERA_POSE_FOR_INTERPOLABLE_PROPERTIES()\
	UE_CAMERA_POSE_FOR_FOV_PROPERTIES()\
	UE_CAMERA_POSE_FOR_FLIPPING_PROPERTIES()

/**
 * Boolean flags for each of the properties inside FCameraPose.
 */
struct GAMEPLAYCAMERAS_API FCameraPoseFlags
{
#define UE_CAMERA_POSE_FOR_PROPERTY(PropType, PropName)\
	bool PropName = false;

UE_CAMERA_POSE_FOR_ALL_PROPERTIES()

#undef UE_CAMERA_POSE_FOR_PROPERTY

public:

	/** Returns a structure where all flags are set. */
	static const FCameraPoseFlags& All();

	/** Creates a new flags structure. */
	FCameraPoseFlags();
	/** Creates a new flags structure with all flags set to the given value. */
	FCameraPoseFlags(bool bInValue);

	/** Sets all flags to the given value. */
	FCameraPoseFlags& SetAllFlags(bool bInValue);
	/** Sets the flags that are set in OtherFlags, but checks that no flag is set on both structures. */
	FCameraPoseFlags& ExclusiveCombine(const FCameraPoseFlags& OtherFlags);

	/** Combines the flags with an AND logical operation. */
	FCameraPoseFlags& AND(const FCameraPoseFlags& OtherFlags);
	/** Combines the flags with an OR logical operation. */
	FCameraPoseFlags& OR(const FCameraPoseFlags& OtherFlags);
};

/**
 * Structure describing the state of a camera.
 *
 * Fields are private and can only be accessed via the getters and setters.
 * The ChangedFlags structure keeps track of which fields were changed via the setters.
 */
USTRUCT()
struct GAMEPLAYCAMERAS_API FCameraPose
{
	GENERATED_BODY()

public:

	FCameraPose();

	/** Resets this camera pose to its default values, with all changed flags off. */
	void Reset();

public:

	// Getters and setters

#define UE_CAMERA_POSE_FOR_PROPERTY(PropType, PropName)\
	PropType Get##PropName() const\
	{\
		return PropName;\
	}\
	void Set##PropName(TCallTraits<PropType>::ParamType InValue, bool bForceSet = false)\
	{\
		if (bForceSet || PropName != InValue)\
		{\
			ChangedFlags.PropName = true;\
			PropName = InValue;\
		}\
	}

UE_CAMERA_POSE_FOR_ALL_PROPERTIES()

#undef UE_CAMERA_POSE_FOR_PROPERTY

public:

	// Changed flags management

	/** Get the changed flags. */
	FCameraPoseFlags& GetChangedFlags() { return ChangedFlags; }
	/** Get the changed flags. */
	const FCameraPoseFlags& GetChangedFlags() const { return ChangedFlags; }

	/** Set the changed flags. */
	void SetChangedFlags(const FCameraPoseFlags& InChangedFlags) { ChangedFlags = InChangedFlags; }
	/** Set all fields as changed. */
	void SetAllChangedFlags();
	/** Set all fields as clean. */
	void ClearAllChangedFlags();

public:

	// Utility

	/** Gets the transform of the camera. */
	FTransform3d GetTransform() const;

	/** Sets the transform of the camera. */
	void SetTransform(FTransform3d Transform);

	/**
	 * Computes the horizontal field of view of the camera.
	 * The effective field of view can be driven by the FieldOfView property, or
	 * the FocalLength property in combination with the sensor size.
	 */
	double GetEffectiveFieldOfView() const;

	/** Gets the aspect ratio of the camera sensor. */
	double GetSensorAspectRatio() const;

	/** Gets the aiming ray of the camera. */
	FRay3d GetAimRay() const;

	/** Gets the aiming direction of the camera. */
	FVector3d GetAimDir() const;

	/** Gets the location of the camera's target. */
	FVector3d GetTarget() const;

	/** Gets the location of the camera's target given a specific distance. */
	FVector3d GetTarget(double InTargetDistance) const;

public:

	/** Computes the horizontal field of view of a camera. */
	static double GetEffectiveFieldOfView(float FocalLength, float FieldOfView, float SensorWidth, float SensorHeight, float SqueezeFactor);
	
	/** Computes the aspect ratio of a camera sensor. */
	static double GetSensorAspectRatio(float SensorWidth, float SensorHeight);

	/** Gets the default sensor size. */
	static void GetDefaultSensorSize(float& OutSensorWidth, float& OutSensorHeight);

	/**
	 * Applies the necessary post-process settings given the current values
	 * on this camera pose.
	 *
	 * This function doesn't do anything if EnablePhysicalCamera is false, or if
	 * PhysicalCameraBlendWeight is zero or less.
	 *
	 * @param PostProcessSettings  The post-process settings to modify
	 * @param bOverwriteSettings   Whether to overwrite values found to already be set
	 * @return  Whether post-process settings were created.
	 */
	bool ApplyPhysicalCameraSettings(FPostProcessSettings& PostProcessSettings, bool bOverwriteSettings = false) const;

public:

	// Interpolation
	
	/** Takes all properties from OtherPose and sets them on this camera pose. */
	void OverrideAll(const FCameraPose& OtherPose);
	/** Takes all changed properties from OtherPose and sets them on this camera pose. */
	void OverrideChanged(const FCameraPose& OtherPose);
	/** Interpolates all properties from ToPose using the given factor. */
	void LerpAll(const FCameraPose& ToPose, float Factor);
	/** Interpolates all changed properties from ToPose using the given factor. */
	void LerpChanged(const FCameraPose& ToPose, float Factor);
	/** Interpolates changed properties from ToPose using the given factor. Only properties defined by InMask are taken into account. */
	void LerpChanged(const FCameraPose& ToPose, float Factor, const FCameraPoseFlags& InMask, bool bInvertMask, FCameraPoseFlags& OutMask);

public:

	/** Serializes the given camera pose including the written-property flags. */
	static void SerializeWithFlags(FArchive& Ar, FCameraPose& CameraPose);

	/** Serializes this camera pose including the written-property flags. */
	void SerializeWithFlags(FArchive& Ar);

private:

	void InternalOverrideChanged(const FCameraPose& OtherPose, bool bChangedOnly);
	void InternalLerpChanged(const FCameraPose& ToPose, float Factor, const FCameraPoseFlags& InMask, bool bInvertMask, FCameraPoseFlags& OutMask, bool bChangedOnly);

private:

	/** The location of the camera in the world */
	UPROPERTY()
	FVector3d Location = {0, 0, 0};

	/** The rotation of the camera in the world */
	UPROPERTY()
	FRotator3d Rotation = {0, 0, 0};

	/** Distance to the target */
	UPROPERTY()
	double TargetDistance = 1000.0;

	/**
	 * The horizontal field of view of the camera, in degrees 
	 * If zero or less, focal length is used instead
	 */
	UPROPERTY()
	float FieldOfView = -1.f;  // Default to using a focal length

	/**
	 * The focal length of the camera's lens, in millimeters
	 * If zero or less, field of view is used instead
	 */
	UPROPERTY()
	float FocalLength = 35.f;

	/** The aperture of the camera's lens, in f-stops */
	UPROPERTY()
	float Aperture = 2.8f;

	/** The shutter speed of the camera's lens, in 1/seconds */
	UPROPERTY()
	float ShutterSpeed = 60.f;

	/** The focus distance of the camera's lens, in world units */
	UPROPERTY()
	float FocusDistance = -1.f;

	/** The width of the camera's sensor, in millimeters */
	UPROPERTY()
	float SensorWidth = 24.89f;

	/** The height of the camera's sensor, in millimeters */
	UPROPERTY()
	float SensorHeight = 18.67f;

	/** The camera sensor sensitivity in ISO. */
	UPROPERTY()
	float ISO = 100.f;

	/** Squeeze factor for anamorphic lenses */
	UPROPERTY()
	float SqueezeFactor = 1.f;

	/** Number of blades in the lens diaphragm */
	UPROPERTY()
	int32 DiaphragmBladeCount = 8;

	/** The distance to the near clipping plane, in world units */
	UPROPERTY()
	float NearClippingPlane = 10.f;

	/** The distance to the far clipping plane, in world units */
	UPROPERTY()
	float FarClippingPlane = -1.f;

	/** 
	 * An internal weight for the physical camera post-process settings, used when blending between 
	 * cameras with EnablePhysicalCamera enabled/disabled.
	 */
	UPROPERTY()
	float PhysicalCameraBlendWeight = 0.f;

	/** 
	 * Whether to setup post-process settings based on physical camera properties such as Aperture,
	 * FocusDistance, DiaphragmBladeCount, and so on.
	 */
	UPROPERTY()
	bool EnablePhysicalCamera = false;

	/** Whether to constrain aspect ratio */
	UPROPERTY()
	bool ConstrainAspectRatio = false;

	/** Whether to override the default aspect ratio axis constraint defined on the player controller */
	UPROPERTY()
	bool OverrideAspectRatioAxisConstraint = false;

	/** If ConstrainAspectRatio is false and OverrideAspectRatioAxisConstraint is true, how we should compute FieldOfView */
	UPROPERTY()
	TEnumAsByte<EAspectRatioAxisConstraint> AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MaintainYFOV;

private:
	
	/**
	 * Flags keeping track of which properties were written to since
	 * last time the flags were cleared.
	 */
	FCameraPoseFlags ChangedFlags;
};

