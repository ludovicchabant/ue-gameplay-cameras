// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/ConsumableValue.h"

namespace UE::Cameras
{

/**
 * Hard-coded type IDs for built-in camera operations.
 */
enum EBuiltInCameraOperationTypes : uint32
{
	YawPitch = 0,
	SingleValue = 1,

	MAX = 3
};

/**
 * Simple type ID for camera operations.
 */
struct FCameraOperationTypeID
{
	FCameraOperationTypeID() : Value(INVALID) {}

	FCameraOperationTypeID(uint32 InValue) : Value(InValue) {}

public:

	/** Returns whether this ID is valid. */
	bool IsValid() const { return Value != INVALID; }

	friend bool operator==(FCameraOperationTypeID A, FCameraOperationTypeID B)
	{
		return A.Value == B.Value;
	}

private:

	static const uint32 INVALID = (uint32)-1;

	uint32 Value;
};

/**
 * Base class for an operation to be executed on a camera rig.
 */
struct FCameraOperation
{
public:

	/** Attempts to cast this operation into a sub-class. */
	template<typename OperationType>
	OperationType* CastOperation()
	{
		if (OperationTypeID == OperationType::GetOperationTypeID())
		{
			return static_cast<OperationType*>(this);
		}
		return nullptr;
	}

protected:

	FCameraOperation(FCameraOperationTypeID InOperationTypeID)
		: OperationTypeID(InOperationTypeID)
	{}

private:

	FCameraOperationTypeID OperationTypeID;
};

#define UE_DEFINE_CAMERA_IK_OPERATION(OperationName, OperationType)\
	static FCameraOperationTypeID GetOperationTypeID()\
	{\
		return FCameraOperationTypeID(OperationType);\
	}\
	OperationName()\
		: FCameraOperation(OperationName::GetOperationTypeID())\
	{}

/**
 * A camera operation that tries to correct the yaw/pitch of a camera rig.
 */
struct FYawPitchCameraOperation : public FCameraOperation
{
	UE_DEFINE_CAMERA_IK_OPERATION(FYawPitchCameraOperation, EBuiltInCameraOperationTypes::YawPitch)

	FConsumableDouble Yaw;
	FConsumableDouble Pitch;
};

/**
 * A camera operation that tries to correct a single undetermined input value on a
 * camera rig.
 */
struct FSingleValueCameraOperation : public FCameraOperation
{
	UE_DEFINE_CAMERA_IK_OPERATION(FSingleValueCameraOperation, EBuiltInCameraOperationTypes::SingleValue)

	FConsumableDouble Value;
};

}  // namespace UE::Cameras

