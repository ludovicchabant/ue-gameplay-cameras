// Copyright Epic Games, Inc. All Rights Reserved.

#include "MovieScene/GameplayCamerasTrackInstancePropertyBindings.h"

#include "Core/CameraAsset.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigParameterDefinition.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "MovieSceneCommonHelpers.h"

namespace UE::Cameras
{

/**
 * A custom property binding handler for camera parameters.
 */
struct FGameplayCamerasTrackInstanceCustomPropertyBindingHandler : public ITrackInstanceCustomPropertyBindingHandler
{
public:

	virtual bool FindProperty(void* BasePointer, const FString& PropertyName, FTrackInstanceCustomProperty& OutProperty) override
	{
		UGameplayCameraComponent* CameraComponent = reinterpret_cast<UGameplayCameraComponent*>(BasePointer);
		check(CameraComponent);

		// See if we have already resolved this parameter before.
		FResolvedParameterKey Key{ CameraComponent, FName(PropertyName) };
		if (FResolvedParameterInfo* ExistingInfo = ResolvedParameters.Find(Key))
		{
			OutProperty.ContainerType = ETrackInstancePropertyContainerType::None;
			OutProperty.PropertyStruct = ExistingInfo->CustomPropertyStruct;
			OutProperty.PropertyID = ExistingInfo->CustomPropertyID;
			return true;
		}

		// Binding to a camera component that doesn't have a camera reference set is ignored.
		const UCameraAsset* CameraAsset = CameraComponent->CameraReference.GetCameraAsset();
		if (CameraAsset == nullptr)
		{
			return false;
		}

		TConstArrayView<FCameraRigParameterDefinition> ParameterDefinitions = CameraAsset->GetParameterDefinitions();
		const FCameraRigParameterDefinition* ParameterDefinition = ParameterDefinitions.FindByPredicate(
				[&PropertyName](const FCameraRigParameterDefinition& Item)
				{
					return Item.ParameterName == PropertyName;
				});
		if (!ParameterDefinition)
		{
			// We might not find the parameter definition if the asset is out of date and hasn't been built.
			return false;
		}

		// See if we can find a default value for this parameter.
		const FInstancedPropertyBag& DefaultParameters = CameraAsset->GetDefaultParameters();
		const UPropertyBag* DefaultParametersStruct = DefaultParameters.GetPropertyBagStruct();
		if (!DefaultParametersStruct)
		{
			return false;
		}

		const FPropertyBagPropertyDesc* PropertyDesc = DefaultParametersStruct->FindPropertyDescByID(ParameterDefinition->ParameterGuid);
		if (!PropertyDesc || !PropertyDesc->CachedProperty)
		{
			return false;
		}

		const uint8* RawDefaultParameters = DefaultParameters.GetValue().GetMemory();
		const void* RawDefaultValuePtr = PropertyDesc->CachedProperty->ContainerPtrToValuePtr<void>(RawDefaultParameters);
		if (!RawDefaultValuePtr)
		{
			return false;
		}

		// Cache the info for this resolved parameter.
		FResolvedParameterInfo& NewInfo = ResolvedParameters.Emplace(Key);
		NewInfo.Definition = *ParameterDefinition;
		NewInfo.RawDefaultValuePtr = RawDefaultValuePtr;
		NewInfo.CustomPropertyID = NextCustomPropertyID++;
		if (ParameterDefinition->ParameterType == ECameraRigInterfaceParameterType::Blendable)
		{
			switch (ParameterDefinition->VariableType)
			{
				case ECameraVariableType::Vector2f:
					NewInfo.CustomPropertyStruct = TVariantStructure<FVector2f>::Get();
					break;
				case ECameraVariableType::Vector2d:
					NewInfo.CustomPropertyStruct = TBaseStructure<FVector2D>::Get();
					break;
				case ECameraVariableType::Vector3f:
					NewInfo.CustomPropertyStruct = TVariantStructure<FVector3f>::Get();
					break;
				case ECameraVariableType::Vector3d:
					NewInfo.CustomPropertyStruct = TBaseStructure<FVector>::Get();
					break;
				case ECameraVariableType::Vector4f:
					NewInfo.CustomPropertyStruct = TVariantStructure<FVector4f>::Get();
					break;
				case ECameraVariableType::Vector4d:
					NewInfo.CustomPropertyStruct = TBaseStructure<FVector4>::Get();
					break;
				case ECameraVariableType::Rotator3f:
					NewInfo.CustomPropertyStruct = TVariantStructure<FRotator3f>::Get();
					break;
				case ECameraVariableType::Rotator3d:
					NewInfo.CustomPropertyStruct = TBaseStructure<FRotator>::Get();
					break;
				case ECameraVariableType::Transform3f:
					NewInfo.CustomPropertyStruct = TVariantStructure<FTransform3f>::Get();
					break;
				case ECameraVariableType::Transform3d:
					NewInfo.CustomPropertyStruct = TBaseStructure<FTransform>::Get();
					break;
				case ECameraVariableType::BlendableStruct:
					NewInfo.CustomPropertyStruct = const_cast<UScriptStruct*>(ParameterDefinition->BlendableStructType.Get());
					break;
			}
		}
		else if (ParameterDefinition->ParameterType == ECameraRigInterfaceParameterType::Data)
		{
			NewInfo.CustomPropertyStruct = const_cast<UStruct*>(Cast<const UStruct>(ParameterDefinition->DataTypeObject));
		}

		CustomPropertyIDToParameterKey.Add(NewInfo.CustomPropertyID, Key);

		OutProperty.ContainerType = ETrackInstancePropertyContainerType::None;
		OutProperty.PropertyStruct = NewInfo.CustomPropertyStruct;
		OutProperty.PropertyID = NewInfo.CustomPropertyID;

		return true;
	}

