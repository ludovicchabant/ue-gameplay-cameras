// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Attach/AttachToActorCameraNode.h"

#include "Core/CameraParameterReader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttachToActorCameraNode)

namespace UE::Cameras
{

class FAttachToActorCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FAttachToActorCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	FCameraActorAttachmentInfoReader AttachmentReader;
	TCameraParameterReader<bool> AttachToLocationReader;
	TCameraParameterReader<bool> AttachToRotationReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FAttachToActorCameraNodeEvaluator)

void FAttachToActorCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UAttachToActorCameraNode* AttachNode = GetCameraNodeAs<UAttachToActorCameraNode>();
	AttachmentReader.Initialize(AttachNode->Attachment, AttachNode->AttachmentDataID);
	AttachToLocationReader.Initialize(AttachNode->AttachToLocation);
	AttachToRotationReader.Initialize(AttachNode->AttachToRotation);
}

void FAttachToActorCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FTransform3d AttachTransform;
	if (!AttachmentReader.GetAttachmentTransform(OutResult.ContextDataTable, AttachTransform))
	{
		return;
	}

	const bool bAttachToLocation = AttachToLocationReader.Get(OutResult.VariableTable);
	const bool bAttachToRotation = AttachToRotationReader.Get(OutResult.VariableTable);
	
	if (bAttachToLocation)
	{
		const FVector3d AttachLocation = AttachTransform.GetLocation();
		OutResult.CameraPose.SetLocation(AttachLocation);
	}

	if (bAttachToRotation)
	{
		const FRotator3d AttachRotation = AttachTransform.GetRotation().Rotator();
		OutResult.CameraPose.SetRotation(AttachRotation);
	}
}

}  // namespace UE::Cameras

FCameraNodeEvaluatorPtr UAttachToActorCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FAttachToActorCameraNodeEvaluator>();
}

