// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAssetBuilder.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableReferences.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "GameplayCamerasDelegates.h"
#include "Logging/TokenizedMessage.h"
#include "UObject/Object.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetBuilder"

namespace UE::Cameras
{

namespace Internal
{

struct FInterfaceParameterBindingBuilder
{
	UCameraRigAsset* CameraRig;

	FInterfaceParameterBindingBuilder(FCameraRigAssetBuilder& InOwner)
		: Owner(InOwner)
	{
		CameraRig = Owner.CameraRig;
	}

	void ReportError(FText&& ErrorMessage)
	{
		ReportError(nullptr, MoveTemp(ErrorMessage));
	}

	void ReportError(UObject* Object, FText&& ErrorMessage)
	{
		Owner.BuildLog.AddMessage(EMessageSeverity::Error, MoveTemp(ErrorMessage));
	}

	template<typename CameraParameterOrVariableReferenceType>
	void SetCameraParameterOrVariableReferenceOverride(
			const UCameraRigBlendableParameter* BlendableParameter,
			FStructProperty* TargetProperty,
			CameraParameterOrVariableReferenceType* CameraParameterOrVariableReferencePtr)
	{
		using VariableAssetType = typename CameraParameterOrVariableReferenceType::VariableAssetType;
		using ValueType = typename VariableAssetType::ValueType;

		ensure(BlendableParameter->PrivateVariableID.IsValid());
		ensure(BlendableParameter->TargetPropertyName == TargetProperty->GetFName());

		const bool bIsValid = CheckIfParameterCanBeOverridden(BlendableParameter, CameraParameterOrVariableReferencePtr);
		if (!bIsValid)
		{
			return;
		}

		UCameraNode* TargetNode = BlendableParameter->Target;
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(TargetProperty->GetFName(), TargetNode);
		if (PreviousVariableID != BlendableParameter->PrivateVariableID)
		{
			TargetNode->Modify();
		}

		CameraParameterOrVariableReferencePtr->VariableID = BlendableParameter->PrivateVariableID;
	}

	template<typename VariableAssetType>
	void SetCustomBlendableParameterOverride(
			const UCameraRigBlendableParameter* BlendableParameter,
			const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& CustomParameter)
	{
		using ValueType = typename VariableAssetType::ValueType;

		ensure(BlendableParameter->PrivateVariableID.IsValid());
		ensure(BlendableParameter->TargetPropertyName == CustomParameter.ParameterName);

		const bool bIsValid = CheckIfParameterCanBeOverridden(BlendableParameter, CustomParameter);
		if (!bIsValid)
		{
			return;
		}

		UCameraNode* TargetNode = BlendableParameter->Target;
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(CustomParameter.ParameterName, TargetNode);
		if (PreviousVariableID != BlendableParameter->PrivateVariableID)
		{
			TargetNode->Modify();
		}

		(*CustomParameter.OverrideVariableID) = BlendableParameter->PrivateVariableID;
	}

	void SetCustomBlendableStructParameterOverride(
			const UCameraRigBlendableParameter* BlendableParameter,
			const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& CustomParameter)
	{
		ensure(BlendableParameter->PrivateVariableID.IsValid());
		ensure(BlendableParameter->TargetPropertyName == CustomParameter.ParameterName);

		const bool bIsValid = CheckIfParameterCanBeOverridden(BlendableParameter, CustomParameter.OverrideVariableID);
		if (!bIsValid)
		{
			return;
		}

		// Also ensure the struct type is compatible.
		if (CustomParameter.BlendableStructType != BlendableParameter->BlendableStructType)
		{
			ReportError(BlendableParameter->Target,
					FText::Format(
						LOCTEXT(
							"IncompatibleBlendableStructType", 
							"Invalid interface parameter '{0}', driving property '{1}' on '{2}': expected type {3} but was {4}"),
						FText::FromString(BlendableParameter->InterfaceParameterName),
						FText::FromName(BlendableParameter->TargetPropertyName),
						FText::FromName(BlendableParameter->Target->GetFName()),
#if WITH_EDITORONLY_DATA
						CustomParameter.BlendableStructType->GetDisplayNameText(),
						BlendableParameter->BlendableStructType->GetDisplayNameText()
#else
						FText::FromName(CustomParameter.BlendableStructType->GetFName()),
						FText::FromName(BlendableParameter->BlendableStructType->GetFName())
#endif
						));
			return;
		}

		UCameraNode* TargetNode = BlendableParameter->Target;
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(CustomParameter.ParameterName, TargetNode);
		if (PreviousVariableID != BlendableParameter->PrivateVariableID)
		{
			TargetNode->Modify();
		}

		(*CustomParameter.OverrideVariableID) = BlendableParameter->PrivateVariableID;
	}

