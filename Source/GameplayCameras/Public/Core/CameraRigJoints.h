// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"

namespace UE::Cameras
{

/**
 * A structure describing a joint in a camera rig.
 */
struct GAMEPLAYCAMERAS_API FCameraRigJoint
{
	/** The variable driving the rotation of this joint. */
	FCameraVariableID VariableID;
	/** The position of the this joint. */
	FTransform3d Transform;
};

FArchive& operator<< (FArchive& Ar, FCameraRigJoint& RigJoint);

/**
 * A structure describing the joints of a camera rig.
 * These joints allow for "manipulating" the rig, e.g. to make it point
 * towards a desired target or direction.
 */
class GAMEPLAYCAMERAS_API FCameraRigJoints
{
public:

	/** Add a joint. */
	void AddJoint(const FCameraRigJoint& InJoint);
	/** Add a joint. */
	void AddJoint(const FCameraVariableDefinition& InVariableDefinition, const FTransform3d& InTransform);
	/** Add a joint related to the yaw/pitch built-in variable. */
	void AddYawPitchJoint(const FTransform3d& InTransform);

	/** Gets the joints. */
	TArrayView<const FCameraRigJoint> GetJoints() const { return Joints; }

	/** Removes all previously added joints. */
	void Reset();

	void Serialize(FArchive& Ar);

public:

	/** Override the joints with another set of joints. */
	void OverrideAll(const FCameraRigJoints& OtherJoints);

	/** Interpolate the joints towards anoter set of joints. */
	void LerpAll(const FCameraRigJoints& ToJoints, float BlendFactor);

private:

	using FJointArray = TArray<FCameraRigJoint, TInlineAllocator<2>>;
	FJointArray Joints;
};

}  // namespace UE::Cameras

