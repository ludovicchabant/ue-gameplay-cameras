// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAssetBuilder.h"

#include "Core/CameraNode.h"
#include "Core/CameraNodeEvaluatorStorage.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigBuildContext.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableReferences.h"
#include "GameplayCamerasDelegates.h"
#include "Logging/TokenizedMessage.h"
#include "Nodes/Common/CameraRigCameraNode.h"

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

struct FPrivateVariableBuilder
{
	UCameraRigAsset* CameraRig;

	FPrivateVariableBuilder(FCameraRigAssetBuilder& InOwner)
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

	template<typename ExpectedVariableAssetType>
	ExpectedVariableAssetType* FindReusablePrivateVariable(FStructProperty* ForParameterProperty, UCameraNode* ForCameraNode)
	{
		using FDrivenParameterKey = FCameraRigAssetBuilder::FDrivenParameterKey;

		FDrivenParameterKey ParameterKey{ ForParameterProperty, ForCameraNode };
		UCameraVariableAsset** FoundItem = Owner.OldDrivenParameters.Find(ParameterKey);
		if (FoundItem)
		{
			// We found an existing variable that was driving this camera node's property.
			// Re-use it and remove it from the re-use pool.
			UCameraVariableAsset* ReusedVariable = (*FoundItem);
			Owner.OldDrivenParameters.Remove(ParameterKey);
			return CastChecked<ExpectedVariableAssetType>(ReusedVariable);
		}

		return nullptr;
	}

	template<
		typename ParameterOverrideType, 
		typename ExpectedVariableAssetType = typename ParameterOverrideType::CameraParameterType::VariableAssetType>
	ExpectedVariableAssetType* FindReusablePrivateVariable(ParameterOverrideType* ForParameterOverride, UCameraRigCameraNode* ForCameraRigNode)
	{
		using FDrivenOverrideKey = FCameraRigAssetBuilder::FDrivenOverrideKey;

		FDrivenOverrideKey OverrideKey{ ForParameterOverride->InterfaceParameterGuid, ForCameraRigNode };
		UCameraVariableAsset** FoundItem = Owner.OldDrivenOverrides.Find(OverrideKey);
		if (FoundItem)
		{
			UCameraVariableAsset* ReusedVariable = (*FoundItem);
			Owner.OldDrivenOverrides.Remove(OverrideKey);
			// Don't do a checked cast here because interface parameters can change type if they are
			// reconnected to a different type of parameter. If the cast fail, we will retur null and
			// the variable won't get reused, which is what we want.
			return Cast<ExpectedVariableAssetType>(ReusedVariable);
		}

		return nullptr;
	}

	bool ReuseInterfaceParameter(UCameraRigInterfaceParameter* InterfaceParameter, UCameraVariableAsset* IntendedPrivateVariable)
	{
		using FReusableInterfaceParameterInfo = FCameraRigAssetBuilder::FReusableInterfaceParameterInfo;
		FReusableInterfaceParameterInfo* FoundItem = Owner.OldInterfaceParameters.Find(InterfaceParameter);
		if (ensure(FoundItem))
		{
			// This is an interface parameter that existed before. Flag things as modified if the 
			// private variable is changing.
			ensure(!FoundItem->Value);
			FoundItem->Value = true;  // This one has now been re-used.
			return FoundItem->Key != IntendedPrivateVariable;
		}
		// We should have had this interface parameter in our map, since we built it just a second ago!
		// Something's wrong... oh well, flag things as modified.
		return true;
	}

private:

	FCameraRigAssetBuilder& Owner;
};