	virtual void* ContainerPtrToValuePtr(void* BasePointer, const FTrackInstanceCustomProperty& CustomProperty, int32 ArrayIndex = INDEX_NONE) override
	{
		// Find a previous resolved camera parameter.
		UGameplayCameraComponent* CameraComponent = reinterpret_cast<UGameplayCameraComponent*>(BasePointer);
		if (!ensure(CameraComponent))
		{
			return nullptr;
		}

		TSharedPtr<FCameraEvaluationContext> EvaluationContext = CameraComponent->GetEvaluationContext();
		if (!EvaluationContext)
		{
			return nullptr;
		}

		const FResolvedParameterKey* KeyPtr = CustomPropertyIDToParameterKey.Find(CustomProperty.PropertyID);
		if (!ensure(KeyPtr))
		{
			return nullptr;
		}

		const FResolvedParameterInfo* ResolvedParameterPtr = ResolvedParameters.Find(*KeyPtr);
		if (!ensure(ResolvedParameterPtr))
		{
			return nullptr;
		}

		// Get the pointer to the value in the variable table or context data table. Since we don't know if
		// this pointer will be used for reading or writing, we need to make sure it points to something
		// valid. So if the value is uninitialized, we write the default value into it.
		const FCameraRigParameterDefinition& Definition = ResolvedParameterPtr->Definition;
		if (Definition.ParameterType == ECameraRigInterfaceParameterType::Blendable)
		{
			FCameraVariableTable& VariableTable = EvaluationContext->GetInitialResult().VariableTable;
			uint8* RawValuePtr = VariableTable.TryGetMutableValue(Definition.VariableID, Definition.VariableType, Definition.BlendableStructType);
			if (!VariableTable.IsValueWritten(Definition.VariableID))
			{
				VariableTable.SetValue(Definition.VariableID, Definition.VariableType, Definition.BlendableStructType, (uint8*)ResolvedParameterPtr->RawDefaultValuePtr);
			}
			return RawValuePtr;
		}
		else if (Definition.ParameterType == ECameraRigInterfaceParameterType::Data)
		{
			FCameraContextDataTable& ContextDataTable = EvaluationContext->GetInitialResult().ContextDataTable;
			uint8* RawValuePtr = ContextDataTable.TryGetMutableData(Definition.DataID, Definition.DataType, Definition.DataTypeObject);
			if (!ContextDataTable.IsValueWritten(Definition.DataID))
			{
				ContextDataTable.SetData(Definition.DataID, Definition.DataType, Definition.DataTypeObject, (uint8*)ResolvedParameterPtr->RawDefaultValuePtr);
			}
			return RawValuePtr;
		}

		return nullptr;
	}

private:

	struct FResolvedParameterKey
	{
		FObjectKey CameraComponentKey = nullptr;
		FName ParameterName = NAME_None;

		bool operator==(const FResolvedParameterKey& Other) const = default;

		friend int32 GetTypeHash(const FResolvedParameterKey& Key)
		{
			return HashCombine(GetTypeHash(Key.CameraComponentKey), GetTypeHash(Key.ParameterName));
		}
	};

	struct FResolvedParameterInfo
	{
		FCameraRigParameterDefinition Definition;
		const void* RawDefaultValuePtr = nullptr;
		UStruct* CustomPropertyStruct = nullptr;
		int32 CustomPropertyID = 0;
	};

	TMap<FResolvedParameterKey, FResolvedParameterInfo> ResolvedParameters;
	TMap<int32, FResolvedParameterKey> CustomPropertyIDToParameterKey;
	int32 NextCustomPropertyID = 0;
};

void FGameplayCamerasTrackInstancePropertyBindings::Register()
{
	using namespace UE::MovieScene;

	UClass* CameraComponentClass = UGameplayCameraComponent::StaticClass();
	FTrackInstancePropertyBindings::RegisterCustomHandler(
			CameraComponentClass, 
			FOnCreateTrackInstanceCustomPropertyBindingHandler::CreateLambda(
				[](){ return MakeShared<FGameplayCamerasTrackInstanceCustomPropertyBindingHandler>(); }));
}

void FGameplayCamerasTrackInstancePropertyBindings::Unregister()
{
	using namespace UE::MovieScene;

	UClass* CameraComponentClass = UGameplayCameraComponent::StaticClass();
	FTrackInstancePropertyBindings::UnregisterCustomHandler(CameraComponentClass);
}

}  // namespace UE::Cameras

