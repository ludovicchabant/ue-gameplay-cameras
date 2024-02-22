// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorBuilder.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ObjectChildrenView.h"
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

/** View on a camera node's children. */
using FCameraNodeChildrenView = TObjectChildrenView<TObjectPtr<UCameraNode>>;

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, MinimalAPI)
class UCameraNode : public UObject
{
	GENERATED_BODY()

public:

	/** Get the list of children under this node. */
	FCameraNodeChildrenView GetChildren();

	/** Gets optional info about this node's required allocations at runtime. */
	FCameraNodeAllocationInfo GetAllocationInfo() const;

	/** Builds the evaluator for this node. */
	FCameraNodeEvaluatorPtr BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

protected:

	/** Get the list of children under this node. */
	virtual FCameraNodeChildrenView OnGetChildren() { return FCameraNodeChildrenView(); }

	/** Gets optional info about this node's required allocations at runtime. */
	virtual FCameraNodeAllocationInfo OnGetAllocationInfo() const { return FCameraNodeAllocationInfo(); }

	/** Builds the evaluator for this node. */
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const { return nullptr; }

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;
};

