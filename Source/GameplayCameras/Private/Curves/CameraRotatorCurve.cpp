// Copyright Epic Games, Inc. All Rights Reserved.

#include "Curves/CameraRotatorCurve.h"

#include "Math/Rotator.h"

FRotator FCameraRotatorCurve::GetValue(float InTime) const
{
	FRotator Result;
	Result.Pitch = Curves[0].Eval(InTime);
	Result.Yaw = Curves[1].Eval(InTime);
	Result.Roll = Curves[2].Eval(InTime);
	return Result;
}

bool FCameraRotatorCurve::HasAnyData() const
{
	return Curves[0].HasAnyData() || Curves[1].HasAnyData() || Curves[2].HasAnyData();
}

