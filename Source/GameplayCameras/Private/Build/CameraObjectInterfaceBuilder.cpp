// Copyright Epic Games, Inc. All Rights Reserved.

#include "Build/CameraObjectInterfaceBuilder.h"

#include "Core/BaseCameraObject.h"
#include "Core/CameraNode.h"
#include "Core/CameraNodeHierarchy.h"
#include "Core/CameraObjectInterface.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableReferences.h"
#include "Core/ICustomCameraNodeParameterProvider.h"
#include "GameplayCameras.h"

#define LOCTEXT_NAMESPACE "CameraObjectInterfaceBuilder"

namespace UE::Cameras
{

namespace Internal
{

struct FInterfaceParameterBindingBuilder
{
	UBaseCameraObject* CameraObject;

	FInterfaceParameterBindingBuilder(FCameraObjectInterfaceBuilder& InOwner)
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
		Owner.BuildLog.AddMessage(EMessageSeverity::Error, MoveTemp(ErrorMessage));
	}

	template<typename CameraParameterOrVariableReferenceType>
	void SetCameraParameterOrVariableReferenceOverride(
			const UCameraObjectInterfaceBlendableParameter* BlendableParameter,
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

		UObject* TargetNode = BlendableParameter->Target;
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(TargetProperty->GetFName(), TargetNode);
		if (PreviousVariableID != BlendableParameter->PrivateVariableID)
		{
			TargetNode->Modify();
		}

		CameraParameterOrVariableReferencePtr->VariableID = BlendableParameter->PrivateVariableID;
	}

	template<typename VariableAssetType>
	void SetCustomBlendableParameterOverride(
			const UCameraObjectInterfaceBlendableParameter* BlendableParameter,
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

		UObject* TargetNode = BlendableParameter->Target;
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(CustomParameter.ParameterName, TargetNode);
		if (PreviousVariableID != BlendableParameter->PrivateVariableID)
		{
			TargetNode->Modify();
		}

		(*CustomParameter.OverrideVariableID) = BlendableParameter->PrivateVariableID;
	}

	void SetCustomBlendableStructParameterOverride(
			const UCameraObjectInterfaceBlendableParameter* BlendableParameter,
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

		UObject* TargetNode = BlendableParameter->Target;
		FCameraVariableID PreviousVariableID = FindOldDrivingVariableID(CustomParameter.ParameterName, TargetNode);
		if (PreviousVariableID != BlendableParameter->PrivateVariableID)
		{
			TargetNode->Modify();
		}

		(*CustomParameter.OverrideVariableID) = BlendableParameter->PrivateVariableID;
	}

	void SetDataContextPropertyOverride(
			const UCameraObjectInterfaceDataParameter* DataParameter,
			FProperty* TargetProperty,
			FCameraContextDataID* OverrideDataID)
	{
		ensure(DataParameter->PrivateDataID);

		const bool bIsValid = CheckIfParameterCanBeOverridden(DataParameter, OverrideDataID);
		if (!bIsValid)
		{
			return;
		}

		UObject* TargetNode = DataParameter->Target;
		FCameraContextDataID PreviousDataID = FindOldDrivingDataID(TargetProperty->GetFName(), TargetNode);
		if (PreviousDataID != DataParameter->PrivateDataID)
		{
			TargetNode->Modify();
		}

		(*OverrideDataID) = DataParameter->PrivateDataID;
	}

	void SetCustomDataParameterOverride(
			const UCameraObjectInterfaceDataParameter* DataParameter,
			const FCustomCameraNodeParameterInfos::FDataParameterInfo& CustomParameter)
	{
		ensure(DataParameter->PrivateDataID);
		ensure(DataParameter->TargetPropertyName == CustomParameter.ParameterName);

		const bool bIsValid = CheckIfParameterCanBeOverridden(DataParameter, CustomParameter.OverrideDataID);
		if (!bIsValid)
		{
			return;
		}

		UObject* TargetNode = DataParameter->Target;
		FCameraContextDataID PreviousDataID = FindOldDrivingDataID(CustomParameter.ParameterName, TargetNode);
		if (PreviousDataID != DataParameter->PrivateDataID)
		{
			TargetNode->Modify();
		}

		(*CustomParameter.OverrideDataID) = DataParameter->PrivateDataID;
	}

private:

