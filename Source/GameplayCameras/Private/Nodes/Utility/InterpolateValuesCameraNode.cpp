// Copyright Epic Games, Inc. All Rights Reserved.

#include "Nodes/Utility/InterpolateValuesCameraNode.h"

#include "Build/CameraObjectBuildContext.h"
#include "Core/CameraNodeEvaluator.h"
#include "Core/CameraParameterReader.h"
#include "Debug/CameraDebugBlockBuilder.h"
#include "Debug/CameraDebugRenderer.h"
#include "Debug/DebugTextRenderer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InterpolateValuesCameraNode)

namespace UE::Cameras
{

#if UE_GAMEPLAY_CAMERAS_DEBUG

template<typename ValueType>
class TInterpolateValuesCameraNodeEvaluatorDebugBlockImpl
{
public:

	void AddEntry(ValueType InValue, ValueType OutValue)
	{
		Entries.Add(FEntryValues{ InValue, OutValue });
	}

	void SerializeImpl(FArchive& Ar)
	{
		Ar << Entries;
	}

	void DebugDrawImpl(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer)
	{
		Renderer.AddText(TEXT("%d entries\n"), Entries.Num());
		Renderer.AddIndent();
		{
			for (int32 Index = 0; Index < Entries.Num(); ++Index)
			{
				const FEntryValues& Entry = Entries[Index];
				Renderer.AddText(TEXT("{cam_passive}[%d]{cam_default} %s {cam_passive}->{cam_default} %s\n"),
						Index,
						*ToDebugString(Entry.InValue),
						*ToDebugString(Entry.OutValue));
			}
		}
		Renderer.RemoveIndent();
	}

private:

	struct FEntryValues
	{
		ValueType InValue;
		ValueType OutValue;
	};

	TArray<FEntryValues> Entries;

	friend FArchive& operator<< (FArchive& Ar, FEntryValues& Entry)
	{
		Ar << Entry.InValue;
		Ar << Entry.OutValue;
		return Ar;
	}
};

#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

template<typename ValueType>
class TInterpolateValuesCameraNodeEvaluatorImpl
{
public:

	void InitializeImpl(const UInterpolateValuesCameraNodeBase* InterpolateValuesNode, FCameraVariableTable& VariableTable)
	{
		using namespace UE::Cameras::Internal;

		const FString OutPrefix(TEXT("Out"));
		const ECameraVariableType VariableType = InterpolateValuesNode->GetVariableType();

		for (const FInterpolateValuesCameraNodeEntry& Value : InterpolateValuesNode->Values)
		{
			if (Value.Interpolator && Value.InVariableID && Value.OutVariableID)
			{
				if (!VariableTable.ContainsValue(Value.OutVariableID))
				{
					FCameraVariableDefinition VariableDefinition;
					VariableDefinition.VariableName = (OutPrefix + Value.Name);
					VariableDefinition.VariableType = VariableType;
					VariableDefinition.VariableID = Value.OutVariableID;
					VariableDefinition.bIsPrivate = true;
					VariableTable.AddVariable(VariableDefinition);
				}

				FEntry NewEntry;
				NewEntry.Interpolator = BuildInterpolator(Value.Interpolator);
				NewEntry.InVariableID = Value.InVariableID;
				NewEntry.OutVariableID = Value.OutVariableID;

				ValueType InitValue = GetDefaultValue();
				TryGetVariableTableValue<ValueType>(Value.InVariableID, VariableTable, InitValue);
				NewEntry.Interpolator->Reset(InitValue, InitValue);
				VariableTable.TrySetValue<ValueType>(Value.OutVariableID, InitValue);

				Entries.Add(MoveTemp(NewEntry));
			}
		}
	}

