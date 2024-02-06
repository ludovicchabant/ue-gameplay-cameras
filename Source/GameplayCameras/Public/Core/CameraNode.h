// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorBuilder.h"
#include "Core/CameraVariableTableFwd.h"
#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraNode.generated.h"

/**
 * Structure describing various allocations needed by a camera node.
 */
USTRUCT()
struct FCameraNodeAllocationInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FCameraNodeEvaluatorAllocationInfo EvaluatorInfo;

	UPROPERTY()
	FCameraVariableTableAllocationInfo VariableTableInfo;
};

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, MinimalAPI)
class UCameraNode : public UObject
{
	GENERATED_BODY()

public:

	/** Gets optional info about this node's required allocations at runtime. */
	FCameraNodeAllocationInfo GetAllocationInfo() const;

	/** Builds the evaluator for this node. */
	FCameraNodeEvaluatorPtr BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

protected:

	/** Gets optional info about this node's required allocations at runtime. */
	virtual FCameraNodeAllocationInfo OnGetAllocationInfo() const { return FCameraNodeAllocationInfo(); }

	/** Builds the evaluator for this node. */
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const { return nullptr; }

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;
};

