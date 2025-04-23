// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigProxyTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigProxyTable)

UCameraRigAsset* FCameraRigProxyTable::ResolveProxy(const FCameraRigProxyTableResolveParams& InParams) const
{
	ensure(InParams.CameraRigProxy);

	for (const FCameraRigProxyTableEntry& Entry : Entries)
	{
		if (InParams.CameraRigProxy == Entry.CameraRigProxy)
		{
			return Entry.CameraRig;
		}
	}
	return nullptr;
}

