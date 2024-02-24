// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNodeEvaluatorBuilder.h"
#include "Core/CameraVariableTableFwd.h"
#include "Core/ObjectChildrenView.h"
#include "CoreTypes.h"
#include "UObject/Object.h"

#include "CameraNode.generated.h"

struct FCameraRigAllocationInfo;

/** View on a camera node's children. */
using FCameraNodeChildrenView = UE::Cameras::TObjectChildrenView<TObjectPtr<UCameraNode>>;

/**
 * The base class for a camera node.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, MinimalAPI)
class UCameraNode : public UObject
{
	GENERATED_BODY()

public:
	
	using FCameraNodeEvaluatorBuilder = UE::Cameras::FCameraNodeEvaluatorBuilder;

	/** Get the list of children under this node. */
	FCameraNodeChildrenView GetChildren();

	/** Gets optional info about this node's required allocations at runtime. */
	void BuildAllocationInfo(FCameraRigAllocationInfo& AllocationInfo) const;

	/** Builds the evaluator for this node. */
	FCameraNodeEvaluatorPtr BuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const;

protected:

	/** Get the list of children under this node. */
	virtual FCameraNodeChildrenView OnGetChildren() { return FCameraNodeChildrenView(); }

	/** Gets optional info about this node's required allocations at runtime. */
	virtual void OnBuildAllocationInfo(FCameraRigAllocationInfo& AllocationInfo) const {}

	/** Builds the evaluator for this node. */
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const { return nullptr; }

public:

	/** Specifies whether this node is enabled. */
	UPROPERTY(EditAnywhere, Category=Common)
	bool bIsEnabled = true;
};

