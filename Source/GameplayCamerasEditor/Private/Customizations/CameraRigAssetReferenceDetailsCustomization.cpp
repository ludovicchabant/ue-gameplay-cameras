// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraRigAssetReferenceDetailsCustomization.h"

#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "GameplayCamerasDelegates.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IPropertyTypeCustomization.h"
#include "IPropertyUtilities.h"
#include "IStructureDataProvider.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetReferenceDetailsCustomization"

namespace UE::Cameras
{

/**
 * A class that holds information regarding a given camera rig parameter override, and how it should
 * be shown in a details view panel.
 */
class FCameraRigParameterOverrideDetailRow 
	: public IStructureDataProvider 
	, public TSharedFromThis<FCameraRigParameterOverrideDetailRow>
{
public:

	FCameraRigParameterOverrideDetailRow(TSharedPtr<IPropertyHandle> InCameraRigReferenceProperty, TSharedPtr<IPropertyUtilities> InPropertyUtilities);

	void Initialize(UCameraRigInterfaceParameter* InInterfaceParameter);
	void InitializeAsInvalid(const FString& InInterfaceParameterName, const FGuid& InInterfaceParameterGuid);

	void BuildDetailPropertyRow(IDetailChildrenBuilder& StructBuilder);

	const FString& GetDisplayName() const;
	
private:

	template<typename ParameterOverrideType>
	void InitializeValues();

	template<typename ParameterOverrideType>
	void BuildDetailPropertyRowImpl(IDetailChildrenBuilder& StructBuilder);

	void AccessCameraRigReferences(TArray<FCameraRigAssetReference*>& OutCameraRigReferences) const;
	void ModifyOuterObjects() const;

	template<typename ParameterOverrideType, typename CameraParameterType = typename ParameterOverrideType::CameraParameterType>
	void OnPropertyValueChanged();
	template<typename ParameterOverrideType>
	bool OnIsResetToDefaultVisible(TSharedPtr<IPropertyHandle> PropertyHandle);
	template<typename ParameterOverrideType>
	void OnResetToDefault(TSharedPtr<IPropertyHandle> PropertyHandle);

	void OnRemoveInvalidOverride();

public:

	// IStructureDataProvider interface.
	virtual bool IsValid() const override
	{
		return true; 
	}
	
	virtual const UStruct* GetBaseStructure() const override
	{
		return ParameterType;
	}
	
	virtual void GetInstances(TArray<TSharedPtr<FStructOnScope>>& OutInstances, const UStruct* ExpectedBaseStructure) const override
	{
		OutInstances = ParameterOverrideStructs;
	}

private:

	TSharedPtr<IPropertyHandle> CameraRigReferenceProperty;
	TSharedPtr<IPropertyUtilities> PropertyUtilities;

	UCameraRigInterfaceParameter* InterfaceParameter = nullptr;
	FString InvalidInterfaceParameterName;
	FGuid InvalidInterfaceParameterGuid;

	const UStruct* ParameterType = nullptr;
	TArray<TSharedPtr<FStructOnScope>> ParameterOverrideStructs;
	TSharedPtr<FStructOnScope> DefaultValue;
};

FCameraRigParameterOverrideDetailRow::FCameraRigParameterOverrideDetailRow(TSharedPtr<IPropertyHandle> InCameraRigReferenceProperty, TSharedPtr<IPropertyUtilities> InPropertyUtilities)
	: CameraRigReferenceProperty(InCameraRigReferenceProperty)
	, PropertyUtilities(InPropertyUtilities)
{
}

void FCameraRigParameterOverrideDetailRow::Initialize(UCameraRigInterfaceParameter* InInterfaceParameter)
{
	InterfaceParameter = InInterfaceParameter;

	if (InterfaceParameter && InterfaceParameter->PrivateVariable != nullptr)
	{
		switch (InterfaceParameter->PrivateVariable->GetVariableType())
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		case ECameraVariableType::ValueName:\
			InitializeValues<F##ValueName##CameraRigParameterOverride>();\
			break;
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		}
	}
}

void FCameraRigParameterOverrideDetailRow::InitializeAsInvalid(const FString& InInterfaceParameterName, const FGuid& InInterfaceParameterGuid)
{
	InvalidInterfaceParameterName = InInterfaceParameterName;
	InvalidInterfaceParameterGuid = InInterfaceParameterGuid;
}

