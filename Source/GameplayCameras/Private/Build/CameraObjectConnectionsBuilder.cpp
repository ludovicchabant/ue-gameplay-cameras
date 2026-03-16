// Copyright Epic Games, Inc. All Rights Reserved.

#include "Build/CameraObjectConnectionsBuilder.h"

#include "Core/BaseCameraObject.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeHierarchy.h"
#include "Core/CameraObjectInterface.h"
#include "Core/CameraVariableAssets.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "GameplayCameras.h"

#define LOCTEXT_NAMESPACE "CameraObjectConnectionsBuilder"

namespace UE::Cameras
{

namespace Internal
{

struct FInterfaceParameterBindingBuilder
{
	UBaseCameraObject* CameraObject;
	TMap<const UCameraNode*, FCameraNodeParameterInfos> BuiltCameraNodeParameters;

	FInterfaceParameterBindingBuilder(FCameraObjectConnectionsBuilder& InOwner)
		: Owner(InOwner)
	{
		CameraObject = Owner.CameraObject;
	}

	void ReportError(FText&& ErrorMessage)
	{
		ReportError(nullptr, MoveTemp(ErrorMessage));
	}

	void ReportError(UObject* Object, FText&& ErrorMessage)
	{
		Owner.BuildContext.BuildLog.AddMessage(EMessageSeverity::Error, MoveTemp(ErrorMessage));
	}

	void SetParameterOverride(
			const FCameraVariableDefinition& VariableDefinition,
			UCameraNode* TargetNode,
			FName TargetNodeParameterName)
	{
		const FCameraNodeBlendableParameterInfo* TargetParameterInfo = FindBlendableParameter(TargetNode, TargetNodeParameterName);
		if (!TargetParameterInfo)
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"InvalidConnectionTargetProperty",
							"Invalid connection to property '{0}' on '{1}', no such property found."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName())));
			return;
		}

		bool bCompatibleTypes = (VariableDefinition.VariableType == TargetParameterInfo->VariableType);
		if (!bCompatibleTypes)
		{
			TSharedPtr<const ICameraVariableTraits> SourceTraits = FCameraVariableTable::GetVariableTraits(TargetParameterInfo->VariableType);
			if (SourceTraits)
			{
				bCompatibleTypes = SourceTraits->CanConvertFrom(VariableDefinition.VariableType);
			}
		}
		if (!bCompatibleTypes || TargetParameterInfo->BlendableStructType != VariableDefinition.BlendableStructType)
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"ConnectionVariableTypeMismatch",
							"Invalid connection to property '{0}' on '{1}', expected type '{2}' but is '{3}', "
							"and no conversion is supported between the two."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName()),
						UEnum::GetDisplayValueAsText(VariableDefinition.VariableType),
						UEnum::GetDisplayValueAsText(TargetParameterInfo->VariableType)));
			return;
		}

		if (!TargetParameterInfo->OverrideVariableID)
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"InvalidConnectionTargetVariableID",
							"Can't connect to property '{0}' on '{1}', no camera variable ID storage available."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName())));
			return;
		}

		if (TargetParameterInfo->OverrideVariableID->IsValid())
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"ConnectionTargetVariableIDAlreadyUsed",
							"Can't connect to property '{0}' on '{1}', it is already connected with something else."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName())));
			return;
		}

		ensure(VariableDefinition.VariableID.IsValid());
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(TargetNode, TargetNodeParameterName);
		if (PreviousVariableID != VariableDefinition.VariableID)
		{
			TargetNode->Modify();
		}
		*TargetParameterInfo->OverrideVariableID = VariableDefinition.VariableID;
	}

	void SetParameterOverride(
			const FCameraContextDataDefinition& DataDefinition,
			UCameraNode* TargetNode,
			FName TargetNodeParameterName)
	{
		const FCameraNodeDataParameterInfo* TargetParameterInfo = FindDataParameter(TargetNode, TargetNodeParameterName);
		if (!TargetParameterInfo)
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"InvalidConnectionTargetProperty",
							"Invalid connection to property '{0}' on '{1}', no such property found."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName())));
			return;
		}

		if (TargetParameterInfo->DataType != DataDefinition.DataType ||
				TargetParameterInfo->DataContainerType != DataDefinition.DataContainerType ||
				TargetParameterInfo->DataTypeObject != DataDefinition.DataTypeObject)
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"ConnectionDataTypeMismatch",
							"Invalid connection to property '{0}' on '{1}', expected type '{2}' but is '{3}'."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName()),
						UEnum::GetDisplayValueAsText(DataDefinition.DataType),
						UEnum::GetDisplayValueAsText(TargetParameterInfo->DataType)));
			return;
		}

		if (!TargetParameterInfo->OverrideDataID)
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"InvalidConnectionTargetDataID",
							"Can't connect to property '{0}' on '{1}', no camera data ID storage available."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName())));
			return;
		}

		if (TargetParameterInfo->OverrideDataID->IsValid())
		{
			ReportError(
					TargetNode,
					FText::Format(LOCTEXT(
							"ConnectionTargetDataIDAlreadyUsed",
							"Can't connect to property '{0}' on '{1}', it is already connected with something else."),
						FText::FromName(TargetNodeParameterName),
						FText::FromName(TargetNode->GetFName())));
			return;
		}

		ensure(DataDefinition.DataID.IsValid());
		FCameraContextDataID PreviousDataID = FindOldDrivingDataID(TargetNode, TargetNodeParameterName);
		if (PreviousDataID != DataDefinition.DataID)
		{
			TargetNode->Modify();
		}
		*TargetParameterInfo->OverrideDataID = DataDefinition.DataID;
	}