	void SetDataContextPropertyOverride(
			const UCameraRigDataParameter* DataParameter,
			FProperty* TargetProperty,
			FCameraContextDataID* OverrideDataID)
	{
		ensure(DataParameter->PrivateDataID);

		const bool bIsValid = CheckIfParameterCanBeOverridden(DataParameter, OverrideDataID);
		if (!bIsValid)
		{
			return;
		}

		UCameraNode* TargetNode = DataParameter->Target;
		FCameraContextDataID PreviousDataID = FindOldDrivingDataID(TargetProperty->GetFName(), TargetNode);
		if (PreviousDataID != DataParameter->PrivateDataID)
		{
			TargetNode->Modify();
		}

		(*OverrideDataID) = DataParameter->PrivateDataID;
	}

	void SetCustomDataParameterOverride(
			const UCameraRigDataParameter* DataParameter,
			const FCustomCameraNodeParameterInfos::FDataParameterInfo& CustomParameter)
	{
		ensure(DataParameter->PrivateDataID);
		ensure(DataParameter->TargetPropertyName == CustomParameter.ParameterName);

		const bool bIsValid = CheckIfParameterCanBeOverridden(DataParameter, CustomParameter.OverrideDataID);
		if (!bIsValid)
		{
			return;
		}

		UCameraNode* TargetNode = DataParameter->Target;
		FCameraContextDataID PreviousDataID = FindOldDrivingDataID(CustomParameter.ParameterName, TargetNode);
		if (PreviousDataID != DataParameter->PrivateDataID)
		{
			TargetNode->Modify();
		}

		(*CustomParameter.OverrideDataID) = DataParameter->PrivateDataID;
	}

private:

	FCameraVariableID FindOldDrivingVariableID(FName ForParameterName, UCameraNode* ForCameraNode)
	{
		using FDrivenParameterKey = FCameraRigAssetBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterName, ForCameraNode };
		FCameraVariableID ReusedVariableID;
		Owner.OldDrivenBlendableParameters.RemoveAndCopyValue(ParameterKey, ReusedVariableID);
		return ReusedVariableID;
	}

	FCameraContextDataID FindOldDrivingDataID(FName ForParameterName, UCameraNode* ForCameraNode)
	{
		using FDrivenParameterKey = FCameraRigAssetBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterName, ForCameraNode };
		FCameraContextDataID ReusedDataID;
		Owner.OldDrivenDataParameters.RemoveAndCopyValue(ParameterKey, ReusedDataID);
		return ReusedDataID;
	}

	template<typename CameraParameterOrVariableReferenceType>
	bool CheckIfParameterCanBeOverridden(
			const UCameraRigBlendableParameter* BlendableParameter, 
			CameraParameterOrVariableReferenceType* CameraParameterOrVariableReference)
	{
		if (CameraParameterOrVariableReference->Variable != nullptr)
		{
			ReportError(BlendableParameter->Target,
					FText::Format(
						LOCTEXT(
							"BlendableParameterDrivenTwice", 
							"Camera node parameter '{0}.{1}' is both exposed and driven by a variable!"),
						FText::FromName(BlendableParameter->Target->GetFName()), 
						FText::FromName(BlendableParameter->TargetPropertyName)));
			return false;
		}

		return CheckIfParameterCanBeOverridden(BlendableParameter, &CameraParameterOrVariableReference->VariableID);
	}

	bool CheckIfParameterCanBeOverridden(
			const UCameraRigBlendableParameter* BlendableParameter, 
			const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& CustomParameter)
	{
		if (CustomParameter.OverrideVariable != nullptr)
		{
			ReportError(BlendableParameter->Target,
					FText::Format(
						LOCTEXT(
							"BlendableParameterDrivenTwice", 
							"Camera node parameter '{0}.{1}' is both exposed and driven by a variable!"),
						FText::FromName(BlendableParameter->Target->GetFName()), 
						FText::FromName(BlendableParameter->TargetPropertyName)));
			return false;
		}

		return CheckIfParameterCanBeOverridden(BlendableParameter, CustomParameter.OverrideVariableID);
	}

	bool CheckIfParameterCanBeOverridden(
			const UCameraRigBlendableParameter* BlendableParameter, 
			FCameraVariableID* VariableID)
	{
		if (!VariableID)
		{
			ReportError(BlendableParameter->Target,
					FText::Format(
						LOCTEXT(
							"BlendableParameterMissingOverrideID", 
							"Camera node parameter '{0}.{1}' cannot be overriden by a parameter"),
						FText::FromName(BlendableParameter->Target->GetFName()), 
						FText::FromName(BlendableParameter->TargetPropertyName)));
			return false;
		}
		if (VariableID->IsValid())
		{
			ReportError(BlendableParameter->Target,
					FText::Format(
						LOCTEXT(
							"BlendableParameterDrivenTwice", 
							"Camera node parameter '{0}.{1}' is somehow overriden twice!"),
						FText::FromName(BlendableParameter->Target->GetFName()), 
						FText::FromName(BlendableParameter->TargetPropertyName)));
			return false;
		}
		return true;
	}

