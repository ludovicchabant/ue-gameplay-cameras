// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/Input2DCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Input2DCameraNode)

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FInput2DCameraNodeEvaluator)

void FInput2DCameraNodeEvaluator::OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar)
{
	Ar << InputValue;
}

}  // namespace UE::Cameras

