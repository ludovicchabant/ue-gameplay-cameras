// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/Input2DCameraNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Input2DCameraNode)

namespace UE::Cameras
{

UE_DEFINE_CAMERA_NODE_EVALUATOR(FInput2DCameraNodeEvaluator)

FInput2DCameraNodeEvaluator::FInput2DCameraNodeEvaluator()
{
	// Only run during parameter update.
	SetNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::NeedsParameterUpdate);
}

}  // namespace UE::Cameras