private:

	const FCameraNodeParameterInfos& GetCameraNodeParameterInfos(UCameraNode* CameraNode)
	{
		const FCameraNodeParameterInfos* CameraNodeParameters = BuiltCameraNodeParameters.Find(CameraNode);
		if (CameraNodeParameters)
		{
			return *CameraNodeParameters;
		}
		
		FCameraNodeParameterInfos& NewCameraNodeParameters = BuiltCameraNodeParameters.Add(CameraNode);
		NewCameraNodeParameters.BuildFrom(CameraNode);
		return NewCameraNodeParameters;
	}

	const FCameraNodeBlendableParameterInfo* FindBlendableParameter(UCameraNode* CameraNode, FName ParameterName)
	{
		const FCameraNodeParameterInfos& CameraNodeParameters = GetCameraNodeParameterInfos(CameraNode);
		return CameraNodeParameters.FindBlendableParameter(ParameterName);
	}

	const FCameraNodeDataParameterInfo* FindDataParameter(UCameraNode* CameraNode, FName ParameterName)
	{
		const FCameraNodeParameterInfos& CameraNodeParameters = GetCameraNodeParameterInfos(CameraNode);
		return CameraNodeParameters.FindDataParameter(ParameterName);
	}

private:

	FCameraVariableID FindOldDrivingVariableID(UObject* ForObject, FName ForParameterName)
	{
		using FDrivenParameterKey = FCameraObjectConnectionsBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForObject, ForParameterName };
		FCameraVariableID ReusedVariableID;
		Owner.OldDrivenBlendableParameters.RemoveAndCopyValue(ParameterKey, ReusedVariableID);
		return ReusedVariableID;
	}

	FCameraContextDataID FindOldDrivingDataID(UObject* ForObject, FName ForParameterName)
	{
		using FDrivenParameterKey = FCameraObjectConnectionsBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForObject, ForParameterName };
		FCameraContextDataID ReusedDataID;
		Owner.OldDrivenDataParameters.RemoveAndCopyValue(ParameterKey, ReusedDataID);
		return ReusedDataID;
	}

private:

	FCameraObjectConnectionsBuilder& Owner;
};

}  // namespace Internal

FCameraObjectConnectionsBuilder::FCameraObjectConnectionsBuilder(FCameraBuildContext& InBuildContext)
	: BuildContext(InBuildContext)
{
}

void FCameraObjectConnectionsBuilder::BuildConnections(UBaseCameraObject* InCameraObject, const FCameraNodeHierarchy& InHierarchy, bool bCollectStrayNodes)
{
	TSet<UCameraNode*> CameraNodesToGather(InHierarchy.GetFlattenedHierarchy());

	if (bCollectStrayNodes)
	{
		// Get the list of nodes, both connected and disconnected from the root hierarchy.
		// We could use AllNodeTreeObjects for that, but it only exists in editor builds, and we 
		// don't want to rely on unit tests or runtime data manipulation to have correctly populated 
		// it, so we'll try to gather any stray nodes by looking at objects outer'ed to the camera rig.
		ForEachObjectWithOuter(InCameraObject, [&CameraNodesToGather](UObject* Obj)
				{
					if (UCameraNode* CameraNode = Cast<UCameraNode>(Obj))
					{
						CameraNodesToGather.Add(CameraNode);
					}
				});
		const int32 NumStrayCameraNodes = (CameraNodesToGather.Num() - InHierarchy.Num());
		if (NumStrayCameraNodes > 0)
		{
			UE_LOGF(LogCameraSystem, Verbose, "Collected %d stray camera nodes while building camera rig '%ls'.",
					NumStrayCameraNodes, *GetPathNameSafe(CameraObject));
		}
	}

	BuildConnections(InCameraObject, CameraNodesToGather.Array());
}

void FCameraObjectConnectionsBuilder::BuildConnections(UBaseCameraObject* InCameraObject, TArrayView<UCameraNode*> InCameraObjectNodes)
{
	if (!ensure(InCameraObject))
	{
		return;
	}

	CameraObject = InCameraObject;
	CameraObjectNodes = InCameraObjectNodes;
	{
		GatherOldDrivenParameters();
		BuildConnectionsImpl();
		DiscardUnusedParameters();
	}
	CameraObject = nullptr;
	CameraObjectNodes.Reset();
}

