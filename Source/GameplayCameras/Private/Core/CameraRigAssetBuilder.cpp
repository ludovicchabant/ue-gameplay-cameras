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

template<typename VariableAssetType, typename ValueType>
void SetPrivateVariableDefaultValue(VariableAssetType* PrivateVariable, typename TCallTraits<ValueType>::ParamType Value)
{
	if (PrivateVariable->DefaultValue != Value)
	{
		PrivateVariable->Modify();
		PrivateVariable->DefaultValue = Value;
	}
}

template<>
void SetPrivateVariableDefaultValue<UTransform3dCameraVariable, FTransform3d>(UTransform3dCameraVariable* PrivateVariable, const FTransform3d& Value)
{
	// Template overload because transforms don't have an operator!=.
	if (!PrivateVariable->DefaultValue.Equals(Value, 0.f))
	{
		PrivateVariable->Modify();
		PrivateVariable->DefaultValue = Value;
	}
}

template<>
void SetPrivateVariableDefaultValue<UTransform3fCameraVariable, FTransform3f>(UTransform3fCameraVariable* PrivateVariable, const FTransform3f& Value)
{
	// Template overload because transforms don't have an operator!=.
	if (!PrivateVariable->DefaultValue.Equals(Value, 0.f))
	{
		PrivateVariable->Modify();
		PrivateVariable->DefaultValue = Value;
	}
}

template<>
void SetPrivateVariableDefaultValue<UBooleanCameraVariable, bool>(UBooleanCameraVariable* PrivateVariable, bool bValue)
{
	// Template overload because boolean variables have a bDefaultValue field, not DefaultValue.
	if (PrivateVariable->bDefaultValue != bValue)
	{
		PrivateVariable->Modify();
		PrivateVariable->bDefaultValue = bValue;
	}
}

