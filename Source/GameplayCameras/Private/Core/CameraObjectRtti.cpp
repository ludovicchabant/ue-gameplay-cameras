// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraObjectRtti.h"

namespace UE::Cameras
{

uint32 FCameraObjectTypeID::RegisterNewID()
{
	static uint32 ID = 0;
	return ID++;
}

FCameraObjectTypeRegistry& FCameraObjectTypeRegistry::Get()
{
	static FCameraObjectTypeRegistry StaticRegistry;
	return StaticRegistry;
}

void FCameraObjectTypeRegistry::RegisterType(FCameraObjectTypeID TypeID, FCameraObjectTypeInfo&& TypeInfo)
{
	ensureMsgf(
			!TypeIDsByName.Contains(TypeID.GetTypeName()), 
			TEXT("Type '%s' has already been registered!"), *TypeID.GetTypeName().ToString());
	TypeIDsByName.Add(TypeID.GetTypeName(), TypeID.GetTypeID());
	TypeInfos.Insert(TypeID.GetTypeID(), MoveTemp(TypeInfo));
}

void FCameraObjectTypeRegistry::ConstructObject(FCameraObjectTypeID TypeID, void* Ptr)
{
	if (ensureMsgf(TypeInfos.IsValidIndex(TypeID.GetTypeID()), TEXT("Invalid camera object type ID!")))
	{
		const FCameraObjectTypeInfo& TypeInfo = TypeInfos[TypeID.GetTypeID()];
		TypeInfo.Constructor(Ptr);
	}
}

}  // namespace UE::Cameras

