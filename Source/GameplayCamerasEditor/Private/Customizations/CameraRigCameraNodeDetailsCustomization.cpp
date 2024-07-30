// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraRigCameraNodeDetailsCustomization.h"

#include "Core/CameraRigAsset.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "IGameplayCamerasEditorModule.h"
#include "IPropertyUtilities.h"
#include "Modules/ModuleManager.h"
#include "Nodes/Common/CameraRigCameraNode.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "CameraRigCameraNodeDetailsCustomization"

namespace UE::Cameras
{

class FParameterOverrideDetailRows : public TSharedFromThis<FParameterOverrideDetailRows>
{
public:

	FParameterOverrideDetailRows(
			UCameraRigCameraNode* InCameraRigNode,
			IDetailLayoutBuilder& InLayoutBuilder,
			IDetailCategoryBuilder& InOverridesCategory, 
			TSharedRef<IPropertyUtilities> InPropertyUtilities)
		: CameraRigNode(InCameraRigNode)
		, LayoutBuilder(InLayoutBuilder)
		, OverridesCategory(InOverridesCategory)
		, PropertyUtilities(InPropertyUtilities)
	{}

	void AddParameterOverrideDetailRow(const UCameraRigInterfaceParameter* InterfaceParameter)
	{
		if (!InterfaceParameter)
		{
			return;
		}

		UCameraNode* TargetNode = InterfaceParameter->Target;
		FName TargetPropertyName = InterfaceParameter->TargetPropertyName;
		if (!TargetNode || TargetPropertyName.IsNone())
		{
			return;
		}

		UClass* TargetNodeClass = TargetNode->GetClass();
		FStructProperty* TargetProperty = CastField<FStructProperty>(TargetNodeClass->FindPropertyByName(TargetPropertyName));
		if (!TargetProperty)
		{
			return;
		}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		if (TargetProperty->Struct == F##ValueName##CameraParameter::StaticStruct())\
		{\
			using FCameraRigParameterOverrideType = F##ValueName##CameraRigParameterOverride;\
			AddParameterOverrideDetailRowImpl<FCameraRigParameterOverrideType>(InterfaceParameter, TargetProperty);\
		}\
		else
		UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		{
			// Unknown sort of property...
		}
	}

private:

	template<typename CameraRigParameterOverrideType>
	void AddParameterOverrideDetailRowImpl(const UCameraRigInterfaceParameter* InterfaceParameter, FStructProperty* TargetProperty)
	{
		using CameraParameterType = typename CameraRigParameterOverrideType::CameraParameterType;

		const FGuid ParameterGuid = InterfaceParameter->Guid;

		// Get the default value of this parameter from the original target node.
		UObject* TargetNode = InterfaceParameter->Target;
		const CameraParameterType* DefaultParameterPtr = TargetProperty->ContainerPtrToValuePtr<CameraParameterType>(TargetNode);

		// Create a new version of the target node, and get a pointer to the parameter we're interested
		// in showing in the details view.
		UClass* TargetNodeClass = InterfaceParameter->Target->GetClass();
		UObject* WrapperObject = NewObject<UObject>(GetTransientPackage(), TargetNodeClass);
		CameraParameterType* ScratchParameterPtr = TargetProperty->ContainerPtrToValuePtr<CameraParameterType>(WrapperObject);
		{
			// If we already have an override, show its value. Otherwise, show the original value.
			CameraRigParameterOverrideType* Override = CameraRigNode->FindParameterOverride<CameraRigParameterOverrideType>(ParameterGuid);
			if (Override)
			{
				*ScratchParameterPtr = Override->Value;
			}
			else
			{
				ScratchParameterPtr->Value = DefaultParameterPtr->Value;
			}
		}
		// Make sure this temporary object isn't GC'ed while the details view is still shown.
		WrapperObjects.Add(TStrongObjectPtr<UObject>(WrapperObject));

		// Add the scratch parameter to the details view.
		TSharedPtr<IPropertyHandle> ParameterPropertyHandle = LayoutBuilder.AddObjectPropertyData(
				{ WrapperObject }, InterfaceParameter->TargetPropertyName);
		
		FText ParameterNameText(FText::FromString(InterfaceParameter->InterfaceParameterName));
		ParameterPropertyHandle->SetPropertyDisplayName(ParameterNameText);

		IDetailPropertyRow& PropertyRow = OverridesCategory.AddProperty(ParameterPropertyHandle);
		
		// Register a change callback so that we can propagate edits made to the scratch parameter onto 
		// the camera rig node's overrides.
		PropertyRow.GetPropertyHandle()->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(
					this,
					&FParameterOverrideDetailRows::OnPropertyValueChanged<CameraRigParameterOverrideType, CameraParameterType>,
					InterfaceParameter, DefaultParameterPtr, ScratchParameterPtr));
		// Register the same callback for when the user edits a child property, such as the X/Y/Z coordinates
		// of a 3d vector camera parameter.
		PropertyRow.GetPropertyHandle()->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateSP(
					this,
					&FParameterOverrideDetailRows::OnPropertyValueChanged<CameraRigParameterOverrideType, CameraParameterType>,
					InterfaceParameter, DefaultParameterPtr, ScratchParameterPtr));

