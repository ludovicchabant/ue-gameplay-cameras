// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Common/FilmbackCameraNode.h"

#include "Core/CameraParameterReader.h"
#include "Core/CameraPose.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FilmbackCameraNode)

namespace UE::Cameras
{

class FFilmbackCameraNodeEvaluator : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(GAMEPLAYCAMERAS_API, FFilmbackCameraNodeEvaluator)

protected:

	virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override;
	virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;

private:

	TCameraParameterReader<float> SensorWidthReader;
	TCameraParameterReader<float> SensorHeightReader;
	TCameraParameterReader<float> ISOReader;
	TCameraParameterReader<bool> ConstrainAspectRatioReader;
	TCameraParameterReader<bool> OverrideAspectRatioAxisConstraintReader;
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FFilmbackCameraNodeEvaluator)

void FFilmbackCameraNodeEvaluator::OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	const UFilmbackCameraNode* FilmbackNode = GetCameraNodeAs<UFilmbackCameraNode>();
	SensorWidthReader.Initialize(FilmbackNode->SensorWidth);
	SensorHeightReader.Initialize(FilmbackNode->SensorHeight);
	ISOReader.Initialize(FilmbackNode->ISO);
	ConstrainAspectRatioReader.Initialize(FilmbackNode->ConstrainAspectRatio);
	OverrideAspectRatioAxisConstraintReader.Initialize(FilmbackNode->OverrideAspectRatioAxisConstraint);
}

void FFilmbackCameraNodeEvaluator::OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult)
{
	FCameraPose& OutPose = OutResult.CameraPose;

	float SensorWidth = SensorWidthReader.Get(OutResult.VariableTable);
	if (SensorWidth > 0)
	{
		OutPose.SetSensorWidth(SensorWidth);
	}
	float SensorHeight = SensorHeightReader.Get(OutResult.VariableTable);
	if (SensorHeight > 0)
	{
		OutPose.SetSensorHeight(SensorHeight);
	}
	float ISO = ISOReader.Get(OutResult.VariableTable);
	if (ISO > 0)
	{
		OutPose.SetISO(ISO);
	}

	OutPose.SetConstrainAspectRatio(ConstrainAspectRatioReader.Get(OutResult.VariableTable));
	OutPose.SetOverrideAspectRatioAxisConstraint(OverrideAspectRatioAxisConstraintReader.Get(OutResult.VariableTable));

	// TODO: add support for enum camera parameters.
	const UFilmbackCameraNode* FilmbackNode = GetCameraNodeAs<UFilmbackCameraNode>();
	OutPose.SetAspectRatioAxisConstraint(FilmbackNode->AspectRatioAxisConstraint);
}

}  // namespace UE::Cameras

UFilmbackCameraNode::UFilmbackCameraNode(const FObjectInitializer& ObjectInitializer)
{
	FCameraPose::GetDefaultSensorSize(SensorWidth.Value, SensorHeight.Value);
}

FCameraNodeEvaluatorPtr UFilmbackCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FFilmbackCameraNodeEvaluator>();
}