template<typename CameraParameterType>
void CheckTwiceDrivenParameter(
		FPrivateVariableBuilder& Builder, 
		UCameraRigInterfaceParameter* InterfaceParameter, 
		CameraParameterType* CameraParameter)
{
	if (CameraParameter->Variable != nullptr)
	{
		// We should have cleared all exposed parameters in GatherOldDrivenParameters, so the only variables
		// left on camera parameters should be user-defined ones.
		UObject* VariableOuter = CameraParameter->Variable->GetOuter();
		if (ensureMsgf(
					VariableOuter != Builder.CameraRig, 
					TEXT("Unexpected driving variable found: all exposed parameters should have been cleared before rebuilding.")))
		{
			// If this parameter is driven by a user-defined variable, emit an error, and replace that 
			// driving variable with our private variable.
			Builder.ReportError(
					InterfaceParameter->Target,
					FText::Format(
						LOCTEXT(
							"CameraParameterDrivenTwice", 
							"Camera node parameter '{0}.{1}' is both exposed and driven by a variable!"),
						FText::FromName(InterfaceParameter->Target->GetFName()), 
						FText::FromName(InterfaceParameter->TargetPropertyName)));
		}
	}
}

template<typename VariableAssetType>
VariableAssetType* MakeOrRenamePrivateVariable(
		FPrivateVariableBuilder& Builder,
		const FString& InterfaceParameterName,
		VariableAssetType* PrivateVariable)
{
	const FString VariableName = FString::Format(
			TEXT("Override_{0}_{1}"), 
			{ Builder.CameraRig->GetName(), InterfaceParameterName });

	if (PrivateVariable)
	{
		// We have a pre-existing variable! Make sure it's got the right name, in case the exposed rig parameter
		// was renamed. Keeping a good name is mostly to help with debugging.
		FString OriginalName = PrivateVariable->GetName();
		OriginalName.RemoveFromStart("REUSABLE_", ESearchCase::CaseSensitive);
		if (OriginalName != VariableName)
		{
			PrivateVariable->Modify();
		}
		// Rename non-transactionally because we might be simply setting the variable's name back to what it
		// always was. We don't want to dirty the package for no-op builds.
		PrivateVariable->Rename(*VariableName, nullptr, REN_NonTransactional);
	}
	else
	{
		// Make a new variable.
		PrivateVariable = NewObject<VariableAssetType>(Builder.CameraRig, FName(*VariableName), RF_Transactional);
	}

	// Make sure it's a private input variable.
	PrivateVariable->bIsInput = true;
	PrivateVariable->bIsPrivate = true;
	PrivateVariable->bAutoReset = false;

	return PrivateVariable;
}

template<typename CameraParameterType, typename VariableAssetType = typename CameraParameterType::VariableAssetType>
void DoSetupPrivateVariable(
		FPrivateVariableBuilder& Builder,
		UCameraRigInterfaceParameter* InterfaceParameter,
		CameraParameterType* CameraParameter,
		VariableAssetType* ReusedVariable
		)
{
	using ValueType = typename CameraParameterType::ValueType;

	const bool bIsReusedVariable = (ReusedVariable != nullptr);

	// Either rename the camera variable we are re-using, or make a new one with the right name.
	VariableAssetType* PrivateVariable = MakeOrRenamePrivateVariable(
			Builder, InterfaceParameter->InterfaceParameterName, ReusedVariable);
	ensure(PrivateVariable->GetOuter() == Builder.CameraRig);

	// Set the default value of the variable to be the value in the camera parameter.
	SetPrivateVariableDefaultValue<VariableAssetType, ValueType>(PrivateVariable, CameraParameter->Value);

	// Set the variable on both the interface parameter and the camera node. Flag them as modified
	// if we actually changed anything.
	const bool bShouldModifyInterfaceParameter = Builder.ReuseInterfaceParameter(InterfaceParameter, PrivateVariable);
	if (bShouldModifyInterfaceParameter)
	{
		InterfaceParameter->Modify();
	}
	if (!bIsReusedVariable)
	{
		InterfaceParameter->Target->Modify();
	}
	InterfaceParameter->PrivateVariable = PrivateVariable;
	CameraParameter->Variable = PrivateVariable;
}

