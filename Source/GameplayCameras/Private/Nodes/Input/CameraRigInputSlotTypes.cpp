// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Input/CameraRigInputSlotTypes.h"

double FCameraParameterClamping::ClampValue(double Value) const
{
	if (bClampMin && Value < MinValue)
	{
		Value = MinValue;
	}
	if (bClampMax && Value > MaxValue)
	{
		Value = MaxValue;
	}
	return Value;
}

double FCameraParameterNormalization::NormalizeValue(double Value) const
{
	if (bNormalize && MaxValue > 0)
	{
		while (Value > MaxValue)
		{
			Value -= MaxValue;
		}
	}
	return Value;
}