	bool CheckIfParameterCanBeOverridden(
			const UCameraRigDataParameter* DataParameter,
			FCameraContextDataID* DataID)
	{
		if (!DataID)
		{
			ReportError(DataParameter->Target,
					FText::Format(
						LOCTEXT(
							"DataParameterMissingOverrideID", 
							"Camera node parameter '{0}.{1}' cannot be overriden by a parameter"),
						FText::FromName(DataParameter->Target->GetFName()), 
						FText::FromName(DataParameter->TargetPropertyName)));
			return false;
		}
		if (DataID->IsValid())
		{
			ReportError(DataParameter->Target,
					FText::Format(
						LOCTEXT(
							"DataParameterDrivenTwice",
							"Camera node parameter '{0}.{1}' is somehow overriden twice!"),
						FText::FromName(DataParameter->Target->GetFName()), 
						FText::FromName(DataParameter->TargetPropertyName)));
			return false;
		}
		return true;
	}

private:

	FCameraRigAssetBuilder& Owner;
};

void AddVariableToAllocationInfo(UCameraVariableAsset* Variable, FCameraVariableTableAllocationInfo& AllocationInfo)
{
	if (Variable)
	{
		FCameraVariableDefinition VariableDefinition = Variable->GetVariableDefinition();
		AllocationInfo.VariableDefinitions.Add(VariableDefinition);
	}
}

void AddVariableToAllocationInfo(FCameraVariableID VariableID, ECameraVariableType VariableType, const UScriptStruct* BlendableStructType, FCameraVariableTableAllocationInfo& AllocationInfo)
{
	if (VariableID)
	{
		FCameraVariableDefinition VariableDefinition;
		VariableDefinition.VariableID = VariableID;
		VariableDefinition.VariableType = VariableType;
		VariableDefinition.BlendableStructType = BlendableStructType;
		VariableDefinition.bIsPrivate = true;
		VariableDefinition.bIsInput = true;
		AllocationInfo.VariableDefinitions.Add(VariableDefinition);
	}
}

void AddContextDataToAllocationInfo(FCameraContextDataID DataID, ECameraContextDataType DataType, const UObject* DataTypeObject, FCameraContextDataTableAllocationInfo& AllocationInfo)
{
	if (DataID)
	{
		FCameraContextDataDefinition DataDefinition;
		DataDefinition.DataID = DataID;
		DataDefinition.DataType = DataType;
		DataDefinition.DataTypeObject = DataTypeObject;
		AllocationInfo.DataDefinitions.Add(DataDefinition);
	}
}

}  // namespace Internal

FCameraRigAssetBuilder::FCameraRigAssetBuilder(FCameraBuildLog& InBuildLog)
	: BuildLog(InBuildLog)
{
}

void FCameraRigAssetBuilder::BuildCameraRig(UCameraRigAsset* InCameraRig)
{
	BuildCameraRig(InCameraRig, FCustomBuildStep::CreateLambda([](UCameraRigAsset*, FCameraBuildLog&) {}));
}