template<typename CameraParameterType>
void SetupPrivateVariable(
		FPrivateVariableBuilder& Builder, 
		UCameraRigInterfaceParameter* InterfaceParameter, 
		FStructProperty* ParameterTargetProperty,
		CameraParameterType* CameraParameter)
{
	using ValueType = typename CameraParameterType::ValueType;
	using VariableAssetType = typename CameraParameterType::VariableAssetType;

	CheckTwiceDrivenParameter(Builder, InterfaceParameter, CameraParameter);

	VariableAssetType* ReusedVariable = Builder.FindReusablePrivateVariable<VariableAssetType>(
			ParameterTargetProperty, InterfaceParameter->Target);

	DoSetupPrivateVariable(Builder, InterfaceParameter, CameraParameter, ReusedVariable);
}

template<typename ParameterOverrideType>
void SetupPrivateVariable(
		FPrivateVariableBuilder& Builder, 
		UCameraRigInterfaceParameter* InterfaceParameter, 
		ParameterOverrideType* ParameterOverride)
{
	using CameraParameterType = typename ParameterOverrideType::CameraParameterType;
	using ValueType = typename CameraParameterType::ValueType;
	using VariableAssetType = typename CameraParameterType::VariableAssetType;

	CheckTwiceDrivenParameter(Builder, InterfaceParameter, &ParameterOverride->Value);

	UCameraRigCameraNode* CameraRigNode = CastChecked<UCameraRigCameraNode>(InterfaceParameter->Target);
	VariableAssetType* ReusedVariable = Builder.FindReusablePrivateVariable<ParameterOverrideType>(
			ParameterOverride, CameraRigNode);

	DoSetupPrivateVariable(Builder, InterfaceParameter, &ParameterOverride->Value, ReusedVariable);
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

		FGameplayCamerasDelegates::OnCameraRigAssetBuilt().Broadcast(CameraRig, BuildLog);
	}
	BuildLog.SetLoggingPrefix(FString());
	UpdateBuildStatus();
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
	BuildNewDrivenParameters();
	DiscardUnusedPrivateVariables();

	BuildAllocationInfo();
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
	// Keep track of what camera parameters were previously driven by private variables,
	// and then clear those variables. This is because it's easier to rebuild this from
	// a blank slate than trying to figure out what changed.
	//
	// As we rebuild things in BuildNewDrivenParameters, we compare to the old state to
	// figure out if we need to flag anything as modified for the current transaction.
	//
	// Note that parameters driven by user-defined variables are left alone.

	TSet<UCameraVariableAsset*> GatheredVariables;
	TSet<UCameraNode*> CameraNodesToGather(CameraNodeHierarchy.GetFlattenedHierarchy());

	// Start by going through all interface parameters, remembering what private variable 
	// they were associated with originally. Also collect that private variable to be
	// renamed and put in the re-use pool.

	OldInterfaceParameters.Reset();

	for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRig->Interface.InterfaceParameters)
	{
		OldInterfaceParameters.Add(InterfaceParameter, FReusableInterfaceParameterInfo(InterfaceParameter->PrivateVariable, false));
		if (InterfaceParameter->PrivateVariable)
		{
			GatheredVariables.Add(InterfaceParameter->PrivateVariable);
		}

		ensureMsgf(
				InterfaceParameter->Target == nullptr || InterfaceParameter->IsInOuter(CameraRig),
				TEXT("Interface parameter '%s' points to camera node '%s' which isn't outer'ed to camera rig '%s'."),
				*InterfaceParameter->InterfaceParameterName,
				*GetNameSafe(InterfaceParameter->Target),
				*GetPathNameSafe(CameraRig));
	}

	// Next go through all the camera nodes we know of. Nodes in CameraNodeHierarchy are the ones
	// connected to the camera rig's root node, so we are missing nodes that were disconnected
	// since the last build. We could use AllNodeTreeObjects for that, but it only exists in 
	// editor builds, and we don't want to rely on unit tests or runtime data manipulation to 
	// have correctly populated it, so we'll try to gather any stray nodes by looking at
	// objects outer'ed to the camera rig.

	OldDrivenParameters.Reset();

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
						OldDrivenParameters.Add(\
								FDrivenParameterKey{ StructProperty, CameraNode },\
								CameraParameterPtr->Variable);\
						GatheredVariables.Add(CameraParameterPtr->Variable);\
						CameraParameterPtr->Variable = nullptr;\
					}\
				}\
			}\
			else
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
			{
				// Some other struct property.
			}
		}

		if (UCameraRigCameraNode* CameraRigNode = Cast<UCameraRigCameraNode>(CameraNode))
		{
			FCameraRigParameterOverrides& ParameterOverrides = CameraRigNode->CameraRigReference.GetParameterOverrides();

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides.Get##ValueName##Overrides())\
			{\
				if (ParameterOverride.Value.Variable)\
				{\
					UObject* VariableOuter = ParameterOverride.Value.Variable->GetOuter();\
					if (VariableOuter == CameraRig)\
					{\
						OldDrivenOverrides.Add(\
								FDrivenOverrideKey{ ParameterOverride.InterfaceParameterGuid, CameraRigNode },\
								ParameterOverride.Value.Variable);\
						GatheredVariables.Add(ParameterOverride.Value.Variable);\
						ParameterOverride.Value.Variable = nullptr;\
					}\
				}\
			}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		}
	}

	// Sanity check: see if we have any stray camera variables, possibly introduced by incorrect
	// editor code or dynamic data manipulation.
	const int32 PreviousNumGatheredVariables = GatheredVariables.Num();
	ForEachObjectWithOuter(CameraRig, [&GatheredVariables](UObject* Obj)
			{
				if (UCameraVariableAsset* CameraVariable = Cast<UCameraVariableAsset>(Obj))
				{
					GatheredVariables.Add(CameraVariable);
				}
			});
	if (GatheredVariables.Num() > PreviousNumGatheredVariables)
	{
		UE_LOG(LogCameraSystem, Verbose, TEXT("Collected %d stray camera variables while building camera rig '%s'."),
				(GatheredVariables.Num() - PreviousNumGatheredVariables), *GetPathNameSafe(CameraRig));
	}

	// Temporarily rename all old camera variables, so their names are available to the new
	// driven parameters.
	for (UCameraVariableAsset* GatheredVariable : GatheredVariables)
	{
		TStringBuilder<256> StringBuilder;
		StringBuilder.Append("REUSABLE_");
		StringBuilder.Append(GatheredVariable->GetName());
		// Rename non-transactionally because if nothing has changed, we will rename it back
		// later and we don't want to dirty the package for nothing.
		GatheredVariable->Rename(StringBuilder.ToString(), nullptr, REN_NonTransactional);
	}
}