	FCameraVariableID FindOldDrivingVariableID(FName ForParameterName, UObject* ForObject)
	{
		using FDrivenParameterKey = FCameraObjectInterfaceBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterName, ForObject };
		FCameraVariableID ReusedVariableID;
		Owner.OldDrivenBlendableParameters.RemoveAndCopyValue(ParameterKey, ReusedVariableID);
		return ReusedVariableID;
	}

	FCameraContextDataID FindOldDrivingDataID(FName ForParameterName, UObject* ForObject)
	{
		using FDrivenParameterKey = FCameraObjectInterfaceBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterName, ForObject };
		FCameraContextDataID ReusedDataID;
		Owner.OldDrivenDataParameters.RemoveAndCopyValue(ParameterKey, ReusedDataID);
		return ReusedDataID;
	}

	template<typename CameraParameterOrVariableReferenceType>
	bool CheckIfParameterCanBeOverridden(
			const UCameraObjectInterfaceBlendableParameter* BlendableParameter, 
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
			const UCameraObjectInterfaceBlendableParameter* BlendableParameter, 
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
			const UCameraObjectInterfaceBlendableParameter* BlendableParameter, 
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
							"Camera node parameter '{0}.{1}' is both exposed and driven by a variable!"),
						FText::FromName(BlendableParameter->Target->GetFName()), 
						FText::FromName(BlendableParameter->TargetPropertyName)));
			return false;
		}
		return true;
	}

	bool CheckIfParameterCanBeOverridden(
			const UCameraObjectInterfaceDataParameter* DataParameter,
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

	FCameraObjectInterfaceBuilder& Owner;
};

}  // namespace Internal

FCameraObjectInterfaceBuilder::FCameraObjectInterfaceBuilder(FCameraBuildLog& InBuildLog)
	: BuildLog(InBuildLog)
{
}

void FCameraObjectInterfaceBuilder::BuildInterface(UBaseCameraObject* InCameraObject, const FCameraNodeHierarchy& InHierarchy, bool bCollectStrayNodes)
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
			UE_LOG(LogCameraSystem, Verbose, TEXT("Collected %d stray camera nodes while building camera rig '%s'."),
					NumStrayCameraNodes, *GetPathNameSafe(CameraObject));
		}
	}

	BuildInterface(InCameraObject, CameraNodesToGather.Array());
}

void FCameraObjectInterfaceBuilder::BuildInterface(UBaseCameraObject* InCameraObject, TArrayView<UCameraNode*> InCameraObjectNodes)
{
	if (!ensure(InCameraObject))
	{
		return;
	}

	CameraObject = InCameraObject;
	CameraObjectNodes = InCameraObjectNodes;
	{
		BuildInterfaceImpl();
	}
	CameraObject = nullptr;
	CameraObjectNodes.Reset();
}

void FCameraObjectInterfaceBuilder::BuildInterfaceImpl()
{
	GatherOldDrivenParameters();
	BuildInterfaceParameters();
	BuildInterfaceParameterBindings();
	DiscardUnusedParameters();
}

