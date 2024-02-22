// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/ArrayCameraNode.h"

#include "Core/CameraNodeEvaluator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ArrayCameraNode)

class FArrayCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(FArrayCameraNodeEvaluator)

protected:

	virtual FCameraNodeEvaluatorChildrenView OnGetChildren() override;
	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	TArray<FCameraNodeEvaluator*> Children;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FArrayCameraNodeEvaluator)

FCameraNodeEvaluatorChildrenView FArrayCameraNodeEvaluator::OnGetChildren()
{
	return FCameraNodeEvaluatorChildrenView(Children);
}

void FArrayCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params)
{
	const UArrayCameraNode* ArrayNode = GetCameraNodeAs<UArrayCameraNode>();
	for (const UCameraNode* Child : ArrayNode->Children)
	{
		if (Child)
		{
			FCameraNodeEvaluator* ChildEvaluator = Params.BuildEvaluator(Child);
			Children.Add(ChildEvaluator);
		}
	}
}

void FArrayCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	for (FCameraNodeEvaluator* Child : Children)
	{
		if (Child)
		{
			Child->Run(Params, OutResult);
		}
	}
}

FCameraNodeChildrenView UArrayCameraNode::OnGetChildren()
{
	return FCameraNodeChildrenView(Children);
}

FCameraNodeEvaluatorPtr UArrayCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	return Builder.BuildEvaluator<FArrayCameraNodeEvaluator>();
}

