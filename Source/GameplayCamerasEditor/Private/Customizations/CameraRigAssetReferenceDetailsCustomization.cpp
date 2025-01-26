// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraRigAssetReferenceDetailsCustomization.h"

#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "GameplayCamerasDelegates.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "PropertyBagDetails.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "CameraRigAssetReferenceDetailsCustomization"

namespace UE::Cameras
{

class FCameraRigAssetParameterOverrideDataDetails : public FPropertyBagInstanceDataDetails
{
public:

	FCameraRigAssetParameterOverrideDataDetails(
			TSharedPtr<IPropertyHandle> InStructPropertyHandle,
			TSharedPtr<IPropertyHandle> InParametersPropertyHandle,
			TSharedPtr<IPropertyUtilities>& InPropertyUtilities)
		: FPropertyBagInstanceDataDetails(InParametersPropertyHandle, InPropertyUtilities, true)
		, StructPropertyHandle(InStructPropertyHandle)
	{
	}

protected:

	struct FCameraRigAssetReferenceOverrideProvider : public IPropertyBagOverrideProvider
	{
		FCameraRigAssetReferenceOverrideProvider(FCameraRigAssetReference& InCameraRigAssetReference)
			: CameraRigAssetReference(InCameraRigAssetReference)
		{
		}
		
		virtual bool IsPropertyOverridden(const FGuid PropertyID) const override
		{
			return CameraRigAssetReference.IsParameterOverridden(PropertyID);
		}
		
		virtual void SetPropertyOverride(const FGuid PropertyID, const bool bIsOverridden) const override
		{
			CameraRigAssetReference.SetParameterOverridden(PropertyID, bIsOverridden);
		}

	private:
		FCameraRigAssetReference& CameraRigAssetReference;
	};

	virtual bool HasPropertyOverrides() const override
	{
		return true;
	}

	virtual void PreChangeOverrides() override
	{
		StructPropertyHandle->NotifyPreChange();
	}

	virtual void PostChangeOverrides() override
	{
		StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		StructPropertyHandle->NotifyFinishedChangingProperties();
	}

	virtual void EnumeratePropertyBags(TSharedPtr<IPropertyHandle> PropertyBagHandle, const EnumeratePropertyBagFuncRef& Func) const override
	{
		StructPropertyHandle->EnumerateRawData([Func](void* RawData, const int32 DataIndex, const int32 NumDatas)
		{
			if (FCameraRigAssetReference* CameraRigAssetReference = static_cast<FCameraRigAssetReference*>(RawData))
			{
				if (const UCameraRigAsset* CameraRigAsset = CameraRigAssetReference->GetCameraRig())
				{
					const FInstancedPropertyBag& DefaultParameters = CameraRigAsset->GetDefaultParameters();
					FInstancedPropertyBag& Parameters = CameraRigAssetReference->GetParameters();
					FCameraRigAssetReferenceOverrideProvider OverrideProvider(*CameraRigAssetReference);
					if (!Func(DefaultParameters, Parameters, OverrideProvider))
					{
						return false;
					}
				}
			}
			return true;
		});
	}

private:
	
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
};

TSharedRef<IPropertyTypeCustomization> FCameraRigAssetReferenceDetailsCustomization::MakeInstance()
{
	return MakeShared<FCameraRigAssetReferenceDetailsCustomization>();
}

FCameraRigAssetReferenceDetailsCustomization::~FCameraRigAssetReferenceDetailsCustomization()
{
	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().RemoveAll(this);
}

void FCameraRigAssetReferenceDetailsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& InHeaderRow, IPropertyTypeCustomizationUtils& InCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;
	PropertyUtilities = InCustomizationUtils.GetPropertyUtilities();

	CameraRigAssetPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCameraRigAssetReference, CameraRig));
	ParametersPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCameraRigAssetReference, Parameters));

	CameraRigAssetPropertyHandle->SetOnPropertyValueChanged(
			FSimpleDelegate::CreateSP(this, &FCameraRigAssetReferenceDetailsCustomization::RebuildParametersIfNeeded));

	if (!GIsTransacting)
	{
		RebuildParametersIfNeeded();
	}

	InHeaderRow
	.ShouldAutoExpand(true)
	.NameContent()
	[
		CameraRigAssetPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		CameraRigAssetPropertyHandle->CreatePropertyValueWidgetWithCustomization(nullptr)
	];
	
	FGameplayCamerasDelegates::OnCameraRigAssetBuilt().AddSP(this, &FCameraRigAssetReferenceDetailsCustomization::OnCameraRigAssetBuilt);
}

void FCameraRigAssetReferenceDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& InChildrenBuilder, IPropertyTypeCustomizationUtils& InCustomizationUtils)
{
	const TSharedRef<FCameraRigAssetParameterOverrideDataDetails> ParameterOverrideDataDetails = 
		MakeShared<FCameraRigAssetParameterOverrideDataDetails>(StructPropertyHandle, ParametersPropertyHandle, PropertyUtilities);
	InChildrenBuilder.AddCustomBuilder(ParameterOverrideDataDetails);
}

void FCameraRigAssetReferenceDetailsCustomization::OnCameraRigAssetBuilt(const UCameraRigAsset* CameraRig)
{
	RebuildParametersIfNeeded();
}

void FCameraRigAssetReferenceDetailsCustomization::RebuildParametersIfNeeded()
{
	TArray<void*> RawData;
	StructPropertyHandle->AccessRawData(RawData);

	bool bRebuiltAny = false;

	for (int32 Index = 0; Index < RawData.Num(); ++Index)
	{
		FCameraRigAssetReference* CameraRigAssetReference = static_cast<FCameraRigAssetReference*>(RawData[Index]);
		if (CameraRigAssetReference)
		{
			const bool bRebuiltOne = CameraRigAssetReference->RebuildParametersIfNeeded();
			bRebuiltAny |= bRebuiltOne;
		}
	}

	if (bRebuiltAny && PropertyUtilities)
	{
		PropertyUtilities->RequestRefresh();
	}
}

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE

