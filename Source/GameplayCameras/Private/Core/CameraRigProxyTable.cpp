// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigProxyTable.h"

UCameraRigProxyTable::UCameraRigProxyTable(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
}

UCameraRigAsset* UCameraRigProxyTable::ResolveProxy(const FCameraRigProxyTableResolveParams& InParams) const
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