void FCameraObjectInterfaceBuilder::GatherOldDrivenParameters()
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

	for (UCameraNode* CameraNode : CameraObjectNodes)
	{
		UClass* CameraNodeClass = CameraNode->GetClass();
		
		for (TFieldIterator<FProperty> It(CameraNodeClass); It; ++It)
		{
			FProperty* Property(*It);

			// First look for some blendable camera parameters.
			if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				bool bFoundCameraParameter = true;
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
					bFoundCameraParameter = false;
				}

				if (bFoundCameraParameter)
				{
					continue;
				}
			}

			// Then look for some data parameters.
			const FName DataIDPropertyName = FName(It->GetName() + TEXT("DataID"));
			FStructProperty* DataIDStructProperty = CastField<FStructProperty>(CameraNodeClass->FindPropertyByName(DataIDPropertyName));
			if (DataIDStructProperty && DataIDStructProperty->Struct == FCameraContextDataID::StaticStruct())
			{
				FCameraContextDataID* ExistingDataID = DataIDStructProperty->ContainerPtrToValuePtr<FCameraContextDataID>(CameraNode);
				if (ExistingDataID->IsValid())
				{
					OldDrivenDataParameters.Add(FDrivenParameterKey{ Property->GetFName(), CameraNode }, *ExistingDataID);
					*ExistingDataID = FCameraContextDataID();
				}
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

void FCameraObjectInterfaceBuilder::BuildInterfaceParameters()
{
	// Here we simply validate all blendable/data interface parameters and create IDs for their entries in the
	// variable and context data tables.

	using namespace Internal;

	for (auto It = CameraObject->Interface.BlendableParameters.CreateIterator(); It; ++It)
	{
		UCameraObjectInterfaceBlendableParameter* BlendableParameter(*It);

		// Basic validations.
		if (!BlendableParameter)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					CameraObject,
					LOCTEXT("InvalidBlendableParameter", "Invalid interface parameter was found and removed."));

			CameraObject->Modify();
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

	for (auto It = CameraObject->Interface.DataParameters.CreateIterator(); It; ++It)
	{
		UCameraObjectInterfaceDataParameter* DataParameter(*It);

		// Basic validations.
		if (!DataParameter)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					CameraObject,
					LOCTEXT("InvalidDataParameter", "Invalid interface parameter was found and removed."));

			CameraObject->Modify();
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

void FCameraObjectInterfaceBuilder::BuildInterfaceParameterBindings()
{
	// Now we connect the interface paramters to whatever node property they are supposed to drive.
	// Each time we need to check for either a custom property (via ICustomCameraNodeParameterProvider),
	// or a UObject property found with reflection.

	using FBuiltDrivenParameter = TTuple<UObject*, FName>;
	TSet<FBuiltDrivenParameter> BuiltDrivenParameters;

	const FString CameraObjectName = CameraObject->GetName();
	const FString CameraObjectPathName = CameraObject->GetPathName();

	for (const UCameraObjectInterfaceBlendableParameter* BlendableParameter : CameraObject->Interface.BlendableParameters)
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
			UObject* Target = BlendableParameter->Target;
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

	for (const UCameraObjectInterfaceDataParameter* DataParameter : CameraObject->Interface.DataParameters)
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
			UObject* Target = DataParameter->Target;
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

bool FCameraObjectInterfaceBuilder::SetupCameraParameterOrVariableReferenceOverride(const UCameraObjectInterfaceBlendableParameter* BlendableParameter)
{
	using namespace Internal;

	// Here we hook up interface parameters connected to a camera node property. This property is supposed
	// to be of one of the camera parameter types (FBooleanCameraParameter, FInteger32CameraParameter, etc.)
	// so they have both a fixed value (bool, int32, etc.) and a "private variable" which we will set to the 
	// private variable of the given interface parameter, checking that the types match (UBooleanCameraVariable, 
	// UInteger32CameraVariable, etc.)

	UObject* Target = BlendableParameter->Target;
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

bool FCameraObjectInterfaceBuilder::SetupCustomBlendableParameterOverride(const UCameraObjectInterfaceBlendableParameter* BlendableParameter)
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

bool FCameraObjectInterfaceBuilder::SetupDataContextPropertyOverride(const UCameraObjectInterfaceDataParameter* DataParameter)
{
	using namespace Internal;

	UObject* Target = DataParameter->Target;
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

bool FCameraObjectInterfaceBuilder::SetupCustomDataParameterOverride(const UCameraObjectInterfaceDataParameter* DataParameter)
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

void FCameraObjectInterfaceBuilder::DiscardUnusedParameters()
{
	// Now that we've rebuilt all exposed parameters, anything left from the old list 
	// must be discarded. These are nodes and properties that used to be driven by
	// variables and now aren't, so we need to flag them as modified.

	for (TPair<FDrivenParameterKey, FCameraVariableID> Pair : OldDrivenBlendableParameters)
	{
		UObject* Target = Pair.Key.Value;
		Target->Modify();
	}
	OldDrivenBlendableParameters.Reset();

	for (TPair<FDrivenParameterKey, FCameraContextDataID> Pair : OldDrivenDataParameters)
	{
		UObject* Target = Pair.Key.Value;
		Target->Modify();
	}
	OldDrivenDataParameters.Reset();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

