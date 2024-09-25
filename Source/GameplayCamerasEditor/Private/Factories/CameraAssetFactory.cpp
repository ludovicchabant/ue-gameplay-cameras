// Copyright Epic Games, Inc. All Rights Reserved.

#include "Factories/CameraAssetFactory.h"

#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraRigAsset.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Directors/BlueprintCameraDirector.h"
#include "Directors/SingleCameraDirector.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Modules/ModuleManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraAssetFactory)

#define LOCTEXT_NAMESPACE "CameraAssetFactory"

namespace UE::Cameras::Private
{

class FCameraDirectorClassFilter : public IClassViewerFilter
{
public:
	TSet<const UClass*> AllowedClasses;
	EClassFlags DisallowedClassFlags;

	FCameraDirectorClassFilter()
	{
		AllowedClasses.Add(UCameraDirector::StaticClass());
		DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated;
	}

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return 
			!InClass->HasAnyClassFlags(DisallowedClassFlags) &&
			InFilterFuncs->IfInChildOfClassesSet(AllowedClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return 
			!InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags) && 
			InFilterFuncs->IfInChildOfClassesSet(AllowedClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};

}  // namespace UE::Cameras::Private

UCameraAssetFactory::UCameraAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCameraAsset::StaticClass();
}

UObject* UCameraAssetFactory::FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UCameraAsset* NewCameraAsset = NewObject<UCameraAsset>(Parent, Class, Name, Flags | RF_Transactional);

	UCameraRigAsset* FirstCameraRig = NewObject<UCameraRigAsset>(NewCameraAsset, NAME_None, RF_Transactional | RF_Public);
	NewCameraAsset->AddCameraRig(FirstCameraRig);

	if (CameraDirectorClass)
	{
		UCameraDirector* NewCameraDirector = NewObject<UCameraDirector>(NewCameraAsset, CameraDirectorClass, NAME_None, RF_Transactional);
		NewCameraAsset->SetCameraDirector(NewCameraDirector);

		// Let the camera director do some scaffolding.
		FCameraDirectorFactoryCreateParams CreateParams;
		NewCameraDirector->FactoryCreateAsset(CreateParams);
	}

	return NewCameraAsset;
}

bool UCameraAssetFactory::ConfigureProperties()
{
	using namespace UE::Cameras::Private;

	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	Options.bShowNoneOption = true;

	Options.ExtraPickerCommonClasses.Add(UBlueprintCameraDirector::StaticClass());
	Options.ExtraPickerCommonClasses.Add(USingleCameraDirector::StaticClass());

	TSharedPtr<FCameraDirectorClassFilter> Filter = MakeShareable(new FCameraDirectorClassFilter);
	Options.ClassFilters.Add(Filter.ToSharedRef());

	CameraDirectorClass = nullptr;
	UClass* ChosenClass = nullptr;
	const FText TitleText = LOCTEXT("CameraDirectorPicker", "Pick Camera Director Type");
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UCameraDirector::StaticClass());
	if (bPressedOk)
	{
		CameraDirectorClass = ChosenClass;
	}
	return bPressedOk;
}

bool UCameraAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE


