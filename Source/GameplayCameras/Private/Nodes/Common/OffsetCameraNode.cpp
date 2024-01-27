// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/OffsetCameraNode.h"

#include "Core/CameraEvaluationContext.h"
#include "GameplayCameras.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OffsetCameraNode)

void UOffsetCameraNode::OnRun(const FCameraNodeRunParams& Params, FCameraNodeRunResult& OutResult)
{
	FVector3d LocalOffset = Offset;
	switch(OffsetSpace)
	{
		case ECameraNodeSpace::CameraPose:
		default:
			{
				const FRotator3d Rotation = OutResult.CameraPose.GetRotation();
				LocalOffset = Rotation.RotateVector(Offset);
			}
			break;
		case ECameraNodeSpace::Context:
			if (Params.EvaluationContext)
			{ 
				const FCameraNodeRunResult& InitialResult = Params.EvaluationContext->GetInitialResult();
				ensureMsgf(InitialResult.bIsValid,
						TEXT("OffsetCameraNode: using invalid context result as offset space!"));
				const FRotator3d Rotation = InitialResult.CameraPose.GetRotation();
				LocalOffset = Rotation.RotateVector(Offset);
			}
			else
			{
				UE_LOG(LogCameraSystem, Error, 
						TEXT("OffsetCameraNode: cannot offset in context space when there is "
							 "no current context set."));
				return;
			}
			break;
		case ECameraNodeSpace::World:
			// Nothing to do;
			break;
	}

	const FVector3d Location = OutResult.CameraPose.GetLocation();
	OutResult.CameraPose.SetLocation(Location + LocalOffset);
}

