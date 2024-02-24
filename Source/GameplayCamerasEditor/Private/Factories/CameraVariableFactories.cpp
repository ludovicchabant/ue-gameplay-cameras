// Copyright Epic Games, Inc. All Rights Reserved.

#include "Factories/CameraVariableFactories.h"

#include "AssetTools/AssetDefinition_CameraVariableAssets.h"
#include "AssetToolsModule.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Core/CameraVariableAssets.h"
#include "IAssetTools.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Modules/ModuleManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraVariableFactories)

#define LOCTEXT_NAMESPACE "CameraVariableFactories"

namespace UE::Cameras::Private
{

class FCameraVariableClassFilter : public IClassViewerFilter
{
public:
	TSet<const UClass*> AllowedClasses;
	EClassFlags DisallowedClassFlags;

	FCameraVariableClassFilter()
	{
		AllowedClasses.Add(UCameraVariableAsset::StaticClass());
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

UCameraVariableAssetFactory::UCameraVariableAssetFactory(const FObjectInitializer& ObjectInit)
	: Super(ObjectInit)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UCameraVariableAsset::StaticClass();
}

FText UCameraVariableAssetFactory::GetDisplayName() const
{
	return LOCTEXT("DisplayName", "Camera Variable");
}

uint32 UCameraVariableAssetFactory::GetMenuCategories() const
{
	// By default, UFactory looks up the asset definition for our SupportedClass and asks it for the
	// menu categories to use. However, we have many asset definitions (one per type of camera variable)
	// for just one of us (one factory that creates any camera variable based on the type picker 
	// dialog shown to the user). So we need to reimplement GetMenuCategories and make it return the
	// category we want for all camera variables. It would be easy normally but we need to rewrite
	// the code that converts a list of category paths to a mask of category bits, albeit in a slightly
	// simpler manner.
	static IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	uint32 CategoryBits = 0;
	TConstArrayView<FAssetCategoryPath> Categories = UAssetDefinition_CameraVariableAsset::StaticMenuCategories();
	for (const FAssetCategoryPath& Category : Categories)
	{
		const FName& CategoryName = Category.GetCategory();
		ensure(CategoryName.IsValid());

		const EAssetTypeCategories::Type AdvancedCategoryBit = AssetTools.FindAdvancedAssetCategory(CategoryName);
		if (AdvancedCategoryBit == EAssetTypeCategories::Misc)
		{
			CategoryBits |= AssetTools.RegisterAdvancedAssetCategory(CategoryName, Category.GetCategoryText());
		}
		else
		{
			CategoryBits |= AdvancedCategoryBit;
		}
	}
	return CategoryBits;
}

bool UCameraVariableAssetFactory::ConfigureProperties()
{
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;

	using namespace UE::Cameras::Private;
	TSharedPtr<FCameraVariableClassFilter> Filter = MakeShareable(new FCameraVariableClassFilter);
	Options.ClassFilters.Add(Filter.ToSharedRef());

	UClass* ChosenClass = nullptr;
	CameraVariableAssetType = nullptr;
	const FText TitleText = LOCTEXT("CameraVariablePicker", "Pick Camera Variable Type");
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UCameraVariableAsset::StaticClass());
	if (bPressedOk)
	{
		CameraVariableAssetType = ChosenClass;
	}

	return bPressedOk;
}

UObject* UCameraVariableAssetFactory::FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (CameraVariableAssetType)
	{
		return NewObject<UCameraVariableAsset>(Parent, CameraVariableAssetType, Name, Flags | RF_Transactional);
	}
	else
	{
		check(Class->IsChildOf(UCameraVariableAsset::StaticClass()));
		return NewObject<UCameraVariableAsset>(Parent, Class, Name, Flags);
	}
}

#undef LOCTEXT_NAMESPACE