void FCameraRigAssetBuilder::BuildCameraRig(UCameraRigAsset* InCameraRig, FCustomBuildStep InCustomBuildStep)
{
	if (!ensure(InCameraRig))
	{
		return;
	}

	CameraRig = InCameraRig;
	BuildLog.SetLoggingPrefix(CameraRig->GetPathName() + TEXT(": "));
	{
		BuildCameraRigImpl();

		InCustomBuildStep.ExecuteIfBound(CameraRig, BuildLog);

		CameraRig->EventHandlers.Notify(&ICameraRigAssetEventHandler::OnCameraRigBuilt, CameraRig);
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();

	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().Broadcast(CameraRig);
}

void FCameraRigAssetBuilder::BuildCameraRigImpl()
{
	if (!CameraRig->RootNode)
	{
		BuildLog.AddMessage(EMessageSeverity::Error, CameraRig, 
				FText::Format(LOCTEXT("MissingRootNode", "Camera rig '{0}' has no root node."), 
					FText::FromString(GetPathNameSafe(CameraRig))));
		return;
	}

	BuildCameraNodeHierarchy();

	CallPreBuild();

	GatherOldDrivenParameters();
	BuildInterfaceParameters();
	BuildInterfaceParameterBindings();
	DiscardUnusedParameters();

	CallBuild();

	BuildParameterDefinitions();
	BuildDefaultParameters();
}

void FCameraRigAssetBuilder::BuildCameraNodeHierarchy()
{
	// Build a flat list of the camera rig's node hierarchy. It's easier to iterate during
	// our build process.
	CameraNodeHierarchy.Build(CameraRig);

#if WITH_EDITORONLY_DATA
	// Check that all the camera nodes that are in the tree are also inside the camera 
	// rig's AllNodeTreeObjects. This shouldn't happen unless someone added camera nodes
	// directly via C++, or if there's a bug in the camera rig editor code, so emit a
	// warning if that happens.
	TSet<UObject*> MissingNodeTreeObjects;
	if (CameraNodeHierarchy.FindMissingConnectableObjects(ObjectPtrDecay(CameraRig->AllNodeTreeObjects), MissingNodeTreeObjects))
	{
		BuildLog.AddMessage(EMessageSeverity::Warning, 
				FText::Format(
					LOCTEXT("AllNodeTreeObjectsMismatch", 
						"Found {0} nodes missing from the internal list. Please re-save the asset."),
					MissingNodeTreeObjects.Num()));
		CameraRig->AllNodeTreeObjects.Append(MissingNodeTreeObjects.Array());
	}
#endif  // WITH_EDITORONLY_DATA
}

void FCameraRigAssetBuilder::CallPreBuild()
{
	for (UCameraNode* CameraNode : CameraNodeHierarchy.GetFlattenedHierarchy())
	{
		CameraNode->PreBuild(BuildLog);
	}
}

void FCameraRigAssetBuilder::GatherOldDrivenParameters()
{
	// Keep track of which blendable/data parameters were previously overriden with private IDs.
	// Then clear those private IDs. This is because it's easier to rebuild all this from a blank 
	// slate than trying to figure out what changed.
	//
	// As we rebuild things in BuildInterfaceParameterBindings, we compare to the old state to
	// figure out if we need to flag anything as modified for the current transaction.
	//
	// Note that parameters driven by user-defined variables are left alone.

	// First, get the list of nodes, both connected and disconnected from the root hierarchy.
	// We could use AllNodeTreeObjects for that, but it only exists in editor builds, and we 
	// don't want to rely on unit tests or runtime data manipulation to have correctly populated 
	// it, so we'll try to gather any stray nodes by looking at objects outer'ed to the camera rig.
	TSet<UCameraNode*> CameraNodesToGather(CameraNodeHierarchy.GetFlattenedHierarchy());
	ForEachObjectWithOuter(CameraRig, [&CameraNodesToGather](UObject* Obj)
			{
				if (UCameraNode* CameraNode = Cast<UCameraNode>(Obj))
				{
					CameraNodesToGather.Add(CameraNode);
				}
			});
	const int32 NumStrayCameraNodes = (CameraNodesToGather.Num() - CameraNodeHierarchy.Num());
	if (NumStrayCameraNodes > 0)
	{
		UE_LOG(LogCameraSystem, Verbose, TEXT("Collected %d stray camera nodes while building camera rig '%s'."),
				NumStrayCameraNodes, *GetPathNameSafe(CameraRig));
	}

	// Second, gather all driven properties, and null them out.
	OldDrivenBlendableParameters.Reset();
	OldDrivenDataParameters.Reset();

	for (UCameraNode* CameraNode : CameraNodesToGather)
	{
		UClass* CameraNodeClass = CameraNode->GetClass();
		
		for (TFieldIterator<FProperty> It(CameraNodeClass); It; ++It)
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(*It);
			if (!StructProperty)
			{
				continue;
			}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			if (StructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
			{\
				auto* CameraParameterPtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraParameter>(CameraNode);\
				if (CameraParameterPtr->VariableID.IsValid() && !CameraParameterPtr->Variable)\
				{\
					OldDrivenBlendableParameters.Add(\
							FDrivenParameterKey{ StructProperty->GetFName(), CameraNode },\
							CameraParameterPtr->VariableID);\
					CameraParameterPtr->VariableID = FCameraVariableID();\
				}\
			}\
			else if (StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
			{\
				auto* VariableReferencePtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(CameraNode);\
				if (VariableReferencePtr->VariableID.IsValid() && !VariableReferencePtr->Variable)\
				{\
					OldDrivenBlendableParameters.Add(\
							FDrivenParameterKey{ StructProperty->GetFName(), CameraNode },\
							VariableReferencePtr->VariableID);\
					VariableReferencePtr->VariableID = FCameraVariableID();\
				}\
			}\
			else
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
			{
				// Other struct type...
			}
		}

		if (ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNode))
		{
			FCustomCameraNodeParameterInfos CustomParameters;
			CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);

			for (const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& BlendableParameter : CustomParameters.BlendableParameters)
			{
				if (!BlendableParameter.OverrideVariableID)
				{
					continue;
				}

				if (BlendableParameter.OverrideVariableID->IsValid())
				{
					OldDrivenBlendableParameters.Add(
							FDrivenParameterKey{ BlendableParameter.ParameterName, CameraNode },
							*BlendableParameter.OverrideVariableID);
					(*BlendableParameter.OverrideVariableID) = FCameraVariableID();
				}
			}

			for (const FCustomCameraNodeParameterInfos::FDataParameterInfo& DataParameter : CustomParameters.DataParameters)
			{
				if (!DataParameter.OverrideDataID)
				{
					continue;
				}

				if (DataParameter.OverrideDataID->IsValid())
				{
					OldDrivenDataParameters.Add(
							FDrivenParameterKey{ DataParameter.ParameterName, CameraNode },
							*DataParameter.OverrideDataID);
					(*DataParameter.OverrideDataID) = FCameraContextDataID();
				}
			}
		}
	}
}