void FCameraRigAssetBuilder::BuildNewDrivenParameters()
{
	TSet<FString> UsedInterfaceParameterNames;

	using FBuiltDrivenParameter = TTuple<UCameraNode*, FName>;
	TSet<FBuiltDrivenParameter> BuiltDrivenParameters;

	const FString CameraRigName = CameraRig->GetName();
	const FString CameraRigPathName = CameraRig->GetPathName();

	// Look at the new interface parameters and setup the driven camera node parameters with
	// private camera variables. We have gathered the old ones previously so we can re-use them,
	// instead of creating new variable assets each time.
	//
	// Additionally, we need to handle camera rig nodes with special code, for the case of an
	// interface parameter driving a camera rig override (which in turn drives the inner rig's
	// interface parameter, and so on). This is basically for multi-level interface parameters
	// overrides.
	for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRig->Interface.InterfaceParameters)
	{
		// Do some basic validation.
		if (!InterfaceParameter)
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					CameraRig,
					LOCTEXT("InvalidInterfaceParameter", "Invalid interface parameter or target."));
			continue;
		}
		if (!InterfaceParameter->Target)
		{
			BuildLog.AddMessage(EMessageSeverity::Warning,
					InterfaceParameter,
					LOCTEXT(
						"DisconnectedInterfaceParameter", 
						"Interface parameter isn't connected: setting overrides for it will not do anything."));
			continue;
		}
		if (InterfaceParameter->TargetPropertyName.IsNone())
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					InterfaceParameter,
					LOCTEXT(
						"InvalidInterfaceParameterTargetPropertyName", 
						"Invalid interface parameter target property name."));
			continue;
		}
		if (InterfaceParameter->InterfaceParameterName.IsEmpty())
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					InterfaceParameter,
					LOCTEXT(
						"InvalidInterfaceParameterName",
						"Invalid interface parameter name."));
			continue;
		}

		// Check duplicate parameter names.
		if (UsedInterfaceParameterNames.Contains(InterfaceParameter->InterfaceParameterName))
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					InterfaceParameter,
					FText::Format(LOCTEXT(
						"InterfaceParameterNameCollision",
						"Multiple interface parameters named '{0}'. Ignoring duplicates."),
						FText::FromString(InterfaceParameter->InterfaceParameterName)));
			continue;
		}
		UsedInterfaceParameterNames.Add(InterfaceParameter->InterfaceParameterName);

		// Check duplicate targets.
		FBuiltDrivenParameter BuiltDrivenParameter(InterfaceParameter->Target, InterfaceParameter->TargetPropertyName);
		if (BuiltDrivenParameters.Contains(BuiltDrivenParameter))
		{
			BuildLog.AddMessage(EMessageSeverity::Error,
					InterfaceParameter,
					FText::Format(LOCTEXT(
						"InterfaceParameterTargetCollision",
						"Multiple interface parameters targeting property '{0}' on camera node '{1}'. Ignoring duplicates."),
						FText::FromName(InterfaceParameter->Target->GetFName()),
						FText::FromName(InterfaceParameter->TargetPropertyName)));
			continue;
		}
		BuiltDrivenParameters.Add(BuiltDrivenParameter);

		// See if this interface parameter is overriding a camera node parameter.
		// Otherwise, maybe it's targeting a camera rig node's override for an inner rig interface parameter.
		if (SetupCameraParameterOverride(InterfaceParameter))
		{
			// Implicit continue.
		}
		else if (SetupInnerCameraRigParameterOverride(InterfaceParameter))
		{
			// Implicit continue.
		}
		else
		{
			UCameraNode* Target = InterfaceParameter->Target;
			BuildLog.AddMessage(EMessageSeverity::Error,
					Target,
					FText::Format(LOCTEXT(
						"InvalidInterfaceParameterTargetProperty",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but no such property found."),
						FText::FromString(InterfaceParameter->InterfaceParameterName), 
						FText::FromName(InterfaceParameter->TargetPropertyName),
						FText::FromName(Target->GetFName())));
		}
	}
}

