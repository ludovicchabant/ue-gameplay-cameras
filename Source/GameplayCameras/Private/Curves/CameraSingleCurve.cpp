// Copyright Epic Games, Inc. All Rights Reserved.

#include "Curves/CameraSingleCurve.h"

float FCameraSingleCurve::GetValue(float InTime) const
{
	return Curve.Eval(InTime);
}

bool FCameraSingleCurve::HasAnyData() const
{
	return Curve.HasAnyData();
}

