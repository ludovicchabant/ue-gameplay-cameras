// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CameraRigAsset.h"

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAllocationInfoBuilder.h"
#include "Core/CameraVariableAssets.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraRigAsset)

namespace UE::Cameras::Internal
{

template<typename VariableAssetType, typename ValueType>
void SetPrivateVariableDefaultValue(VariableAssetType* PrivateVariable, typename TCallTraits<ValueType>::ParamType Value)
{
	PrivateVariable->DefaultValue = Value;
}

template<>
void SetPrivateVariableDefaultValue<UBooleanCameraVariable, bool>(UBooleanCameraVariable* PrivateVariable, bool bValue)
{
	PrivateVariable->bDefaultValue = bValue;
}

template<typename CameraParameterType>
UCameraVariableAsset* CreatePrivateVariable(UCameraRigAsset* CameraRigAsset, UCameraRigInterfaceParameter* InterfaceParameter, const FString& VariableName, CameraParameterType* CameraParameter)
{
	if (CameraParameter->Variable != nullptr)
	{
		UE_LOG(LogCameraSystem, Error, 
				TEXT("Invalid interface parameter in camera rig '%s': '%s.%s' is already driven by variable."), 
				*CameraRigAsset->GetPathName(), 
				*InterfaceParameter->Target->GetName(), 
				*InterfaceParameter->TargetPropertyName.ToString());
		return nullptr;
	}

	using ValueType = typename CameraParameterType::ValueType;
	using VariableAssetType = typename CameraParameterType::VariableAssetType;

	VariableAssetType* PrivateVariable = NewObject<VariableAssetType>(CameraRigAsset, *VariableName);
	PrivateVariable->bIsPrivate = true;
	PrivateVariable->bAutoReset = false;
	SetPrivateVariableDefaultValue<VariableAssetType, ValueType>(PrivateVariable, CameraParameter->Value);

	InterfaceParameter->PrivateVariable = PrivateVariable;
	CameraParameter->Variable = PrivateVariable;

	return PrivateVariable;
}

}  // namespace UE::Cameras::Internal

#if WITH_EDITOR

void UCameraRigInterfaceParameter::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigInterfaceParameter::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

#endif

UCameraRigInterfaceParameter* FCameraRigInterface::FindInterfaceParameterByName(const FString& ParameterName) const
{
	const TObjectPtr<UCameraRigInterfaceParameter>* FoundItem = InterfaceParameters.FindByPredicate([&ParameterName](UCameraRigInterfaceParameter* Item)
			{
				return Item->InterfaceParameterName == ParameterName;
			});
	return FoundItem ? *FoundItem : nullptr;
}

bool FCameraRigInterface::HasInterfaceParameter(const FString& ParameterName) const
{
	return FindInterfaceParameterByName(ParameterName) != nullptr;
}

