// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraParameters.h"

#include "UObject/UnrealNames.h"

bool FBooleanCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_BoolProperty)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FInteger32CameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_IntProperty || Tag.Type == NAME_Int32Property)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FFloatCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_FloatProperty)
	{
		Slot << Value;
		return true;
	}

	return false;
}


bool FDoubleCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_DoubleProperty)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FVector2fCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && (Tag.StructName == NAME_Vector2f || Tag.StructName == NAME_Vector2D))
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FVector2dCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Vector2d)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FVector3fCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Vector3f)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FVector3dCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Vector3d)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FVector4fCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Vector4f)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FVector4dCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Vector4d)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FRotator3fCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Rotator3f)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FRotator3dCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Rotator3d)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FTransform3fCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Transform3f)
	{
		Slot << Value;
		return true;
	}

	return false;
}

bool FTransform3dCameraParameter::SerializeFromMismatchedTag(const FPropertyTag& Tag, FStructuredArchive::FSlot Slot)
{
	if (Tag.Type == NAME_StructProperty && Tag.StructName == NAME_Transform3d)
	{
		Slot << Value;
		return true;
	}

	return false;
}