bool FCameraRigAssetBuilder::SetupCameraParameterOverride(UCameraRigInterfaceParameter* InterfaceParameter)
{
	using namespace Internal;

	// Here we hook up interface parameters connected to a camera node property. This property is supposed
	// to be of one of the camera parameter types (FBooleanCameraParameter, FInteger32CameraParameter, etc.)
	// so they have both a fixed value (bool, int32, etc.) and a "private variable" which is a reference to 
	// a corresponding camera variable asset (UBooleanCameraVariable, UInteger32CameraVariable, etc.) which
	// has been set to "private".
	//
	// So the goal of this method is to create a private variable and set it on both the interface parameter
	// and the camera node property. This way, if someone wants to override the value of that interface
	// parameter, they set the value of the variable defined on it. It will then drive the value of the
	// corresponding camera node property.

	UCameraNode* Target = InterfaceParameter->Target;
	UClass* TargetClass = Target->GetClass();
	FProperty* TargetProperty = TargetClass->FindPropertyByName(InterfaceParameter->TargetPropertyName);
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
						"InvalidCameraNodeProperty",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but it's not a camera parameter."),
					FText::FromString(InterfaceParameter->InterfaceParameterName), 
					FText::FromName(InterfaceParameter->TargetPropertyName),
					FText::FromName(Target->GetFName())));
		return true;
	}

	// Get the type of the camera parameter by matching the struct against all the types we support,
	// and create a private camera variable asset to drive its value.
	FPrivateVariableBuilder PrivateVariableBuilder(*this);
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	if (TargetStructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
	{\
		auto* CameraParameterPtr = TargetStructProperty->ContainerPtrToValuePtr<F##ValueName##CameraParameter>(Target);\
		SetupPrivateVariable(PrivateVariableBuilder, InterfaceParameter, TargetStructProperty, CameraParameterPtr);\
	}\
	else
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	{
		BuildLog.AddMessage(EMessageSeverity::Error,
				InterfaceParameter,
				FText::Format(LOCTEXT(
						"InvalidCameraNodeProperty",
						"Invalid interface parameter '{0}', driving property '{1}' on '{2}', but it's not a camera parameter."),
					FText::FromString(InterfaceParameter->InterfaceParameterName), 
					FText::FromName(InterfaceParameter->TargetPropertyName),
					FText::FromName(Target->GetFName())));
	}

	return true;
}

