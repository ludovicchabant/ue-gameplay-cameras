// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigJoints.h"

#include "Core/BuiltInCameraVariables.h"

namespace UE::Cameras
{

void FCameraRigJoints::AddJoint(const FCameraRigJoint& InJoint)
{
	Joints.Add(InJoint);
}

void FCameraRigJoints::AddJoint(const FCameraVariableDefinition& InVariableDefinition, const FTransform3d& InTransform)
{
	AddJoint(FCameraRigJoint{ InVariableDefinition.VariableID, InTransform });
}

void FCameraRigJoints::AddYawPitchJoint(const FTransform3d& InTransform)
{
	AddJoint(FBuiltInCameraVariables::Get().YawPitchDefinition, InTransform);
}

void FCameraRigJoints::Reset()
{
	Joints.Reset();
}

}  // namespace UE::Cameras