void FCameraRigAssetBuilder::BuildInterfaceParameters()
{
	using namespace Internal;

	for (auto It = CameraRig->Interface.BlendableParameters.CreateIterator(); It; ++It)
	{
		UCameraRigBlendableParameter* BlendableParameter(*It);

		// Basic validations.
		if (!BlendableParameter)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					CameraRig,
					LOCTEXT("InvalidBlendableParameter", "Invalid interface parameter was found and removed."));

			CameraRig->Modify();
			It.RemoveCurrent();

			continue;
		}

		if (BlendableParameter->InterfaceParameterName.IsEmpty())
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					BlendableParameter,
					LOCTEXT(
						"InvalidBlendableParameterName",
						"Invalid interface parameter name."));
			continue;
		}

		// Create a new private variable ID for this interface parameter. Flag the parameter as changed if
		// the ID is different, generally when it's a new parameter.
		FCameraVariableID VariableID = FCameraVariableID::FromHashValue(GetTypeHash(BlendableParameter->GetGuid()));
		if (BlendableParameter->PrivateVariableID != VariableID)
		{
			BlendableParameter->Modify();
			BlendableParameter->PrivateVariableID = VariableID;
		}
	}

	for (auto It = CameraRig->Interface.DataParameters.CreateIterator(); It; ++It)
	{
		UCameraRigDataParameter* DataParameter(*It);

		// Basic validations.
		if (!DataParameter)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					CameraRig,
					LOCTEXT("InvalidDataParameter", "Invalid interface parameter was found and removed."));

			CameraRig->Modify();
			It.RemoveCurrent();

			continue;
		}

		if (DataParameter->InterfaceParameterName.IsEmpty())
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					DataParameter,
					LOCTEXT(
						"InvalidDataParameterName",
						"Invalid interface parameter name."));
			continue;
		}

		// Create a new private data ID for this interface parameter. Flag the parameter as changed if
		// the ID is different, generally when it's a new parameter.
		FCameraContextDataID DataID = FCameraContextDataID::FromHashValue(GetTypeHash(DataParameter->GetGuid()));
		if (DataParameter->PrivateDataID != DataID)
		{
			DataParameter->Modify();
			DataParameter->PrivateDataID = DataID;
		}
	}
}

void FCameraRigAssetBuilder::BuildInterfaceParameterBindings()
{
	using FBuiltDrivenParameter = TTuple<UCameraNode*, FName>;
	TSet<FBuiltDrivenParameter> BuiltDrivenParameters;

	const FString CameraRigName = CameraRig->GetName();
	const FString CameraRigPathName = CameraRig->GetPathName();

	for (const UCameraRigBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
	{
		// Basic validations.
		if (!BlendableParameter->Target)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					BlendableParameter,
					LOCTEXT(
						"InvalidBlendableParameterTarget",
						"Invalid interface parameter: it has no target node."));
			continue;
		}
		if (BlendableParameter->TargetPropertyName.IsNone())
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					BlendableParameter,
					LOCTEXT(
						"InvalidBlendableParameterTargetPropertyName", 
						"Invalid interface parameter: it has not target property name."));
			continue;
		}
		if (BlendableParameter->InterfaceParameterName.IsEmpty())
		{
			continue;
		}

		// Check duplicate bindings.
		FBuiltDrivenParameter NewDrivenParameter(BlendableParameter->Target, BlendableParameter->TargetPropertyName);
		if (BuiltDrivenParameters.Contains(NewDrivenParameter))
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					FText::Format(LOCTEXT(
						"BlendableParameterTargetCollision",
						"Multiple interface parameters targeting property '{0}' on camera node '{1}'. Ignoring duplicates."),
						FText::FromName(BlendableParameter->TargetPropertyName),
						FText::FromName(BlendableParameter->Target->GetFName())));
			continue;
		}
		BuiltDrivenParameters.Add(NewDrivenParameter);

		// See if this interface parameter is overriding a camera node parameter.
		// Otherwise, maybe it's targeting a camera rig node's override for an inner rig interface parameter.
		if (SetupCustomBlendableParameterOverride(BlendableParameter))
		{
			// Implicit continue.
		}
		else if (SetupCameraParameterOrVariableReferenceOverride(BlendableParameter))
		{
			// Implicit continue.
		}
		else
		{
			UCameraNode* Target = BlendableParameter->Target;
			BuildLog.AddMessage(EMessageSeverity::Error,
					Target,
					FText::Format(LOCTEXT(
						"InvalidBlendableParameterTargetProperty",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but no such property found."),
						FText::FromString(BlendableParameter->InterfaceParameterName), 
						FText::FromName(BlendableParameter->TargetPropertyName),
						FText::FromName(Target->GetFName())));
		}
	}

	for (const UCameraRigDataParameter* DataParameter : CameraRig->Interface.DataParameters)
	{
		// Basic validations.
		if (!DataParameter->Target)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					DataParameter,
					LOCTEXT(
						"InvalidDataParameterTarget",
						"Invalid interface parameter: it has no target node."));
			continue;
		}
		if (DataParameter->TargetPropertyName.IsNone())
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					DataParameter,
					LOCTEXT(
						"InvalidDataParameterTargetPropertyName", 
						"Invalid interface parameter: it has not target property name."));
			continue;
		}
		if (DataParameter->InterfaceParameterName.IsEmpty())
		{
			continue;
		}

		if (SetupCustomDataParameterOverride(DataParameter))
		{
			// Implicit continue.
		}
		else if (SetupDataContextPropertyOverride(DataParameter))
		{
			// Implicit continue.
		}
		else
		{
			UCameraNode* Target = DataParameter->Target;
			BuildLog.AddMessage(EMessageSeverity::Error,
					Target,
					FText::Format(LOCTEXT(
						"InvalidDataParameterTargetProperty",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but no such property found."),
						FText::FromString(DataParameter->InterfaceParameterName), 
						FText::FromName(DataParameter->TargetPropertyName),
						FText::FromName(Target->GetFName())));
		}
	}
}