bool FCameraRigAssetBuilder::SetupInnerCameraRigParameterOverride(UCameraRigInterfaceParameter* InterfaceParameter)
{
	using namespace Internal;

	// Here we hook up interface parameters connected specifically to a camera rig node (aka "prefab node").
	// Unlike other camera nodes, the camera rig node doesn't have "actual" properties on it, in the sense 
	// of UClass and FProperty (although we could in the future generate a UClass, like Blueprints do).
	// Camera rig nodes expose the same properties as their inner camera rig, i.e. they "forward expose" the
	// interface parameters defined on their inner camera rig.
	//
	// So the goal of this method is to handle multi-level exposed parameters. That is: we are exposing the
	// interface parameter of an inner camera rig as one of our own interface parameter. Just like in the
	// previous method (see above) we create a private camera variable to set on the interface parameter,
	// but instead of also setting it on a camera node property, here we set it on an override entry on
	// the camera rig node's list of overrides.
	//
	// Note that this camera rig node may or may not have an existing override. If the user forwards the
	// parameter without changing its default value, there would not be an existing override and we have
	// to create our own. If there is an existing override, we set the private variable on it and it
	// will use the user-defined new override value when the variable isn't set.

	UCameraRigCameraNode* Target = Cast<UCameraRigCameraNode>(InterfaceParameter->Target);
	if (!Target)
	{
		// No match, try something else.
		return false;
	}

	// Look for an interface parameter matching the target name.
	UCameraRigAsset* InnerCameraRig = Target->CameraRigReference.GetCameraRig();
	if (!InnerCameraRig)
	{
		return false;
	}
	UCameraRigInterfaceParameter* InnerInterfaceParameter = InnerCameraRig->Interface.FindInterfaceParameterByName(
			InterfaceParameter->TargetPropertyName.ToString());
	if (!InnerInterfaceParameter)
	{
		return false;
	}

	// Found it! Check that the inner camera rig was built.
	if (InnerInterfaceParameter->PrivateVariable == nullptr)
	{
		BuildLog.AddMessage(EMessageSeverity::Error, Target,
				FText::Format(
					LOCTEXT("UnbuiltInnerCameraRig",
						"Can't expose inner camera rig parameter '{0}': the inner camera rig '{1}' failed to build."),
					FText::FromName(InterfaceParameter->TargetPropertyName),
					FText::FromString(GetPathNameSafe(InnerCameraRig))));
		return true;
	}

	// Look for an override that matches the given interface parameter. Create one if we don't find any.
	FCameraRigParameterOverrides& ParameterOverrides = Target->CameraRigReference.GetParameterOverrides();
	switch (InnerInterfaceParameter->PrivateVariable->GetVariableType())
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		case ECameraVariableType::ValueName:\
			{\
				F##ValueName##CameraRigParameterOverride& ValueName##Override = \
						ParameterOverrides.FindOrAddParameterOverride<F##ValueName##CameraRigParameterOverride>(InnerInterfaceParameter);\
				FPrivateVariableBuilder PrivateVariableBuilder(*this);\
				SetupPrivateVariable(PrivateVariableBuilder, InterfaceParameter, &ValueName##Override);\
			}\
			break;
	UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}

	return true;
}

