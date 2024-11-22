// Copyright Epic Games, Inc. All Rights Reserved.

#include "Curves/CameraVectorCurve.h"

FVector FCameraVectorCurve::GetValue(float InTime) const
{
	FVector Result;
	Result.X = Curves[0].Eval(InTime);
	Result.Y = Curves[1].Eval(InTime);
	Result.Z = Curves[2].Eval(InTime);
	return Result;
}

