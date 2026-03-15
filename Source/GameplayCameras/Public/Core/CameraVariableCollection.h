// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

#include "CameraVariableCollection.generated.h"

class UCameraVariableAsset;
class UCameraVariableCollection;
struct FSoftCameraVariablePtr;
enum class ECameraVariableType;

/**
 * An asset that represents a collection of camera variables.
 */
UCLASS(MinimalAPI)
class UCameraVariableCollection : public UObject
{
	GENERATED_BODY()

public:

	UCameraVariableCollection(const FObjectInitializer& ObjectInit);

public:

	/**
	 * Finds information about the variables found in the given collection asset and which are compatible with the
	 * given variable type.
	 *
	 * If the collection asset was saved before metadata was added, it will have to be loaded.
	 */
	GAMEPLAYCAMERAS_API static void GetVariablesForType(const FAssetData& AssetData, ECameraVariableType DesiredType, TArray<FSoftCameraVariablePtr>& OutVariables);

protected:

	// UObject interface
	virtual void PostLoad() override;
	virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;

private:

#if WITH_EDITOR
	void CleanUpStrayObjects();
#endif

public:

	/** The variables in this collection. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraVariableAsset>> Variables;
};

/**
 * A soft reference to a camera variable.
 */
USTRUCT()
struct FSoftCameraVariablePtr
{
	GENERATED_BODY()

	/** The variable collection. */
	UPROPERTY()
	TSoftObjectPtr<UCameraVariableCollection> VariableCollection;

	/** The ID of the variable in the collection. */
	UPROPERTY()
	FGuid VariableGuid;

	/** Optionally, the display name of the variable if it was available. */
	UPROPERTY()
	FString VariableDisplayName;

	/** Resolve the camera variable.*/
	GAMEPLAYCAMERAS_API UCameraVariableAsset* Get() const;

	/** Returns whether this soft pointer is valid. */
	bool IsValid() const { return VariableCollection.IsValid() && VariableGuid.IsValid(); }
};