UCameraVariableAsset* CreatePrivateVariable(
		UCameraRigAsset* CameraRig,
		const FString& InterfaceParameterName,
		ECameraVariableType ParameterType)
{
	const FString VariableName = FString::Format(
			TEXT("Override_{0}_{1}"), 
			{ CameraRig->GetName(), InterfaceParameterName });

	TSubclassOf<UCameraVariableAsset> VariableClass;
	switch (ParameterType)
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(VariableType, VariableName)\
		case ECameraVariableType::VariableName:\
			VariableClass = U##VariableName##CameraVariable::StaticClass();\
			break;
		UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}
	if (!ensure(VariableClass))
	{
		return nullptr;
	}

	UCameraVariableAsset* PrivateVariable = NewObject<UCameraVariableAsset>(
			CameraRig, VariableClass, FName(*VariableName), RF_Transactional);

	// Make sure it's a private input variable.
	PrivateVariable->bIsInput = true;
	PrivateVariable->bIsPrivate = true;
	PrivateVariable->bAutoReset = false;

	return PrivateVariable;
}

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

	template<typename CameraParameterType>
	void SetCameraParameterOverride(
			const UCameraRigBlendableParameter* BlendableParameter,
			FStructProperty* TargetProperty,
			CameraParameterType* CameraParameterPtr)
	{
		using VariableAssetType = typename CameraParameterType::VariableAssetType;
		using ValueType = typename VariableAssetType::ValueType;

		ensure(BlendableParameter->PrivateVariable);
		ensure(BlendableParameter->TargetPropertyName == TargetProperty->GetFName());

		VariableAssetType* NewVariable = EnsureCompatibleVariable<VariableAssetType>(BlendableParameter);
		const bool bIsValid = CheckTwiceDrivenParameter(BlendableParameter, CameraParameterPtr);
		if (!NewVariable || !bIsValid)
		{
			return;
		}

		// Find the variable that was previously driving this parameter. If it has changed, flag the
		// camera node as having been modified.
		UCameraNode* TargetNode = BlendableParameter->Target;
		VariableAssetType* PreviousVariable = FindOldDrivingVariable<VariableAssetType>(TargetProperty->GetFName(), TargetNode);
		if (PreviousVariable != NewVariable)
		{
			TargetNode->Modify();
		}

		SetPrivateVariableDefaultValue<VariableAssetType, ValueType>(NewVariable, CameraParameterPtr->Value);

		CameraParameterPtr->Variable = NewVariable;
	}

	template<typename VariableReferenceType>
	void SetVariableReferenceOverride(
			const UCameraRigBlendableParameter* BlendableParameter,
			FStructProperty* TargetProperty,
			VariableReferenceType* VariableReferencePtr)
	{
		using VariableAssetType = typename VariableReferenceType::VariableAssetType;
		using ValueType = typename VariableAssetType::ValueType;

		ensure(BlendableParameter->PrivateVariable);
		ensure(BlendableParameter->TargetPropertyName == TargetProperty->GetFName());

		VariableAssetType* NewVariable = EnsureCompatibleVariable<VariableAssetType>(BlendableParameter);
		const bool bIsValid = CheckTwiceDrivenParameter(BlendableParameter, VariableReferencePtr);
		if (!NewVariable || !bIsValid)
		{
			return;
		}

		// Find the variable that was previously driving this reference. If it has changed, flag the
		// camera node as having been modified.
		UCameraNode* TargetNode = BlendableParameter->Target;
		VariableAssetType* PreviousVariable = FindOldDrivingVariable<VariableAssetType>(TargetProperty->GetFName(), TargetNode);
		if (PreviousVariable != NewVariable)
		{
			TargetNode->Modify();
		}

		// No default value to set on the driving variable.

		VariableReferencePtr->Variable = NewVariable;
	}

	template<typename VariableAssetType>
	void SetCustomBlendableParameterOverride(
			const UCameraRigBlendableParameter* BlendableParameter,
			const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& CustomParameter)
	{
		using ValueType = typename VariableAssetType::ValueType;

		ensure(BlendableParameter->PrivateVariable);
		ensure(BlendableParameter->TargetPropertyName == CustomParameter.ParameterName);

		VariableAssetType* NewVariable = EnsureCompatibleVariable<VariableAssetType>(BlendableParameter);
		const bool bIsValid = CheckTwiceDrivenParameter(BlendableParameter, *CustomParameter.OverrideVariable);
		if (!NewVariable || !bIsValid)
		{
			return;
		}

		// Find the variable that was previously driving this override. If it has changed, flag the
		// camera node as having been modified.
		UCameraNode* TargetNode = BlendableParameter->Target;
		VariableAssetType* PreviousVariable = FindOldDrivingVariable<VariableAssetType>(CustomParameter.ParameterName, TargetNode);
		if (PreviousVariable != NewVariable)
		{
			TargetNode->Modify();
		}

		const ValueType* DefaultValue = reinterpret_cast<const ValueType*>(CustomParameter.DefaultValuePtr);
		SetPrivateVariableDefaultValue<VariableAssetType, ValueType>(NewVariable, *DefaultValue);

		(*CustomParameter.OverrideVariable) = NewVariable;
	}

	void SetDataContextPropertyOverride(
			const UCameraRigDataParameter* DataParameter,
			FProperty* TargetProperty,
			FCameraContextDataID* OverrideDataID)
	{
		ensure(DataParameter->PrivateDataID);

		const bool bIsValid = CheckTwiceDrivenParameter(DataParameter, *OverrideDataID);
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

		const bool bIsValid = CheckTwiceDrivenParameter(DataParameter, *CustomParameter.OverrideDataID);
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

	template<typename ExpectedVariableAssetType>
	ExpectedVariableAssetType* FindOldDrivingVariable(FName ForParameterName, UCameraNode* ForCameraNode)
	{
		using FDrivenParameterKey = FCameraRigAssetBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterName, ForCameraNode };
		TObjectPtr<UCameraVariableAsset> ReusedVariable;
		Owner.OldDrivenBlendableParameters.RemoveAndCopyValue(ParameterKey, ReusedVariable);
		return CastChecked<ExpectedVariableAssetType>(ReusedVariable, ECastCheckedType::NullAllowed);
	}

	FCameraContextDataID FindOldDrivingDataID(FName ForParameterName, UCameraNode* ForCameraNode)
	{
		using FDrivenParameterKey = FCameraRigAssetBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterName, ForCameraNode };
		FCameraContextDataID ReusedDataID;
		Owner.OldDrivenDataParameters.RemoveAndCopyValue(ParameterKey, ReusedDataID);
		return ReusedDataID;
	}

	template<typename CameraParameterType>
	bool CheckTwiceDrivenParameter(
			const UCameraRigBlendableParameter* BlendableParameter, 
			CameraParameterType* CameraParameter)
	{
		using VariableAssetType = typename CameraParameterType::VariableAssetType;
		return CheckTwiceDrivenParameter<VariableAssetType>(BlendableParameter, CameraParameter->Variable);
	}

	template<typename VariableAssetType>
	bool CheckTwiceDrivenParameter(
			const UCameraRigBlendableParameter* BlendableParameter, 
			TObjectPtr<VariableAssetType> CameraParameterVariable)
	{
		if (CameraParameterVariable != nullptr)
		{
			// We should have cleared all exposed parameters in GatherOldDrivenParameters, so the only variables
			// left on camera parameters should be user-defined ones.
			UObject* VariableOuter = CameraParameterVariable->GetOuter();
			if (ensureMsgf(
						VariableOuter != CameraRig, 
						TEXT("Unexpected driving variable found: all exposed parameters should have been cleared before rebuilding.")))
			{
				// If this parameter is driven by a user-defined variable, emit an error.
				ReportError(BlendableParameter->Target,
						FText::Format(
							LOCTEXT(
								"BlendableParameterDrivenTwice", 
								"Camera node parameter '{0}.{1}' is both exposed and driven by a variable!"),
							FText::FromName(BlendableParameter->Target->GetFName()), 
							FText::FromName(BlendableParameter->TargetPropertyName)));
				return false;
			}
		}
		return true;
	}

	bool CheckTwiceDrivenParameter(
			const UCameraRigDataParameter* DataParameter,
			FCameraContextDataID DataID)
	{
		if (DataID.IsValid())
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

	template<typename VariableAssetType>
	VariableAssetType* EnsureCompatibleVariable(const UCameraRigBlendableParameter* BlendableParameter)
	{
		if (!ensure(BlendableParameter->PrivateVariable))
		{
			return nullptr;
		}

		VariableAssetType* Variable = Cast<VariableAssetType>(BlendableParameter->PrivateVariable);

		if (!Variable)
		{
			const UClass* VariableAssetClass = VariableAssetType::StaticClass();
			const UCameraVariableAsset* DefaultObject = VariableAssetClass->template GetDefaultObject<UCameraVariableAsset>();

			ReportError(BlendableParameter->Target,
				FText::Format(LOCTEXT(
					"IncompatibleCameraNodeParameter",
					"Invalid interface parameter '{0}', driving property '{1}' on '{2}': expected {3} camera parameter but was {4}"),
					FText::FromString(BlendableParameter->InterfaceParameterName),
					FText::FromName(BlendableParameter->TargetPropertyName),
					FText::FromName(BlendableParameter->Target->GetFName()),
					FText::FromName(UEnum::GetValueAsName(DefaultObject->GetVariableType())),
					FText::FromName(UEnum::GetValueAsName(BlendableParameter->ParameterType))
					)
				);
			return nullptr;
		}

		return Variable;
	}

private:

	FCameraRigAssetBuilder& Owner;
};