template<typename ParameterOverrideType>
void FCameraRigParameterOverrideDetailRow::InitializeValues()
{
	using CameraParameterType = typename ParameterOverrideType::CameraParameterType;
	using VariableAssetType = typename CameraParameterType::VariableAssetType;

	// Get the camera parameter type.
	ParameterType = CameraParameterType::StaticStruct();

	// Get the default value of the camera parameter for this override.
	const VariableAssetType* TypedPrivateVariable = CastChecked<VariableAssetType>(InterfaceParameter->PrivateVariable);
	const CameraParameterType DefaultValueParameter(TypedPrivateVariable->GetDefaultValue());
	DefaultValue = MakeShared<TStructOnScope<CameraParameterType>>(DefaultValueParameter);

	// Add all the actual parameter override values, if any, or the default value, if not.
	TArray<FCameraRigAssetReference*> CameraRigReferences;
	AccessCameraRigReferences(CameraRigReferences);
	for (FCameraRigAssetReference* CameraRigReference : CameraRigReferences)
	{
		check(CameraRigReference);

		FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference->GetParameterOverrides();
		ParameterOverrideType* FoundOverride = ParameterOverrides.FindParameterOverride<ParameterOverrideType>(
				InterfaceParameter->Guid);
		if (FoundOverride)
		{
			ParameterOverrideStructs.Add(MakeShared<TStructOnScope<CameraParameterType>>(FoundOverride->Value));
		}
		else
		{
			ParameterOverrideStructs.Add(MakeShared<TStructOnScope<CameraParameterType>>(DefaultValueParameter));
		}
	}
}

void FCameraRigParameterOverrideDetailRow::BuildDetailPropertyRow(IDetailChildrenBuilder& StructBuilder)
{
	if (InterfaceParameter == nullptr)
	{
		// This is an old parameter override that isn't valid anymore (e.g. the camera rig's interface
		// parameter was deleted).
		FResetToDefaultOverride ResetToDefault = FResetToDefaultOverride::Create(
				TAttribute<bool>(true),
				FSimpleDelegate::CreateSP(this, &FCameraRigParameterOverrideDetailRow::OnRemoveInvalidOverride));

		StructBuilder.AddCustomRow(FText::FromString(InvalidInterfaceParameterName))
			.NameContent()
			[
				SNew(STextBlock)
				.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				.Text(FText::FromString(InvalidInterfaceParameterName))
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.Warning"))
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(STextBlock)
					.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
					.Text(LOCTEXT("InvalidParameterOverrideWarning", "No such parameter found."))
				]
			]
			.OverrideResetToDefault(ResetToDefault);
	}
	else if (InterfaceParameter->PrivateVariable == nullptr)
	{
		// The camera rig wasn't built, show a message.
		StructBuilder.AddCustomRow(FText::FromString(InterfaceParameter->InterfaceParameterName))
			.NameContent()
			[
				SNew(STextBlock)
				.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				.Text(FText::FromString(InterfaceParameter->InterfaceParameterName))
			]
			.ValueContent()
			[
				SNew(STextBlock)
				.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				.Text(LOCTEXT("UnbuiltInnerCameraRigWarning", "Please build the child camera rig."))
			];
	}
	else
	{
		switch (InterfaceParameter->PrivateVariable->GetVariableType())
		{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			case ECameraVariableType::ValueName:\
				BuildDetailPropertyRowImpl<F##ValueName##CameraRigParameterOverride>(StructBuilder);\
				break;
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		}
	}
}

const FString& FCameraRigParameterOverrideDetailRow::GetDisplayName() const
{
	if (InterfaceParameter)
	{
		return InterfaceParameter->InterfaceParameterName;
	}
	else
	{
		return InvalidInterfaceParameterName;
	}
}

void FCameraRigParameterOverrideDetailRow::AccessCameraRigReferences(TArray<FCameraRigAssetReference*>& OutCameraRigReferences) const
{
	TArray<void*> RawData;
	CameraRigReferenceProperty->AccessRawData(RawData);
	for (int32 Index = 0; Index < RawData.Num(); ++Index)
	{
		OutCameraRigReferences.Add(static_cast<FCameraRigAssetReference*>(RawData[Index]));
	}
}