	void RunImpl(const FCameraValueInterpolationParams& Params, FCameraVariableTable& VariableTable)
	{
		using namespace UE::Cameras::Internal;

		FCameraValueInterpolationResult InterpResult(VariableTable);
		for (FEntry& Entry : Entries)
		{
			// Don't process entries that aren't connected.
			if (Entry.InVariableID && ensure(Entry.OutVariableID))
			{
				ValueType CurValue;
				if (TryGetVariableTableValue<ValueType>(Entry.InVariableID, VariableTable, CurValue))
				{
					Entry.Interpolator->Reset(Entry.Interpolator->GetCurrentValue(), CurValue);
				}

				Entry.Interpolator->Run(Params, InterpResult);

				VariableTable.TrySetValue<ValueType>(Entry.OutVariableID, Entry.Interpolator->GetCurrentValue());
			}
		}
	}

	void SerializeImpl(FArchive& Ar)
	{
		if (Ar.IsSaving())
		{
			int32 NumEntries = Entries.Num();
			Ar << NumEntries;

			const FCameraValueInterpolatorSerializeParams InterpParams;
			for (const FEntry& Entry : Entries)
			{
				Entry.Interpolator->Serialize(InterpParams, Ar);
			}
		}
		else if (Ar.IsLoading())
		{
			int32 NumEntries = 0;
			Ar << NumEntries;
			ensure(NumEntries == Entries.Num());

			NumEntries = FMath::Min(NumEntries, Entries.Num());
			const FCameraValueInterpolatorSerializeParams InterpParams;
			for (int32 Index = 0; Index < NumEntries; ++Index)
			{
				Entries[Index].Interpolator->Serialize(InterpParams, Ar);
			}
		}
	}

#if UE_GAMEPLAY_CAMERAS_DEBUG
	void BuildDebugBlockImpl(TInterpolateValuesCameraNodeEvaluatorDebugBlockImpl<ValueType>& DebugBlock) const
	{
		for (const FEntry& Entry : Entries)
		{
			DebugBlock.AddEntry(Entry.Interpolator->GetTargetValue(), Entry.Interpolator->GetCurrentValue());
		}
	}
#endif  // UE_GAMEPLAY_CAMERAS_DEBUG

private:

	static TUniquePtr<TCameraValueInterpolator<ValueType>> BuildInterpolator(UCameraValueInterpolator* InterpolatorData);
	static ValueType GetDefaultValue();

private:

	struct FEntry
	{
		TUniquePtr<TCameraValueInterpolator<ValueType>> Interpolator;
		FCameraVariableID InVariableID;
		FCameraVariableID OutVariableID;
	};

	TArray<FEntry> Entries;
};

template<>
TUniquePtr<TCameraValueInterpolator<double>> TInterpolateValuesCameraNodeEvaluatorImpl<double>::BuildInterpolator(UCameraValueInterpolator* InterpolatorData)
{
	return InterpolatorData->BuildDoubleInterpolator();
}
template<>
TUniquePtr<TCameraValueInterpolator<FVector2d>> TInterpolateValuesCameraNodeEvaluatorImpl<FVector2d>::BuildInterpolator(UCameraValueInterpolator* InterpolatorData)
{
	return InterpolatorData->BuildVector2dInterpolator();
}
template<>
TUniquePtr<TCameraValueInterpolator<FVector3d>> TInterpolateValuesCameraNodeEvaluatorImpl<FVector3d>::BuildInterpolator(UCameraValueInterpolator* InterpolatorData)
{
	return InterpolatorData->BuildVector3dInterpolator();
}

template<>
double TInterpolateValuesCameraNodeEvaluatorImpl<double>::GetDefaultValue()
{
	return 0.0;
}
template<>
FVector2d TInterpolateValuesCameraNodeEvaluatorImpl<FVector2d>::GetDefaultValue()
{
	return FVector2d::ZeroVector;
}
template<>
FVector3d TInterpolateValuesCameraNodeEvaluatorImpl<FVector3d>::GetDefaultValue()
{
	return FVector3d::ZeroVector;
}

class FInterpolateValuesCameraNodeEvaluatorBase : public FCameraNodeEvaluator
{
	UE_DECLARE_CAMERA_NODE_EVALUATOR(, FInterpolateValuesCameraNodeEvaluatorBase)
	
public:

	FInterpolateValuesCameraNodeEvaluatorBase()
	{
		SetNodeEvaluatorFlags(ECameraNodeEvaluatorFlags::None);
	}
};

UE_DEFINE_CAMERA_NODE_EVALUATOR(FInterpolateValuesCameraNodeEvaluatorBase)

#define UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_BEGIN(EvaluatorClass, ValueType)\
	class F##EvaluatorClass##CameraNodeEvaluator : public FInterpolateValuesCameraNodeEvaluatorBase\
	{\
		UE_DECLARE_CAMERA_NODE_EVALUATOR_EX(, F##EvaluatorClass##CameraNodeEvaluator, FInterpolateValuesCameraNodeEvaluatorBase)\
	protected:\
		virtual void OnInitialize(const FCameraNodeEvaluatorInitializeParams& Params, FCameraNodeEvaluationResult& OutResult) override\
		{\
			Impl.InitializeImpl(GetCameraNodeAs<UInterpolateValuesCameraNodeBase>(), OutResult.VariableTable);\
		}\
		virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override\
		{\
			const FCameraValueInterpolationParams InterpParams{ Params.DeltaTime, OutResult.bIsCameraCut };\
			Impl.RunImpl(InterpParams, OutResult.VariableTable);\
		}\
		virtual void OnSerialize(const FCameraNodeEvaluatorSerializeParams& Params, FArchive& Ar) override\
		{\
			Impl.SerializeImpl(Ar);\
		}

#define UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_DEBUG_BLOCKS(EvaluatorClass)\
		virtual void OnBuildDebugBlocks(const FCameraDebugBlockBuildParams& Params, FCameraDebugBlockBuilder& Builder) override\
		{\
			using DebugBlockType = F##EvaluatorClass##CameraDebugBlock;\
			DebugBlockType& DebugBlock = Builder.AttachDebugBlock<DebugBlockType>();\
			DebugBlock.Initialize(Impl);\
		}

#define UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_END(EvaluatorClass, ValueType)\
	private:\
		TInterpolateValuesCameraNodeEvaluatorImpl<ValueType> Impl;\
	};\
	UE_DEFINE_CAMERA_NODE_EVALUATOR(F##EvaluatorClass##CameraNodeEvaluator)

#define UE_DEFINE_INTERPOLATE_VALUES_CAMERA_DEBUG_BLOCK(EvaluatorClass, ValueType)\
	class F##EvaluatorClass##CameraDebugBlock : public FCameraDebugBlock\
	{\
		UE_DECLARE_CAMERA_DEBUG_BLOCK(, F##EvaluatorClass##CameraDebugBlock)\
	public:\
		void Initialize(const TInterpolateValuesCameraNodeEvaluatorImpl<ValueType>& EvalImpl)\
		{\
			EvalImpl.BuildDebugBlockImpl(Impl);\
		}\
	protected:\
		virtual void OnSerialize(FArchive& Ar) override\
		{\
			Impl.SerializeImpl(Ar);\
		}\
		virtual void OnDebugDraw(const FCameraDebugBlockDrawParams& Params, FCameraDebugRenderer& Renderer) override\
		{\
			Impl.DebugDrawImpl(Params, Renderer);\
		}\
	private:\
		TInterpolateValuesCameraNodeEvaluatorDebugBlockImpl<ValueType> Impl;\
	};\
	UE_DEFINE_CAMERA_DEBUG_BLOCK(F##EvaluatorClass##CameraDebugBlock)

#if UE_GAMEPLAY_CAMERAS_DEBUG
	#define UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE(EvaluatorClass, ValueType)\
		UE_DEFINE_INTERPOLATE_VALUES_CAMERA_DEBUG_BLOCK(EvaluatorClass, ValueType)\
		UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_BEGIN(EvaluatorClass, ValueType)\
		UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_DEBUG_BLOCKS(EvaluatorClass)\
		UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_END(EvaluatorClass, ValueType)
#else
	#define UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE(EvaluatorClass, ValueType)\
		UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_BEGIN(EvaluatorClass, ValueType)\
		UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE_END(EvaluatorClass, ValueType)
#endif

UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE(InterpolateDoubleValues, double)
UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE(InterpolateVector2Values, FVector2d)
UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE(InterpolateVector3Values, FVector3d)

#undef UE_DEFINE_INTERPOLATE_VALUES_CAMERA_NODE

}  // namespace UE::Cameras

void UInterpolateValuesCameraNodeBase::GetCustomCameraNodeParameters(FCameraNodeParameterInfos& OutParameterInfos)
{
	const ECameraVariableType VariableType = GetVariableType();

	const FString InPrefix(TEXT("In"));
	const FString OutPrefix(TEXT("Out"));

	for (FInterpolateValuesCameraNodeEntry& Value : Values)
	{
		OutParameterInfos.AddBlendableParameter(
				FName(InPrefix + Value.Name),
				VariableType,
				nullptr,
				nullptr,
				&Value.InVariableID);

		OutParameterInfos.AddBlendableOutputParameter(
				FName(OutPrefix + Value.Name),
				VariableType,
				nullptr,
				nullptr,
				&Value.OutVariableID);
	}
}

void UInterpolateValuesCameraNodeBase::OnPreBuild(FCameraBuildContext& BuildContext)
{
	for (FInterpolateValuesCameraNodeEntry& Value : Values)
	{
		if (!ensure(Value.Guid.IsValid()))
		{
			Value.Guid = FGuid::NewGuid();
		}

		// Generate the variable ID for our output parameter only.
		// The input parameter will *receive* a variable ID by the asset builder *if* it is connected to something.
		if (!Value.OutVariableID)
		{
			Modify();

			const uint32 GuidHash = GetTypeHash(Value.Guid);
			Value.OutVariableID = FCameraVariableID::FromHashValue(GuidHash);
		}
	}
}

void UInterpolateValuesCameraNodeBase::OnBuild(FCameraObjectBuildContext& BuildContext)
{
	const ECameraVariableType VariableType = GetVariableType();

	const FString InPrefix(TEXT("In"));
	const FString OutPrefix(TEXT("Out"));

	FCameraVariableTableAllocationInfo& AllocationInfo = BuildContext.AllocationInfo.VariableTableInfo;
	for (FInterpolateValuesCameraNodeEntry& Value : Values)
	{
		FCameraVariableDefinition InputValueDefinition;
		InputValueDefinition.VariableName = (InPrefix + Value.Name);
		InputValueDefinition.VariableType = VariableType;
		InputValueDefinition.VariableID = Value.InVariableID;
		InputValueDefinition.bIsPrivate = true;
		AllocationInfo.VariableDefinitions.Add(InputValueDefinition);

		FCameraVariableDefinition OutputValueDefinition;
		OutputValueDefinition.VariableName = (OutPrefix + Value.Name);
		OutputValueDefinition.VariableType = VariableType;
		OutputValueDefinition.VariableID = Value.OutVariableID;
		OutputValueDefinition.bIsPrivate = true;
		AllocationInfo.VariableDefinitions.Add(OutputValueDefinition);
	}
}

void UInterpolateValuesCameraNodeBase::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	for (FInterpolateValuesCameraNodeEntry& Value : Values)
	{
		Value.Guid = FGuid::NewGuid();
	}
}

#if WITH_EDITOR

void UInterpolateValuesCameraNodeBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UInterpolateValuesCameraNodeBase, Values))
	{
		OnCustomCameraNodeParametersChanged(this);
	}
}

#endif  // WITH_EDITOR

FCameraNodeEvaluatorPtr UInterpolateDoubleValuesCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FInterpolateDoubleValuesCameraNodeEvaluator>();
}

FCameraNodeEvaluatorPtr UInterpolateVector2ValuesCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FInterpolateVector2ValuesCameraNodeEvaluator>();
}

FCameraNodeEvaluatorPtr UInterpolateVector3ValuesCameraNode::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FInterpolateVector3ValuesCameraNodeEvaluator>();
}