		// Setup custom reset-to-default logic.
		FResetToDefaultOverride ResetToDefault = FResetToDefaultOverride::Create(
				FIsResetToDefaultVisible::CreateLambda(
					[DefaultParameterPtr, ScratchParameterPtr](TSharedPtr<IPropertyHandle> PropertyHandle) -> bool
					{
						return ScratchParameterPtr->Variable != nullptr ||
							!CameraParameterValueEquals<typename CameraParameterType::ValueType>(
								ScratchParameterPtr->Value, DefaultParameterPtr->Value);
					}),
				FResetToDefaultHandler::CreateLambda(
					[DefaultParameterPtr, ScratchParameterPtr](TSharedPtr<IPropertyHandle> PropertyHandle)
					{
						ScratchParameterPtr->Value = DefaultParameterPtr->Value;
						ScratchParameterPtr->Variable = nullptr;
					}));
		PropertyRow.OverrideResetToDefault(ResetToDefault);
	}

	template<typename CameraRigParameterOverrideType, typename CameraParameterType>
	void OnPropertyValueChanged(
			const UCameraRigInterfaceParameter* InterfaceParameter, 
			const CameraParameterType* DefaultParameterPtr,
			CameraParameterType* ScratchParameterPtr)
	{
		CameraRigNode->Modify();

		const bool bEqualValues = CameraParameterValueEquals<typename CameraParameterType::ValueType>(
				ScratchParameterPtr->Value, DefaultParameterPtr->Value);
		if (bEqualValues && ScratchParameterPtr->Variable == nullptr)
		{
			CameraRigNode->RemoveParameterOverride<CameraRigParameterOverrideType>(InterfaceParameter->Guid);
		}
		else
		{
			CameraRigParameterOverrideType& Override = CameraRigNode->GetOrAddParameterOverride<CameraRigParameterOverrideType>(InterfaceParameter);
			Override.Value = *ScratchParameterPtr;
		}

		PropertyUtilities->RequestForceRefresh();
	}

private:

	UCameraRigCameraNode* CameraRigNode;
	IDetailLayoutBuilder& LayoutBuilder;
	IDetailCategoryBuilder& OverridesCategory;
	TSharedRef<IPropertyUtilities> PropertyUtilities;

	TArray<TStrongObjectPtr<UObject>> WrapperObjects;
};

TSharedRef<IDetailCustomization> FCameraRigCameraNodeDetailsCustomization::MakeInstance()
{
	return MakeShared<FCameraRigCameraNodeDetailsCustomization>();
}

void FCameraRigCameraNodeDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Only support a single node selected for now.
	TArray<TWeakObjectPtr<UCameraRigCameraNode>> WeakNodes = DetailBuilder.GetSelectedObjectsOfType<UCameraRigCameraNode>();
	if (WeakNodes.IsEmpty())
	{
		return;
	}

	// Skip invalid nodes.
	UCameraRigCameraNode* CameraRigNode = WeakNodes[0].Get();
	if (!CameraRigNode || !CameraRigNode->CameraRig)
	{
		return;
	}

	// Skip camera rigs that don't have anything exposed.
	FCameraRigInterface& Interface = CameraRigNode->CameraRig->Interface;
	if (Interface.InterfaceParameters.IsEmpty())
	{
		return;
	}

	IDetailCategoryBuilder& OverridesCategory = DetailBuilder.EditCategory(
			TEXT("ParameterOverrides"), 
			LOCTEXT("ParameterOverridesCategory", "Parameter Overrides"));

	ParameterOverrideRows = MakeShared<FParameterOverrideDetailRows>(
			CameraRigNode,
			DetailBuilder,
			OverridesCategory,
			DetailBuilder.GetPropertyUtilities());

	for (const UCameraRigInterfaceParameter* InterfaceParameter : Interface.InterfaceParameters)
	{
		ParameterOverrideRows->AddParameterOverrideDetailRow(InterfaceParameter);
	}
}

void FCameraRigCameraNodeDetailsCustomization::PendingDelete()
{
	ParameterOverrideRows.Reset();
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