void FCameraRigParameterOverrideDetailRow::ModifyOuterObjects() const
{
	TArray<UObject*> OuterObjects;
	CameraRigReferenceProperty->GetOuterObjects(OuterObjects);
	for (UObject* OuterObject : OuterObjects)
	{
		OuterObject->Modify();
	}
}

template<typename ParameterOverrideType>
void FCameraRigParameterOverrideDetailRow::BuildDetailPropertyRowImpl(IDetailChildrenBuilder& StructBuilder)
{
	using CameraParameterType = typename ParameterOverrideType::CameraParameterType;

	// Add a new property row to the detail view. The row shows our copy of a camera parameter that
	// corresponds to the sort of camera parameter needed to override a given camera rig parameter.
	IDetailPropertyRow* ParameterOverrideRow = StructBuilder.AddExternalStructure(SharedThis(this));

	ParameterOverrideRow->DisplayName(FText::FromString(InterfaceParameter->InterfaceParameterName));

	// Set up callbacks so that when the user edits the camera parameter, we replicate the edit onto
	// the parameter overrides list (by adding/removing the override, or updating its value).
	ParameterOverrideRow->GetPropertyHandle()->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(
				this, &FCameraRigParameterOverrideDetailRow::OnPropertyValueChanged<ParameterOverrideType>));
	ParameterOverrideRow->GetPropertyHandle()->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateSP(
				this, &FCameraRigParameterOverrideDetailRow::OnPropertyValueChanged<ParameterOverrideType>));

	// We need to add our own custom reset-to-default logic, which sets the default value as determined
	// by the values inside the camera rig prefab. However, camera parameters also have a detail 
	// customization that has a custom reset-to-default logic. Slate complaines when both the outer and
	// inner rows have custom reset-to-default logic, so suppress the inner one.
	ParameterOverrideRow->GetPropertyHandle()->SetInstanceMetaData("NoCustomCameraParameterResetToDefault", "true");

	FResetToDefaultOverride ResetToDefault = FResetToDefaultOverride::Create(
			FIsResetToDefaultVisible::CreateSP(
				this, &FCameraRigParameterOverrideDetailRow::OnIsResetToDefaultVisible<ParameterOverrideType>),
			FResetToDefaultHandler::CreateSP(
				this, &FCameraRigParameterOverrideDetailRow::OnResetToDefault<ParameterOverrideType>));
	ParameterOverrideRow->OverrideResetToDefault(ResetToDefault);
}

template<typename ParameterOverrideType, typename CameraParameterType>
void FCameraRigParameterOverrideDetailRow::OnPropertyValueChanged()
{
	const CameraParameterType* TypedDefaultValue = reinterpret_cast<CameraParameterType*>(DefaultValue->GetStructMemory());

	TArray<FCameraRigAssetReference*> CameraRigReferences;
	AccessCameraRigReferences(CameraRigReferences);
	check(CameraRigReferences.Num() == ParameterOverrideStructs.Num());

	ModifyOuterObjects();

	for (int32 Index = 0; Index < CameraRigReferences.Num(); ++Index)
	{
		FCameraRigAssetReference* CameraRigReference(CameraRigReferences[Index]);
		TSharedPtr<FStructOnScope> ParameterOverrideStruct(ParameterOverrideStructs[Index]);

		const CameraParameterType* TypedOverrideParameter = reinterpret_cast<CameraParameterType*>(
				ParameterOverrideStruct->GetStructMemory());

		FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference->GetParameterOverrides();
		const bool bEqualValues = CameraParameterValueEquals<typename CameraParameterType::ValueType>(
				TypedOverrideParameter->Value, TypedDefaultValue->Value);
		if (bEqualValues && TypedOverrideParameter->Variable == nullptr)
		{
			ParameterOverrides.RemoveParameterOverride<ParameterOverrideType>(InterfaceParameter->Guid);
		}
		else
		{
			ParameterOverrideType& Override = ParameterOverrides.FindOrAddParameterOverride<ParameterOverrideType>(InterfaceParameter);
			Override.Value = *TypedOverrideParameter;
		}
	}
}

