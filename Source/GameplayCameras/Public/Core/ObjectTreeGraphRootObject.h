// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ObjectTreeGraphRootObject.generated.h"

UINTERFACE(MinimalAPI)
class UObjectTreeGraphRootObject : public UInterface
{
	GENERATED_BODY()
};

class IObjectTreeGraphRootObject
{
	GENERATED_BODY()

public:

#if WITH_EDITOR

	virtual void AddConnectableObject(UObject* InObject) {}
	virtual void RemoveConnectableObject(UObject* InObject) {}
	virtual void GetExtraConnectableObjects(TArray<UObject*>& OutObjects) const {}

#endif  // WITH_EDITOR

};

