// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigProxyRedirectTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigProxyRedirectTable)

UCameraRigAsset* FCameraRigProxyRedirectTable::ResolveProxy(const FCameraRigProxyResolveParams& InParams) const
{
	ensure(InParams.CameraRigProxy);

	for (const FCameraRigProxyRedirectTableEntry& Entry : Entries)
	{
		if (InParams.CameraRigProxy == Entry.CameraRigProxy)
		{
			return Entry.CameraRig;
		}
	}
	return nullptr;
}