template<typename ParameterOverrideType>
bool FCameraRigParameterOverrideDetailRow::OnIsResetToDefaultVisible(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	using CameraParameterType = typename ParameterOverrideType::CameraParameterType;

	const CameraParameterType* TypedDefaultValue = reinterpret_cast<CameraParameterType*>(DefaultValue->GetStructMemory());

	for (TSharedPtr<FStructOnScope> ParameterOverrideStruct : ParameterOverrideStructs)
	{
		const CameraParameterType* TypedOverrideParameter = reinterpret_cast<CameraParameterType*>(ParameterOverrideStruct->GetStructMemory());
		if (TypedOverrideParameter->Variable != nullptr ||
				!CameraParameterValueEquals<typename CameraParameterType::ValueType>(
					TypedOverrideParameter->Value, TypedDefaultValue->Value))
		{
			return true;
		}
	}

	return false;
}

template<typename ParameterOverrideType>
void FCameraRigParameterOverrideDetailRow::OnResetToDefault(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	using CameraParameterType = typename ParameterOverrideType::CameraParameterType;

	const CameraParameterType* TypedDefaultValue = reinterpret_cast<CameraParameterType*>(DefaultValue->GetStructMemory());

	ModifyOuterObjects();

	for (TSharedPtr<FStructOnScope> ParameterOverrideStruct : ParameterOverrideStructs)
	{
		CameraParameterType* TypedOverrideParameter = reinterpret_cast<CameraParameterType*>(ParameterOverrideStruct->GetStructMemory());

		TypedOverrideParameter->Value = TypedDefaultValue->Value;
		TypedOverrideParameter->Variable = nullptr;
	}
}

void FCameraRigParameterOverrideDetailRow::OnRemoveInvalidOverride()
{
	if (InvalidInterfaceParameterGuid.IsValid())
	{
		ModifyOuterObjects();

		TArray<FCameraRigAssetReference*> CameraRigReferences;
		AccessCameraRigReferences(CameraRigReferences);

		for (FCameraRigAssetReference* CameraRigReference : CameraRigReferences)
		{
			FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference->GetParameterOverrides();
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			ParameterOverrides.RemoveParameterOverride<F##ValueName##CameraRigParameterOverride>(InvalidInterfaceParameterGuid);
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		}

		if (PropertyUtilities)
		{
			PropertyUtilities->RequestForceRefresh();
		}
	}
}

TSharedRef<IPropertyTypeCustomization> FCameraRigAssetReferenceDetailsCustomization::MakeInstance()
{
	return MakeShared<FCameraRigAssetReferenceDetailsCustomization>();
}

void FCameraRigAssetReferenceDetailsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Remember a few things.
	CameraRigReferenceProperty = StructPropertyHandle;
	PropertyUtilities = StructCustomizationUtils.GetPropertyUtilities();

	// Display the camera rig picker widget.
	TSharedPtr<IPropertyHandle> CameraRigProperty = StructPropertyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FCameraRigAssetReference, CameraRig));

	HeaderRow
	.NameContent()
	[
		CameraRigProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		CameraRigProperty->CreatePropertyValueWidgetWithCustomization(nullptr)
	]
	.ShouldAutoExpand(true);
	
	// Setup a callback so that when a different camera rig is picked, we refresh the list
	// of parameter overrides.
	CameraRigProperty->SetOnPropertyValueChanged(
			FSimpleDelegate::CreateSP(this, &FCameraRigAssetReferenceDetailsCustomization::OnCameraRigChanged));

	// Setup a similar callback for when the selected camera rig get (re)built.
	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().AddSP(this, &FCameraRigAssetReferenceDetailsCustomization::OnCameraRigBuilt);
}

void FCameraRigAssetReferenceDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Update our list of parameter overrides and add each of them as a detail view row
	// below the camera rig picker.
	UpdateParameterOverrides(nullptr, false);
	BuildParameterOverrideRows(StructBuilder);
}

void FCameraRigAssetReferenceDetailsCustomization::OnCameraRigChanged()
{
	UpdateParameterOverrides(nullptr, true);
}

void FCameraRigAssetReferenceDetailsCustomization::OnCameraRigBuilt(UCameraRigAsset* CameraRig, FCameraBuildLog& BuildLog)
{
	UpdateParameterOverrides(CameraRig, true);
}