void TrashPrivateVariable(UCameraVariableAsset* VariableToTrash)
{
	TStringBuilder<256> StringBuilder;
	StringBuilder.Append("TRASH_");
	StringBuilder.Append(VariableToTrash->GetName());

	VariableToTrash->Modify();
	VariableToTrash->Rename(StringBuilder.ToString());
}

void AddCameraVariableToAllocationInfo(UCameraVariableAsset* Variable, FCameraVariableTableAllocationInfo& AllocationInfo)
{
	if (Variable)
	{
		FCameraVariableDefinition VariableDefinition = Variable->GetVariableDefinition();
		AllocationInfo.VariableDefinitions.Add(VariableDefinition);
		if (Variable->bAutoReset)
		{
			AllocationInfo.AutoResetVariables.Add(Variable);
		}
	}
}

void AddContextDataToAllocationInfo(FCameraContextDataID DataID, ECameraContextDataType DataType, const UObject* DataTypeObject, FCameraContextDataAllocationInfo& AllocationInfo)
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
	// Keep track of which camera parameters and variable references were previously driven by 
	// private variables, and then clear those variables. This is because it's easier to rebuild 
	// all this from a blank slate than trying to figure out what changed.
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
				if (CameraParameterPtr->Variable)\
				{\
					UObject* VariableOuter = CameraParameterPtr->Variable->GetOuter();\
					if (VariableOuter == CameraRig)\
					{\
						OldDrivenBlendableParameters.Add(\
								FDrivenParameterKey{ StructProperty->GetFName(), CameraNode },\
								*reinterpret_cast<TObjectPtr<UCameraVariableAsset>*>(\
									reinterpret_cast<FObjectPtr*>(&CameraParameterPtr->Variable)));\
						CameraParameterPtr->Variable = nullptr;\
					}\
				}\
			}\
			else if (StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
			{\
				auto* VariableReferencePtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(CameraNode);\
				if (VariableReferencePtr->Variable)\
				{\
					UObject* VariableOuter = VariableReferencePtr->Variable->GetOuter();\
					if (VariableOuter == CameraRig)\
					{\
						OldDrivenBlendableParameters.Add(\
								FDrivenParameterKey{ StructProperty->GetFName(), CameraNode },\
								*reinterpret_cast<TObjectPtr<UCameraVariableAsset>*>(\
									reinterpret_cast<FObjectPtr*>(&VariableReferencePtr->Variable)));\
						VariableReferencePtr->Variable = nullptr;\
					}\
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
				if (!ensure(BlendableParameter.OverrideVariable))
				{
					continue;
				}

				if (UCameraVariableAsset* Variable = BlendableParameter.OverrideVariable->Get())
				{
					UObject* VariableOuter = Variable->GetOuter();
					if (VariableOuter == CameraRig)
					{
						OldDrivenBlendableParameters.Add(
								FDrivenParameterKey{ BlendableParameter.ParameterName, CameraNode },
								*BlendableParameter.OverrideVariable);
						(*BlendableParameter.OverrideVariable) = nullptr;
					}
				}
			}

			for (const FCustomCameraNodeParameterInfos::FDataParameterInfo& DataParameter : CustomParameters.DataParameters)
			{
				if (!ensure(DataParameter.OverrideDataID))
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

		// Create a new private variable for this interface parameter if it doesn't have on yet (e.g. it's a
		// newly-created parameter), or if its type has changed.
		UCameraVariableAsset* PrivateVariable = BlendableParameter->PrivateVariable;
		if (PrivateVariable && PrivateVariable->GetVariableType() == BlendableParameter->ParameterType)
		{
			continue;
		}

		if (PrivateVariable)
		{
			// Trash it so its name is available for the new private variable.
			TrashPrivateVariable(PrivateVariable);

			// We supposedly have removed all references to private variables in GatherOldDrivenParameters,
			// so once we null-out the reference on the interface parameter, this private variable should
			// not be used anymore and should get GC'ed soon.
			BlendableParameter->Modify();
			BlendableParameter->PrivateVariable = nullptr;
		}

		BlendableParameter->PrivateVariable = CreatePrivateVariable(
				CameraRig, BlendableParameter->InterfaceParameterName, BlendableParameter->ParameterType);
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

		// Create the data ID for this parameter. Flag the parameter as changed if the ID
		// is different, such as if it was renamed.
		const FString DataIDName = FString::Format(
				TEXT("Override_{0}_{1}"), 
				{ CameraRig->GetName(), DataParameter->InterfaceParameterName });

		FCameraContextDataID DataID = FCameraContextDataID::FromName(FName(DataIDName));
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
		if (SetupCameraParameterOrVariableReferenceOverride(BlendableParameter))
		{
			// Implicit continue.
		}
		else if (SetupCustomBlendableParameterOverride(BlendableParameter))
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

		if (SetupDataContextPropertyOverride(DataParameter))
		{
			// Implicit continue.
		}
		else if (SetupCustomDataParameterOverride(DataParameter))
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
		Builder.SetCameraParameterOverride<F##ValueName##CameraParameter>(\
				BlendableParameter, TargetStructProperty, CameraParameterPtr\
				);\
	}\
	else if (TargetStructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
	{\
		auto* VariableReferencePtr = TargetStructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(Target);\
		Builder.SetVariableReferenceOverride<F##ValueName##CameraVariableReference>(\
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

	for (TPair<FDrivenParameterKey, TObjectPtr<UCameraVariableAsset>> Pair : OldDrivenBlendableParameters)
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
	// This is for both exposed rig parameters (which we just built in BuildNewDrivenParameters) and 
	// for parameters driven by user-defined variables.
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
			AddCameraVariableToAllocationInfo(CameraParameterPtr->Variable, AllocationInfo.VariableTableInfo);\
		}\
		else if (StructProperty->Struct == F##ValueName##CameraVariableReference::StaticStruct())\
		{\
			auto* CameraVariableReferencePtr = StructProperty->ContainerPtrToValuePtr<F##ValueName##CameraVariableReference>(CameraNode);\
			AddCameraVariableToAllocationInfo(CameraVariableReferencePtr->Variable, AllocationInfo.VariableTableInfo);\
		}\
		else
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		{
			// Another struct property...
		}
	}

	// Now do the same with custom parameters handled by the node itself. These custom parameters have 
	// been hooked up to overrides in BuildInterfaceParameterBindings so we should be getting them back 
	// with the overrides set here.
	if (ICustomCameraNodeParameterProvider* CustomParameterProvider = Cast<ICustomCameraNodeParameterProvider>(CameraNode))
	{
		FCustomCameraNodeParameterInfos CustomParameters;
		CustomParameterProvider->GetCustomCameraNodeParameters(CustomParameters);

		for (const FCustomCameraNodeParameterInfos::FBlendableParameterInfo& BlendableParameter : CustomParameters.BlendableParameters)
		{
			if (ensure(BlendableParameter.OverrideVariable))
			{
				AddCameraVariableToAllocationInfo(BlendableParameter.OverrideVariable->Get(), AllocationInfo.VariableTableInfo);
			}
		}

		for (const FCustomCameraNodeParameterInfos::FDataParameterInfo& DataParameter : CustomParameters.DataParameters)
		{
			if (ensure(DataParameter.OverrideDataID))
			{
				AddContextDataToAllocationInfo(*DataParameter.OverrideDataID, DataParameter.ParameterType, DataParameter.ParameterTypeObject, AllocationInfo.ContextDataTableInfo);
			}
		}
	}

	// Let the camera node add any custom variables or extra memory.
	CameraNode->Build(BuildContext);
}

void FCameraRigAssetBuilder::BuildDefaultParameters()
{
	FInstancedPropertyBag DefaultParameters;
	BuildDefaultParameters(CameraRig, DefaultParameters);
	if (DefaultParameters.GetPropertyBagStruct() != CameraRig->DefaultParameters.GetPropertyBagStruct())
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

void FCameraRigAssetBuilder::BuildDefaultParameters(UCameraRigAsset* CameraRigAsset, FInstancedPropertyBag& OutPropertyBag)
{
	TArray<FPropertyBagPropertyDesc> DefaultParameterProperties;
	AppendDefaultParameters(CameraRigAsset->Interface, DefaultParameterProperties);
	OutPropertyBag.AddProperties(DefaultParameterProperties);
}

void FCameraRigAssetBuilder::AppendDefaultParameters(const FCameraRigInterface& CameraRigInterface, TArray<FPropertyBagPropertyDesc>& OutProperties)
{
	for (const UCameraRigBlendableParameter* BlendableParameter : CameraRigInterface.BlendableParameters)
	{
		FName PropertyName(BlendableParameter->InterfaceParameterName);

		EPropertyBagPropertyType PropertyType = EPropertyBagPropertyType::Struct;
		const UObject* PropertyTypeObject = nullptr;

		switch (BlendableParameter->ParameterType)
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			case ECameraVariableType::ValueName:\
				PropertyTypeObject = F##ValueName##CameraParameter::StaticStruct();\
				break;
		UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
			default:
				ensure(false);
				break;
		}

		FPropertyBagPropertyDesc NewProperty(PropertyName, PropertyType, PropertyTypeObject);
		// Make the property bag match the camera interface parameter GUIDs.
		NewProperty.ID = BlendableParameter->GetGuid();

		OutProperties.Add(NewProperty);
	}

	for (const UCameraRigDataParameter* DataParameter : CameraRigInterface.DataParameters)
	{
		FName PropertyName(DataParameter->InterfaceParameterName);

		bool bIsValid = true;
		EPropertyBagPropertyType PropertyType = EPropertyBagPropertyType::Bool;
		const UObject* PropertyTypeObject = DataParameter->DataTypeObject;

		switch (DataParameter->DataType)
		{
			case ECameraContextDataType::Name:
				PropertyType = EPropertyBagPropertyType::Name;
				break;
			case ECameraContextDataType::String:
				PropertyType = EPropertyBagPropertyType::String;
				break;
			case ECameraContextDataType::Enum:
				PropertyType = EPropertyBagPropertyType::Enum;
				ensure(PropertyTypeObject && PropertyTypeObject->IsA<UEnum>());
				break;
			case ECameraContextDataType::Struct:
				PropertyType = EPropertyBagPropertyType::Struct;
				ensure(PropertyTypeObject && PropertyTypeObject->IsA<UScriptStruct>());
				break;
			case ECameraContextDataType::Object:
				PropertyType = EPropertyBagPropertyType::Object;
				break;
			case ECameraContextDataType::Class:
				PropertyType = EPropertyBagPropertyType::Class;
				break;
			default:
				ensure(false);
				bIsValid = false;
				break;
		}

		if (bIsValid)
		{
			FPropertyBagPropertyDesc NewProperty(PropertyName, PropertyType, PropertyTypeObject);
			// Make the property bag match the camera interface parameter GUIDs.
			NewProperty.ID = DataParameter->GetGuid();

			OutProperties.Add(NewProperty);
		}
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