bool FCameraRigAssetBuilder::SetupCameraParameterOrVariableReferenceOverride(const UCameraRigBlendableParameter* BlendableParameter)
{
	using namespace Internal;

	// Here we hook up interface parameters connected to a camera node property. This property is supposed
	// to be of one of the camera parameter types (FBooleanCameraParameter, FInteger32CameraParameter, etc.)
	// so they have both a fixed value (bool, int32, etc.) and a "private variable" which we will set to the 
	// private variable of the given interface parameter, checking that the types match (UBooleanCameraVariable, 
	// UInteger32CameraVariable, etc.)

	UCameraNode* Target = BlendableParameter->Target;
	UClass* TargetClass = Target->GetClass();
	FProperty* TargetProperty = TargetClass->FindPropertyByName(BlendableParameter->TargetPropertyName);
	if (!TargetProperty)
	{
		// No match, try something else.
		return false;
	}

	FStructProperty* TargetStructProperty = CastField<FStructProperty>(TargetProperty);
	if (!TargetStructProperty)
	{
		BuildLog.AddMessage(EMessageSeverity::Error,
				Target,
				FText::Format(LOCTEXT(
						"InvalidCameraNodeParameter",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but it's not a camera parameter."),
					FText::FromString(BlendableParameter->InterfaceParameterName), 
					FText::FromName(BlendableParameter->TargetPropertyName),
					FText::FromName(Target->GetFName())));
		return true;
	}

	// Get the type of the camera parameter by matching the struct against all the types we support,
	// and create a private camera variable asset to drive its value.
	FInterfaceParameterBindingBuilder Builder(*this);
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	if (TargetStructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
	{\
		auto* CameraParameterPtr = TargetStructProperty->ContainerPtrToValuePtr<F##ValueName##CameraParameter>(Target);\
		Builder.SetCameraParameterOrVariableReferenceOverride<F##ValueName##CameraParameter>(\
				BlendableParameter, TargetStructProperty, CameraParameterPtr\
				);\
	}\
	else if (TargetStructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
	{\
		auto* VariableReferencePtr = TargetStructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(Target);\
		Builder.SetCameraParameterOrVariableReferenceOverride<F##ValueName##CameraVariableReference>(\
				BlendableParameter, TargetStructProperty, VariableReferencePtr\
				);\
	}\
	else
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	{
		BuildLog.AddMessage(EMessageSeverity::Error,
				BlendableParameter,
				FText::Format(LOCTEXT(
						"InvalidCameraNodeParameter",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but it's not a camera parameter."),
					FText::FromString(BlendableParameter->InterfaceParameterName), 
					FText::FromName(BlendableParameter->TargetPropertyName),
					FText::FromName(Target->GetFName())));
	}

	return true;
}

bool FCameraRigAssetBuilder::SetupCustomBlendableParameterOverride(const UCameraRigBlendableParameter* BlendableParameter)
{
	using namespace Internal;

	ICustomCameraNodeParameterProvider* Target = Cast<ICustomCameraNodeParameterProvider>(BlendableParameter->Target);
	if (!Target)
	{
		// No match, try something else.
		return false;
	}

	// Look for a parameter override matching the target name.
	// TODO: we're querying the list of custom parameters every time, we may want to cache it for this phase.
	FCustomCameraNodeParameterInfos CustomParameters;
	Target->GetCustomCameraNodeParameters(CustomParameters);

	FCustomCameraNodeParameterInfos::FBlendableParameterInfo* TargetCustomParameter = 
		CustomParameters.BlendableParameters.FindByPredicate(
			[BlendableParameter](FCustomCameraNodeParameterInfos::FBlendableParameterInfo& CustomParameter)
			{
				return CustomParameter.ParameterName == BlendableParameter->TargetPropertyName;
			});
	if (!TargetCustomParameter)
	{
		// No match, try something else.
		return false;
	}

	FInterfaceParameterBindingBuilder Builder(*this);
	switch (TargetCustomParameter->ParameterType)
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		case ECameraVariableType::ValueName:\
			Builder.SetCustomBlendableParameterOverride<U##ValueName##CameraVariable>(BlendableParameter, *TargetCustomParameter);\
			break;
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		case ECameraVariableType::BlendableStruct:
			Builder.SetCustomBlendableStructParameterOverride(BlendableParameter, *TargetCustomParameter);
			break;
	}

	return true;
}

bool FCameraRigAssetBuilder::SetupDataContextPropertyOverride(const UCameraRigDataParameter* DataParameter)
{
	using namespace Internal;

	UCameraNode* Target = DataParameter->Target;
	UClass* TargetClass = Target->GetClass();
	FProperty* TargetProperty = TargetClass->FindPropertyByName(DataParameter->TargetPropertyName);
	if (!TargetProperty)
	{
		// No match, try something else.
		return false;
	}

	const FName TargetDataIDPropertyName = FName(TargetProperty->GetName() + TEXT("DataID"));
	FStructProperty* TargetDataIDProperty = CastField<FStructProperty>(TargetClass->FindPropertyByName(TargetDataIDPropertyName));
	if (!TargetDataIDProperty || TargetDataIDProperty->Struct != FCameraContextDataID::StaticStruct())
	{
		UE_LOG(LogCameraSystem, Error,
				TEXT("Interface parameter '{0}' is driving data context property '{1}' on '{2}' "
					 "but no FCameraContextDataID property '{3}' was found to store the override ID."),
				*DataParameter->InterfaceParameterName,
				*DataParameter->TargetPropertyName.ToString(),
				*Target->GetName(),
				*TargetDataIDPropertyName.ToString());
		return false;
	}

	FCameraContextDataID* OverrideDataID = TargetDataIDProperty->ContainerPtrToValuePtr<FCameraContextDataID>(Target);

	FInterfaceParameterBindingBuilder Builder(*this);
	Builder.SetDataContextPropertyOverride(DataParameter, TargetProperty, OverrideDataID);

	return true;
}

bool FCameraRigAssetBuilder::SetupCustomDataParameterOverride(const UCameraRigDataParameter* DataParameter)
{
	using namespace Internal;

	ICustomCameraNodeParameterProvider* Target = Cast<ICustomCameraNodeParameterProvider>(DataParameter->Target);
	if (!Target)
	{
		// No match, try something else.
		return false;
	}

	FCustomCameraNodeParameterInfos CustomParameters;
	Target->GetCustomCameraNodeParameters(CustomParameters);

	FCustomCameraNodeParameterInfos::FDataParameterInfo* TargetCustomParameter =
		CustomParameters.DataParameters.FindByPredicate(
				[DataParameter](FCustomCameraNodeParameterInfos::FDataParameterInfo& CustomParameter)
				{
					return CustomParameter.ParameterName == DataParameter->TargetPropertyName;
				});
	if (!TargetCustomParameter)
	{
		// No match, try something else.
		return false;
	}

	FInterfaceParameterBindingBuilder Builder(*this);
	Builder.SetCustomDataParameterOverride(DataParameter, *TargetCustomParameter);

	return true;
}

void FCameraRigAssetBuilder::DiscardUnusedParameters()
{
	// Now that we've rebuilt all exposed parameters, anything left from the old list 
	// must be discarded. These are nodes and properties that used to be driven by
	// variables and now aren't, so we need to flag them as modified.

	for (TPair<FDrivenParameterKey, FCameraVariableID> Pair : OldDrivenBlendableParameters)
	{
		UCameraNode* Target = Pair.Key.Value;
		Target->Modify();
	}
	OldDrivenBlendableParameters.Reset();

	for (TPair<FDrivenParameterKey, FCameraContextDataID> Pair : OldDrivenDataParameters)
	{
		UCameraNode* Target = Pair.Key.Value;
		Target->Modify();
	}
	OldDrivenDataParameters.Reset();
}

void FCameraRigAssetBuilder::CallBuild()
{
	FCameraRigBuildContext BuildContext(BuildLog);

	// Build a mock tree of evaluators.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = CameraRig->RootNode;
	FCameraNodeEvaluatorStorage Storage;
	Storage.BuildEvaluatorTree(BuildParams);

	// Get the size of the evaluators' allocation.
	Storage.GetAllocationInfo(BuildContext.AllocationInfo.EvaluatorInfo);

	// Call Build() on all camera nodes in the hierarchy (detached/orphaned camera nodes don't get called).
	for (UCameraNode* CameraNode : CameraNodeHierarchy.GetFlattenedHierarchy())
	{
		CallBuild(BuildContext, CameraNode);
	}

	// Add parameters to the allocation info.
	BuildParametersAllocationInfo(BuildContext);

	// Set the final allocation info on the camera rig asset.
	if (CameraRig->AllocationInfo != BuildContext.AllocationInfo)
	{
		CameraRig->Modify();
		CameraRig->AllocationInfo = BuildContext.AllocationInfo;
	}
}

void FCameraRigAssetBuilder::CallBuild(FCameraRigBuildContext& BuildContext, UCameraNode* CameraNode)
{
	using namespace UE::Cameras::Internal;

	// Look for properties that are camera parameters, and gather what camera variables they reference. 
	// This is only for user-defined variable overrides. We will do the same for exposed camera rig
	// parameters later, in BuildParametersAllocationInfo.
	UClass* CameraNodeClass = CameraNode->GetClass();
	FCameraRigAllocationInfo& AllocationInfo = BuildContext.AllocationInfo;
	for (TFieldIterator<FProperty> It(CameraNodeClass); It; ++It)
	{
		FStructProperty* StructProperty = CastField<FStructProperty>(*It);
		if (!StructProperty)
		{
			continue;
		}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		if (StructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
		{\
			auto* CameraParameterPtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraParameter>(CameraNode);\
			AddVariableToAllocationInfo(CameraParameterPtr->Variable, AllocationInfo.VariableTableInfo);\
		}\
		else if (StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
		{\
			auto* CameraVariableReferencePtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(CameraNode);\
			AddVariableToAllocationInfo(CameraVariableReferencePtr->Variable, AllocationInfo.VariableTableInfo);\
		}\
		else
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		{
			// Another struct property...
		}
	}

	// Now do the same with custom parameters handled by the node itself.
	if (ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNode))
	{
		FCustomCameraNodeParameterInfos CustomParameters;
		CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);

		for (const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& BlendableParameter : CustomParameters.BlendableParameters)
		{
			AddVariableToAllocationInfo(BlendableParameter.OverrideVariable, AllocationInfo.VariableTableInfo);
		}
	}

	// Let the camera node add any custom variables or extra memory.
	CameraNode->Build(BuildContext);
}

void FCameraRigAssetBuilder::BuildParametersAllocationInfo(FCameraRigBuildContext& BuildContext)
{
	// The variables and context data definitions should have already been added by the camera nodes
	// who have override variable IDs and data IDs set on them. 

	for (const UCameraRigBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
	{
		if (!BlendableParameter->PrivateVariableID.IsValid())
		{
			continue;
		}

		FCameraVariableDefinition Definition = BlendableParameter->GetVariableDefinition();
		BuildContext.AllocationInfo.VariableTableInfo.VariableDefinitions.Add(Definition);
	}

	for (const UCameraRigDataParameter* DataParameter : CameraRig->Interface.DataParameters)
	{
		if (!DataParameter->PrivateDataID.IsValid())
		{
			continue;
		}

		FCameraContextDataDefinition Definition = DataParameter->GetDataDefinition();
		BuildContext.AllocationInfo.ContextDataTableInfo.DataDefinitions.Add(Definition);
	}
}

void FCameraRigAssetBuilder::BuildParameterDefinitions()
{
	TArray<FCameraRigParameterDefinition> ParameterDefinitions;

	for (const UCameraRigBlendableParameter* BlendableParameter : CameraRig->Interface.BlendableParameters)
	{
		if (BlendableParameter && BlendableParameter->PrivateVariableID)
		{
			FCameraRigParameterDefinition Definition;
			Definition.ParameterName = FName(BlendableParameter->InterfaceParameterName);
			Definition.ParameterGuid = BlendableParameter->GetGuid();
			Definition.ParameterType = ECameraRigInterfaceParameterType::Blendable;
			Definition.VariableID = BlendableParameter->PrivateVariableID;
			Definition.VariableType = BlendableParameter->ParameterType;
			Definition.BlendableStructType = BlendableParameter->BlendableStructType;
			ParameterDefinitions.Add(Definition);
		}
	}

	for (const UCameraRigDataParameter* DataParameter : CameraRig->Interface.DataParameters)
	{
		if (DataParameter && DataParameter->PrivateDataID)
		{
			FCameraRigParameterDefinition Definition;
			Definition.ParameterName = FName(DataParameter->InterfaceParameterName);
			Definition.ParameterGuid = DataParameter->GetGuid();
			Definition.ParameterType = ECameraRigInterfaceParameterType::Data;
			Definition.DataID = DataParameter->PrivateDataID;
			Definition.DataType = DataParameter->DataType;
			Definition.DataTypeObject = DataParameter->DataTypeObject;
			ParameterDefinitions.Add(Definition);
		}
	}

	if (ParameterDefinitions != CameraRig->ParameterDefinitions)
	{
		CameraRig->Modify();
		CameraRig->ParameterDefinitions = ParameterDefinitions;
	}
}

void FCameraRigAssetBuilder::BuildDefaultParameters()
{
	FInstancedPropertyBag DefaultParameters;
	FCameraRigParameterBuilder::BuildDefaultParameters(CameraRig, DefaultParameters);
	if (!DefaultParameters.Identical(&CameraRig->DefaultParameters, 0))
	{
		CameraRig->Modify();
		CameraRig->DefaultParameters = DefaultParameters;
	}
}

void FCameraRigAssetBuilder::UpdateBuildStatus()
{
	ECameraBuildStatus BuildStatus = ECameraBuildStatus::Clean;
	if (BuildLog.HasErrors())
	{
		BuildStatus = ECameraBuildStatus::WithErrors;
	}
	else if (BuildLog.HasWarnings())
	{
		BuildStatus = ECameraBuildStatus::CleanWithWarnings;
	}

	// Don't modify the camera rig: BuildStatus is transient.
	CameraRig->BuildStatus = BuildStatus;
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