void UCameraRigAsset::BuildCameraRig()
{
	using namespace UE::Cameras;
	using namespace UE::Cameras::Internal;

	// Build allocation info.
	FCameraRigAllocationInfoBuilder CameraRigBuilder;
	CameraRigBuilder.BuildAllocationInfo(this, AllocationInfo);
	BuildStatus = ECameraRigBuildStatus::Clean;

	// Build interface parameters.
	TSet<FString> UsedInterfaceParameterNames;
	for (UCameraRigInterfaceParameter* InterfaceParameter : Interface.InterfaceParameters)
	{
		// Do some basic validation.
		if (!InterfaceParameter || !InterfaceParameter->Target)
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid interface parameter target in camera rig: '%s'."),
					*GetPathName());
			continue;
		}
		if (InterfaceParameter->TargetPropertyName.IsNone())
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid interface parameter target property name in camera rig: '%s'."), 
					*GetPathName());
			continue;
		}
		if (InterfaceParameter->InterfaceParameterName.IsEmpty())
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid interface parameter name in camera rig '%s'."), 
					*GetPathName());
			continue;
		}

		if (UsedInterfaceParameterNames.Contains(InterfaceParameter->InterfaceParameterName))
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Multiple interface parameters named '%s' in camera rig '%s'. Ignoring duplicates."), 
					*InterfaceParameter->InterfaceParameterName, 
					*GetPathName());
			continue;
		}
		UsedInterfaceParameterNames.Add(InterfaceParameter->InterfaceParameterName);

		// Get the target camera node property and check that it is a camera parameter struct.
		UClass* TargetClass = InterfaceParameter->Target->GetClass();
		FProperty* TargetProperty = TargetClass->FindPropertyByName(InterfaceParameter->TargetPropertyName);
		if (!TargetProperty)
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid interface parameter in camera rig '%s': no property '%s' on camera node '%s'."), 
					*GetPathName(), 
					*InterfaceParameter->InterfaceParameterName, 
					*InterfaceParameter->Target->GetName());
			continue;
		}

		FStructProperty* TargetStructProperty = CastField<FStructProperty>(TargetProperty);
		if (!TargetStructProperty)
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid interface parameter in camera rig '%s': property '%s' on camera node '%s' is not a camera parameter."),
					*GetPathName(), 
					*InterfaceParameter->InterfaceParameterName, 
					*InterfaceParameter->Target->GetName());
			continue;
		}

		// Get the type of the camera parameter by matching the struct against all the types we support,
		// and create a private camera variable asset to drive its value.
		FString PrivateVariableName(FString::Format(TEXT("Override_{0}_{1}"), { GetName(), InterfaceParameter->InterfaceParameterName }));
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		if (TargetStructProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
		{\
			F##ValueName##CameraParameter CameraParameter;\
			TargetStructProperty->GetValue_InContainer(InterfaceParameter->Target, reinterpret_cast<void*>(&CameraParameter));\
			CreatePrivateVariable(this, InterfaceParameter, PrivateVariableName, &CameraParameter);\
			TargetStructProperty->SetValue_InContainer(InterfaceParameter->Target, reinterpret_cast<void*>(&CameraParameter));\
		}\
		else
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		{
			UE_LOG(LogCameraSystem, Error, 
					TEXT("Invalid interface parameter in camera rig '%s': property '%s' on camera node '%s' is not a camera parameter."), 
					*GetPathName(), 
					*InterfaceParameter->InterfaceParameterName,
					*InterfaceParameter->Target->GetName());
			continue;
		}
	}
}

void UCameraRigAsset::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
#if WITH_EDITOR
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		// Build on save.
		BuildCameraRig();
	}
#endif

	Super::PreSave(ObjectSaveContext);
}

#if WITH_EDITOR

void UCameraRigAsset::GatherPackages(FCameraRigPackages& OutPackages) const
{
	TArray<UCameraNode*> NodeStack;
	if (RootNode)
	{
		NodeStack.Add(RootNode);
	}
	while (!NodeStack.IsEmpty())
	{
		UCameraNode* CurrentNode = NodeStack.Pop();
		const UPackage* CurrentPackage = CurrentNode->GetOutermost();
		OutPackages.AddUnique(CurrentPackage);

		FCameraNodeChildrenView CurrentChildren = CurrentNode->GetChildren();
		for (UCameraNode* CurrentChild : ReverseIterate(CurrentChildren))
		{
			NodeStack.Add(CurrentChild);
		}
	}
}

void UCameraRigAsset::GetGraphNodePosition(int32& NodePosX, int32& NodePosY) const
{
	NodePosX = GraphNodePosX;
	NodePosY = GraphNodePosY;
}

void UCameraRigAsset::OnGraphNodeMoved(int32 NodePosX, int32 NodePosY)
{
	GraphNodePosX = NodePosX;
	GraphNodePosY = NodePosY;
}

const FString& UCameraRigAsset::GetGraphNodeCommentText() const
{
	return GraphNodeComment;
}

void UCameraRigAsset::OnUpdateGraphNodeCommentText(const FString& NewComment)
{
	GraphNodeComment = NewComment;
}

#endif  // WITH_EDITOR
