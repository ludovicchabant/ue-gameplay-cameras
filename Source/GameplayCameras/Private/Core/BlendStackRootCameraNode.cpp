// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/BlendStackRootCameraNode.h"

#include "Core/BlendCameraNode.h"
#include "Core/CameraMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlendStackRootCameraNode)

FCameraNodeChildrenView UBlendStackRootCameraNode::OnGetChildren()
{
	return FCameraNodeChildrenView({ Blend, RootNode });
}

void UBlendStackRootCameraNode::OnRun(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult)
{
	if (Blend)
	{
		Blend->Run(Params, OutResult);
	}
	if (RootNode)
	{
		RootNode->Run(Params, OutResult);
	}
}

