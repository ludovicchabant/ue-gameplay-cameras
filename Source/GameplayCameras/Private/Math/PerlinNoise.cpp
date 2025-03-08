// Copyright Epic Games, Inc. All Rights Reserved.

#include "Math/PerlinNoise.h"

#include "Math/Interpolation.h"
#include "Math/UnrealMathUtility.h"
#include "Serialization/Archive.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PerlinNoise)

namespace UE::Cameras
{

FPerlinNoise::FPerlinNoise()
{
	Initialize(1.f);
}

FPerlinNoise::FPerlinNoise(const FPerlinNoiseData& InData, uint8 InOctaves)
	: Amplitude(InData.Amplitude)
	, NumOctaves(FMath::Clamp(InOctaves, 1, MAX_OCTAVES))
{
	Initialize(1.f);
}

FPerlinNoise::FPerlinNoise(float InAmplitude, float InFrequency, uint8 InOctaves)
	: Amplitude(InAmplitude)
	, NumOctaves(FMath::Clamp(InOctaves, 1, MAX_OCTAVES))
{
	Initialize(InFrequency);
}

void FPerlinNoise::Initialize(float InFrequency)
{
	float OctaveFrequency = InFrequency;

	for (int32 Index = 0; Index < NumOctaves; ++Index)
	{
		FSinglePerlinNoise& Octave(Octaves[Index]);
		Octave.Frequency = OctaveFrequency;
		Octave.Prev = FMath::FRandRange(-1.f, 1.f);
		Octave.Next = FMath::FRandRange(-1.f, 1.f);

		OctaveFrequency *= Lacunarity;
	}
}

void FPerlinNoise::SetFrequency(float InFrequency)
{
	if (InFrequency == Octaves[0].Frequency)
	{
		return;
	}

	// Move our current time to be in the same relative place inside the new interval
	// compared to the old interval. So for instance if we were at 70% between the 
	// noise peaks using the old frequency, let's move ourselves to be at 70% between
	// the noise peaks of the new frequency.
	// This loses the amount of accumulated time, but we don't really need it.

	float NewOctaveFrequency = InFrequency;

	for (int32 Index = 0; Index < NumOctaves; ++Index)
	{
		FSinglePerlinNoise& Octave(Octaves[Index]);

		const float OldInterval = (Octave.Frequency > 0 ? 1.f / Octave.Frequency : 1.f);
		const float OldIntervalFactor = FMath::Fractional(Octave.CurTime / OldInterval);

		Octave.Frequency = NewOctaveFrequency;

		const float NewInterval = (Octave.Frequency > 0 ? 1.f / Octave.Frequency : 1.f);
		Octave.CurTime = NewInterval * OldIntervalFactor;

		NewOctaveFrequency *= Lacunarity;
	}
}

void FPerlinNoise::SetNumOctaves(uint8 InNumOctaves)
{
	InNumOctaves = FMath::Clamp(InNumOctaves, 1, MAX_OCTAVES);
	if (InNumOctaves < NumOctaves)
	{
		NumOctaves = InNumOctaves;
	}
	else if (InNumOctaves > NumOctaves)
	{
		float OctaveFrequency = Octaves[NumOctaves - 1].Frequency * Lacunarity;

		for (int32 Index = NumOctaves; Index < InNumOctaves; ++Index)
		{
			FSinglePerlinNoise& Octave(Octaves[Index]);
			Octave.Frequency = OctaveFrequency;
			Octave.CurTime = 0.f;
			Octave.Prev = FMath::FRandRange(-1.f, 1.f);
			Octave.Next = FMath::FRandRange(-1.f, 1.f);

			OctaveFrequency *= Lacunarity;
		}

		NumOctaves = InNumOctaves;
	}
}

float FPerlinNoise::GenerateValue(float DeltaTime)
{
	float Value = 0.f;
	float OctaveAmplitude = Amplitude;

	for (int32 Index = 0; Index < NumOctaves; ++Index)
	{
		FSinglePerlinNoise& Octave(Octaves[Index]);

		const float Interval = (Octave.Frequency > 0 ? 1.f / Octave.Frequency : 1.f);

		const float PrevNumIntervals = (Octave.CurTime / Interval);
		const float NextNumIntervals = ((Octave.CurTime + DeltaTime) / Interval);

		// If we are going over the end of the current interval, generate
		// some new value for the next interval.
		if ((int32)NextNumIntervals > (int32)PrevNumIntervals)
		{
			Octave.Prev = Octave.Next;
			Octave.Next = FMath::FRandRange(-1.f, 1.f);
		}

		Octave.CurTime += DeltaTime;

		const float IntervalFactor = (NextNumIntervals - FMath::TruncToFloat(NextNumIntervals));
		const float InterpFactor = SmootherStep(IntervalFactor);

		Value += OctaveAmplitude * FMath::Lerp(Octave.Prev, Octave.Next, InterpFactor);

		OctaveAmplitude *= OctaveGain;
	}

	return Value;
}

void FPerlinNoise::Serialize(FArchive& Ar)
{
	Ar << Amplitude;
	Ar << Lacunarity;
	Ar << OctaveGain;

	Ar << NumOctaves;

	for (int32 Index = 0; Index < MAX_OCTAVES; ++Index)
	{
		FSinglePerlinNoise& Octave(Octaves[Index]);
		Ar << Octave.Frequency;
		Ar << Octave.CurTime;
		Ar << Octave.Prev;
		Ar << Octave.Next;
	}
}

}  // namespace UE::Cameras

