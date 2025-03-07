// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNode.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "Core/ObjectTreeGraphObject.h"
#include "GameFramework/BlueprintCameraPose.h"
#include "GameFramework/BlueprintCameraVariableTable.h"

#include "BlueprintCameraNode.generated.h"

class UCameraVariableAsset;
class UPropertyBag;

namespace UE::Cameras
{
	class FBlueprintCameraNodeEvaluator;
	class FCameraVariableTable;
	struct FCameraNodeEvaluationParams;
	struct FCameraNodeEvaluationResult;
};

/**
 * The base class for Blueprint camera node evaluators.
 */
UCLASS(MinimalAPI, Blueprintable, Abstract, EditInlineNew, CollapseCategories)
class UBlueprintCameraNodeEvaluator : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category="Evaluation")
	void InitializeCameraNode();

	/** The main execution callback for the camera node. Call SetCameraPose to affect the result. */
	UFUNCTION(BlueprintImplementableEvent, Category="Evaluation")
	void TickCameraNode(float DeltaTime);

	/**
	 * A utility function that tries to find if an actor owns the evaluation context.
	 * Handles the situation where the evaluation context is an actor component (like a
	 * UGameplayCameraComponent) or an actor itself.
	 */
	UFUNCTION(BlueprintPure, Category="Evaluation", meta=(DeterminesOutputType="ActorClass"))
	AActor* FindEvaluationContextOwnerActor(TSubclassOf<AActor> ActorClass) const;

public:

	using FCameraNodeEvaluatorInitializeParams = UE::Cameras::FCameraNodeEvaluatorInitializeParams;
	using FCameraNodeEvaluationParams = UE::Cameras::FCameraNodeEvaluationParams;
	using FCameraNodeEvaluationResult = UE::Cameras::FCameraNodeEvaluationResult;
	using FCameraEvaluationContext = UE::Cameras::FCameraEvaluationContext;

	/** Initialize this camera node. */
	void NativeInitializeCameraNode(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult);

	/** Runs this camera node. */
	void NativeRunCameraNode(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult);

public:

	// UObject interface.
	virtual UWorld* GetWorld() const override;
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override { return true; }
#endif  // WITH_EDITOR

private:

	void SetupExecution(TSharedPtr<const FCameraEvaluationContext> EvaluationContext, FCameraNodeEvaluationResult& OutResult);
	void TeardownExecution();

protected:

	/** Whether this is the first frame of this camera node's lifetime. */
	UPROPERTY(BlueprintReadOnly, Category="Evaluation")
	bool bIsFirstFrame = false;

	/** The owner object of this camera node's evaluation context. */
	UPROPERTY(BlueprintReadOnly, Category="Evaluation")
	TObjectPtr<UObject> EvaluationContextOwner;

	/** The input/output camera pose for this frame. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	FBlueprintCameraPose CameraPose;

	/** The input/output camera variable table for this frame. */
	UPROPERTY(BlueprintReadWrite, Category="Evaluation")
	FBlueprintCameraVariableTable VariableTable;

private:

	TSharedPtr<const UE::Cameras::FCameraEvaluationContext> CurrentContext;

	FCameraNodeEvaluationResult* CurrentResult = nullptr;

	mutable TWeakObjectPtr<UWorld> WeakCachedWorld;
};

/**
 * A camera node that runs arbitrary Blueprint logic.
 */
UCLASS(MinimalAPI, meta=(CameraNodeCategories="Common,Transform"))
class UBlueprintCameraNode
	: public UCameraNode
	, public ICustomCameraNodeParameterProvider
{
	GENERATED_BODY()

public:

	UBlueprintCameraNode(const FObjectInitializer& ObjInit);

protected:

	// UCameraNode interface.
	virtual void OnPreBuild(FCameraBuildLog& BuildLog) override;
	virtual void OnBuild(FCameraObjectBuildContext& BuildContext) override;
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

	// IObjectTreeGraphObject interface.
#if WITH_EDITOR
	virtual EObjectTreeGraphObjectSupportFlags GetSupportFlags(FName InGraphName) const override;
	virtual void GetGraphNodeName(FName InGraphName, FText& OutName) const override;
#endif  // WITH_EDITOR

	// ICustomCameraNodeParameterProvider interface.
	virtual void GetCustomCameraNodeParameters(FCustomCameraNodeParameterInfos& OutParameterInfos) override;

	// UObject interface.
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	const UBlueprintCameraNodeEvaluator* GetCameraNodeEvaluatorTemplate() const { return CameraNodeEvaluatorTemplate; }

private:

	void RebuildOverrides();

#if WITH_EDITOR
	void OnObjectsReplaced(const TMap<UObject*, UObject*>& ReplacementMap);
#endif

private:

	/** The camera node evaluator to instantiate and run. */
	UPROPERTY(Instanced, EditAnywhere, Category=Common)
	TObjectPtr<UBlueprintCameraNodeEvaluator> CameraNodeEvaluatorTemplate;

	/** Overrides for the evaluator instance. */
	UPROPERTY()
	FCustomCameraNodeParameters CameraNodeEvaluatorOverrides;

	// Deprecated.
	
	UPROPERTY()
	TSubclassOf<UBlueprintCameraNodeEvaluator> CameraNodeEvaluatorClass_DEPRECATED;

	friend class UE::Cameras::FBlueprintCameraNodeEvaluator;
};