void FCameraRigAssetReferenceDetailsCustomization::UpdateParameterOverrides(const UCameraRigAsset* CameraRigToUpdate, bool bRequestRefresh)
{
	// Get all the camera rig references we are editing.
	TArray<void*> RawData;
	CameraRigReferenceProperty->AccessRawData(RawData);

	TArray<FCameraRigAssetReference*> CameraRigReferences;
	for (int32 Index = 0; Index < RawData.Num(); Index++)
	{
		FCameraRigAssetReference* CameraRigReference = static_cast<FCameraRigAssetReference*>(RawData[Index]);
		check(CameraRigReference);
		CameraRigReferences.Add(CameraRigReference);
	}

	// Update the parameter overrides for the given camera rig, or for all the ones we're editing if none
	// was specified. As we go, determine if we're editing parameter overrides for the same camera rig
	// across all edited references, or if we have a mix.
	bool bIsEditingNullCameraRig = false;
	TSet<const UCameraRigAsset*> UsedCameraRigs;
	for (FCameraRigAssetReference* CameraRigReference : CameraRigReferences)
	{
		const UCameraRigAsset* CameraRig = CameraRigReference->GetCameraRig();
		if (CameraRigToUpdate == nullptr || CameraRigToUpdate == CameraRig)
		{
			CameraRigReference->UpdateParameterOverrides();
		}

		if (CameraRig)
		{
			UsedCameraRigs.Add(CameraRig);
		}
		else
		{
			bIsEditingNullCameraRig = true;
		}
	}

	// Re-create our list of parameter overrides.
	ParameterOverrideRows.Reset();
	TSet<FGuid> UsedInterfaceParameterGuids;
	if (UsedCameraRigs.Num() == 1 && !bIsEditingNullCameraRig)
	{
		// Add rows for all of the camera rig's parameters.
		const UCameraRigAsset* CameraRig = UsedCameraRigs.Array()[0];
		for (UCameraRigInterfaceParameter* InterfaceParameter : CameraRig->Interface.InterfaceParameters)
		{
			TSharedPtr<FCameraRigParameterOverrideDetailRow> Row = MakeShared<FCameraRigParameterOverrideDetailRow>(
					CameraRigReferenceProperty, PropertyUtilities);
			Row->Initialize(InterfaceParameter);
			ParameterOverrideRows.Add(Row);
			UsedInterfaceParameterGuids.Add(InterfaceParameter->Guid);
		}

		// Add any left-over/invalid parameter overrides.
		for (FCameraRigAssetReference* CameraRigReference : CameraRigReferences)
		{
			FCameraRigParameterOverrides& ParameterOverrides = CameraRigReference->GetParameterOverrides();
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
			for (F##ValueName##CameraRigParameterOverride& ParameterOverride : ParameterOverrides.Get##ValueName##Overrides())\
			{\
				if (!UsedInterfaceParameterGuids.Contains(ParameterOverride.InterfaceParameterGuid))\
				{\
					TSharedPtr<FCameraRigParameterOverrideDetailRow> InvalidRow = MakeShared<FCameraRigParameterOverrideDetailRow>(\
							CameraRigReferenceProperty, PropertyUtilities);\
					InvalidRow->InitializeAsInvalid(ParameterOverride.InterfaceParameterName, ParameterOverride.InterfaceParameterGuid);\
					ParameterOverrideRows.Add(InvalidRow);\
				}\
			}
			UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
		}

		// Sort rows by name.
		ParameterOverrideRows.StableSort(
				[](TSharedPtr<FCameraRigParameterOverrideDetailRow> A,
					TSharedPtr<FCameraRigParameterOverrideDetailRow> B)
				{
					return A->GetDisplayName().Compare(B->GetDisplayName()) < 0;
				});
	}

	if (bRequestRefresh && PropertyUtilities)
	{
		PropertyUtilities->RequestForceRefresh();
	}
}

void FCameraRigAssetReferenceDetailsCustomization::BuildParameterOverrideRows(IDetailChildrenBuilder& StructBuilder)
{
	for (TSharedPtr<FCameraRigParameterOverrideDetailRow> Row : ParameterOverrideRows)
	{
		Row->BuildDetailPropertyRow(StructBuilder);
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

