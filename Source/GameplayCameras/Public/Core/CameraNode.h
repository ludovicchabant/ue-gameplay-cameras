// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorBuilder.h"
#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraNode.generated.h"

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, MinimalAPI)
class UCameraNode : public UObject
{
	GENERATED_BODY()

public:

	/** Gets optional info about this node's evaluator. */
	FCameraNodeEvaluatorAllocationInfo GetEvaluatorInfo() const;
	/** Builds the evaluator for this node. */
	FCameraNodeEvaluatorPtr BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

protected:

	/** Gets optional info about this node's evaluator. */
	virtual FCameraNodeEvaluatorAllocationInfo OnGetEvaluatorInfo() const { return FCameraNodeEvaluatorAllocationInfo(); }
	/** Builds the evaluator for this node. */
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const { return nullptr; }

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;
};

