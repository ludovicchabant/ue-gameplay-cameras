// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraRigProxyTable.generated.h"

class UCameraRigAsset;
class UCameraRigProxyAsset;

/**
 * Parameter structure for resolving a camera rig proxy from a proxy table.
 */
struct FCameraRigProxyTableResolveParams
{
	/** The camera rig proxy to resolve. */
	const UCameraRigProxyAsset* CameraRigProxy = nullptr;
};

/**
 * An entry in a camera rig proxy table.
 */
USTRUCT()
struct FCameraRigProxyTableEntry
{
	GENERATED_BODY()

	/** The camera rig proxy for this table entry. */
	UPROPERTY(EditAnywhere, Category="Camera")
	TObjectPtr<UCameraRigProxyAsset> CameraRigProxy;

	/** The actual camera rig that should be mapped to the correspondig proxy. */
	UPROPERTY(EditAnywhere, Category="Camera")
	TObjectPtr<UCameraRigAsset> CameraRig;
};

/**
 * A table that defines mappings between camera rig proxies and actual camera rigs.
 */
UCLASS(MinimalAPI, EditInlineNew)
class UCameraRigProxyTable : public UObject
{
	GENERATED_BODY()

public:

	UCameraRigProxyTable(const FObjectInitializer& ObjectInit);

	/**
	 * Resolves a given proxy to an actual camera rig.
	 * Returns nullptr if the given proxy wasn't found, or not mapped to anything in the table.
	 */
	UCameraRigAsset* ResolveProxy(const FCameraRigProxyTableResolveParams& InParams) const;

public:

	/** The entries in the table. */
	UPROPERTY(EditAnywhere, Category="Camera")
	TArray<FCameraRigProxyTableEntry> Entries;
};

