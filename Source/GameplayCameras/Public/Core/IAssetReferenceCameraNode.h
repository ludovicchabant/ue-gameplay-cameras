// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraRigAsset.h"
#include "Containers/Array.h"
#include "UObject/Interface.h"

#include "IAssetReferenceCameraNode.generated.h"

class UCameraNode;

UINTERFACE(MinimalAPI)
class UAssetReferenceCameraNode : public UInterface
{
	GENERATED_BODY()
};

class IAssetReferenceCameraNode
{
	GENERATED_BODY()

public:

	virtual void GatherPackages(FCameraRigPackages& OutPackages) const = 0;
};

