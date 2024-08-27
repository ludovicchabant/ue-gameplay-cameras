// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "StateTreeConditionBase.h"
#include "StateTreeSchema.h"
#include "StateTreeTaskBase.h"
#include "StateTreeTypes.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectPtr.h"

#include "CameraDirectorStateTreeSchema.generated.h"

class UCameraRigAsset;
class UCameraRigProxyAsset;
struct FStateTreeExternalDataDesc;

namespace UE::Cameras
{

struct FStateTreeContextDataNames
{
	const static FName ContextOwner;
};

}  // namespace UE::Cameras

/**
 * The schema of the StateTree for a StateTree camera director.
 */
UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta=(DisplayName="Gameplay Camera Director"))
class GAMEPLAYCAMERAS_API UCameraDirectorStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:

	UCameraDirectorStateTreeSchema();

protected:

	// UStateTreeSchema interface.
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
	virtual bool IsClassAllowed(const UClass* InScriptStruct) const override;
	virtual bool IsExternalItemAllowed(const UStruct& InStruct) const override;
	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override { return ContextDataDescs; }

private:

	UPROPERTY()
	TArray<FStateTreeExternalDataDesc> ContextDataDescs;
};

/** The evaluation data for the StateTree camera director. */
USTRUCT(BlueprintType)
struct GAMEPLAYCAMERAS_API FCameraDirectorStateTreeEvaluationData
{
	GENERATED_BODY()

	/** Camera rigs activated during a StateTree's execution frame. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigAsset>> ActiveCameraRigs;

	/** Camera rig proxies activated during a StateTree's execution frame. */
	UPROPERTY()
	TArray<TObjectPtr<UCameraRigProxyAsset>> ActiveCameraRigProxies;

public:

	/** Reset this evaluation data for a new frame. */
	void Reset();
};

/** Base classs for camera director StateTree tasks. */
USTRUCT(meta = (Hidden))
struct GAMEPLAYCAMERAS_API FGameplayCamerasStateTreeTask : public FStateTreeTaskBase
{
	GENERATED_BODY()
};

/** Base classs for camera director StateTree conditions. */
USTRUCT(meta = (Hidden))
struct GAMEPLAYCAMERAS_API FGameplayCamerasStateTreeCondition : public FStateTreeConditionBase
{
	GENERATED_BODY()
};