void FCameraObjectConnectionsBuilder::GatherOldDrivenParameters()
{
	// Keep track of which blendable/data parameters were previously overriden with private IDs.
	// Then clear those private IDs. This is because it's easier to rebuild all this from a blank 
	// slate than trying to figure out what changed.
	//
	// As we rebuild things in BuildInterfaceParameterBindings, we compare to the old state to
	// figure out if we need to flag anything as modified for the current transaction.
	//
	// Note that parameters driven by user-defined variables are left alone.

	OldDrivenBlendableParameters.Reset();
	OldDrivenDataParameters.Reset();

	FCameraNodeParameterInfos CameraNodeParameters;

	for (UCameraNode* CameraNode : CameraObjectNodes)
	{
		CameraNodeParameters.BuildFrom(CameraNode);

		for (const FCameraNodeBlendableParameterInfo& BlendableParameter : CameraNodeParameters.GetBlendableParameters())
		{
			if (BlendableParameter.OverrideVariableID && BlendableParameter.OverrideVariableID->IsValid())
			{
				OldDrivenBlendableParameters.Add(
						FDrivenParameterKey{ CameraNode, BlendableParameter.ParameterName },
						*BlendableParameter.OverrideVariableID);
				*BlendableParameter.OverrideVariableID = FCameraVariableID();
			}
		}

		for (const FCameraNodeDataParameterInfo& DataParameter : CameraNodeParameters.GetDataParameters())
		{
			if (DataParameter.OverrideDataID && DataParameter.OverrideDataID->IsValid())
			{
				OldDrivenDataParameters.Add(
						FDrivenParameterKey{ CameraNode, DataParameter.ParameterName },
						*DataParameter.OverrideDataID);
				*DataParameter.OverrideDataID = FCameraContextDataID();
			}
		}
	}
}

void FCameraObjectConnectionsBuilder::BuildConnectionsImpl()
{
	// Now we process all the connections that pass a variable or context data from one thing (source) to another thing
	// (destination) through the variable/data tables.

	const FString CameraObjectName = CameraObject->GetName();
	const FString CameraObjectPathName = CameraObject->GetPathName();

	Internal::FInterfaceParameterBindingBuilder Builder(*this);

	for (const FCameraObjectConnection& Connection : CameraObject->Connections.Connections)
	{
		if (Connection.Source == nullptr || Connection.Target == nullptr)
		{
			continue;
		}

		UCameraNode* TargetNode = Cast<UCameraNode>(Connection.Target);
		if (!TargetNode)
		{
			continue;
		}

		if (const UCameraObjectInterfaceParameterGetter* ParameterGetter = Cast<UCameraObjectInterfaceParameterGetter>(Connection.Source))
		{
			const FGuid& ParameterGuid = ParameterGetter->ParameterGuid;
			if (ParameterGuid.IsValid())
			{
				if (UCameraObjectInterfaceBlendableParameter* BlendableParameter = CameraObject->Interface.FindBlendableParameterByGuid(ParameterGuid))
				{
					const FCameraVariableDefinition VariableDefinition = BlendableParameter->GetVariableDefinition();
					Builder.SetParameterOverride(VariableDefinition, TargetNode, Connection.TargetPropertyName);
				}
				else if (UCameraObjectInterfaceDataParameter* DataParameter = CameraObject->Interface.FindDataParameterByGuid(ParameterGuid))
				{
					const FCameraContextDataDefinition DataDefinition = DataParameter->GetDataDefinition();
					Builder.SetParameterOverride(DataDefinition, TargetNode, Connection.TargetPropertyName);
				}
			}
		}
		else if (const UCameraVariableAssetGetter* VariableGetter = Cast<UCameraVariableAssetGetter>(Connection.Source))
		{
			const UCameraVariableAsset* Variable = VariableGetter->Variable;
			if (Variable)
			{
				const FCameraVariableDefinition VariableDefinition = Variable->GetVariableDefinition();
				Builder.SetParameterOverride(VariableDefinition, TargetNode, Connection.TargetPropertyName);
			}
		}
	}
}

void FCameraObjectConnectionsBuilder::DiscardUnusedParameters()
{
	// Now that we've rebuilt all exposed parameters, anything left from the old list 
	// must be discarded. These are nodes and properties that used to be driven by
	// variables and now aren't, so we need to flag them as modified.

	for (TPair<FDrivenParameterKey, FCameraVariableID> Pair : OldDrivenBlendableParameters)
	{
		UObject* Target = Pair.Key.Key;
		Target->Modify();
	}
	OldDrivenBlendableParameters.Reset();

	for (TPair<FDrivenParameterKey, FCameraContextDataID> Pair : OldDrivenDataParameters)
	{
		UObject* Target = Pair.Key.Key;
		Target->Modify();
	}
	OldDrivenDataParameters.Reset();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