void FCameraRigAssetBuilder::DiscardUnusedPrivateVariables()
{
	// Now that we've rebuilt all exposed parameters, anything left from the old list 
	// must be discarded.
	TSet<UCameraVariableAsset*> VariablesToTrash;

	for (TPair<FDrivenParameterKey, UCameraVariableAsset*> Pair : OldDrivenParameters)
	{
		// We null'ed the driving variable in GatherOldDrivenParameters. Now it's time
		// to flag the camera node as modified.
		UCameraNode* Target = Pair.Key.Value;
		Target->Modify();

		VariablesToTrash.Add(Pair.Value);
	}
	OldDrivenParameters.Reset();

	for (TPair<FDrivenOverrideKey, UCameraVariableAsset*> Pair : OldDrivenOverrides)
	{
		// We null'ed the override variable in GatherOldDrivenParameters. Flag the 
		// camera rig node that owns this override as modified.
		UCameraRigCameraNode* CameraRigNode = Pair.Key.Value;
		CameraRigNode->Modify();

		VariablesToTrash.Add(Pair.Value);
	}
	OldDrivenOverrides.Reset();
	
	// Trash the old camera variable. This helps with debugging.
	for (UCameraVariableAsset* VariableToTrash : VariablesToTrash)
	{
		TStringBuilder<256> StringBuilder;
		StringBuilder.Append("TRASH_");
		StringBuilder.Append(VariableToTrash->GetName());
		VariableToTrash->Rename(StringBuilder.ToString());
	}
}

void FCameraRigAssetBuilder::BuildAllocationInfo()
{
	AllocationInfo = FCameraRigAllocationInfo();

	// Build a mock tree of evaluators.
	FCameraNodeEvaluatorTreeBuildParams BuildParams;
	BuildParams.RootCameraNode = CameraRig->RootNode;
	FCameraNodeEvaluatorStorage Storage;
	Storage.BuildEvaluatorTree(BuildParams);

	// Get the size of the evaluators' allocation.
	Storage.GetAllocationInfo(AllocationInfo.EvaluatorInfo);

	// Compute the allocation info for camera variables.
	for (UCameraNode* CameraNode : CameraNodeHierarchy.GetFlattenedHierarchy())
	{
		BuildAllocationInfo(CameraNode);
	}

	// Set it on the camera rig asset.
	if (CameraRig->AllocationInfo != AllocationInfo)
	{
		CameraRig->Modify();
		CameraRig->AllocationInfo = AllocationInfo;
	}
}

void FCameraRigAssetBuilder::BuildAllocationInfo(UCameraNode* CameraNode)
{
	using namespace UE::Cameras::Internal;

	// Look for properties that are camera parameters, and gather what camera variables they reference. 
	// This is for both exposed rig parameters (which we just built in BuildNewDrivenParameters) and 
	// for parameters driven by user-defined variables.
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
			// Some other struct property.
		}
	}

	// Let the camera node add any custom variables or extra memory.
	FCameraRigBuildContext BuildContext(AllocationInfo, BuildLog);
	CameraNode->Build(BuildContext);
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

