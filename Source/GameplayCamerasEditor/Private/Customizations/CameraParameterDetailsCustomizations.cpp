// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/CameraParameterDetailsCustomizations.h"

#include "ContentBrowserModule.h"
#include "Core/CameraParameters.h"
#include "Core/CameraVariableAssets.h"
#include "Core/CameraVariableCollection.h"
#include "Customizations/MathStructCustomizations.h"
#include "DetailLayoutBuilder.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Views/ITypedTableView.h"
#include "IContentBrowserSingleton.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailPropertyRow.h"
#include "IPropertyUtilities.h"
#include "Modules/ModuleManager.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Styling/StyleColors.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"

#define LOCTEXT_NAMESPACE "CameraParameterDetailsCustomization"

namespace UE::Cameras
{

DECLARE_DELEGATE_OneParam(FExecuteSetParameterVariable, UCameraVariableAsset*);

class SCameraVariableBrowser : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCameraVariableBrowser)
	{}
		SLATE_ARGUMENT(UClass*, VariableClass)
		SLATE_ARGUMENT(UCameraVariableAsset*, InitialVariable)
		SLATE_EVENT(FExecuteSetParameterVariable, RequestSetVariable)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs)
	{
		VariableClass = InArgs._VariableClass;
		InitialVariable = InArgs._InitialVariable;

		RequestSetVariable = InArgs._RequestSetVariable;

		ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(0.5f)
				[
					BuildVariableCollectionAssetPicker()
				]
				+SVerticalBox::Slot()
				.FillHeight(0.5f)
				[
					SAssignNew(CameraVariableListView, SListView<UCameraVariableAsset*>)
					.ListItemsSource(&CameraVariableItemsSource)
					.OnGenerateRow(this, &SCameraVariableBrowser::OnVariableListGenerateRow)
					.OnSelectionChanged(this, &SCameraVariableBrowser::OnVariableListSelectionChanged)
				]
			]
		];

		if (InitialVariable)
		{
			bSuppressVariableListSelectionChanged = true;
			CameraVariableListView->SetSelection(InitialVariable);
			CameraVariableListView->RequestScrollIntoView(InitialVariable);
			bSuppressVariableListSelectionChanged = false;
		}
	}

private:

	TSharedRef<SWidget> BuildVariableCollectionAssetPicker()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

		FAssetPickerConfig AssetPickerConfig;

		FARFilter ARFilter;
		ARFilter.ClassPaths.Add(FTopLevelAssetPath(UCameraVariableCollection::StaticClass()->GetPathName()));

		UCameraVariableCollection* InitialVariableCollection = nullptr;
		if (InitialVariable)
		{
			InitialVariableCollection = InitialVariable->GetTypedOuter<UCameraVariableCollection>();
		}

		AssetPickerConfig.bAllowDragging = false;
		AssetPickerConfig.bCanShowClasses = false;
		AssetPickerConfig.bAllowNullSelection = false;
		AssetPickerConfig.bShowBottomToolbar = true;
		AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
		AssetPickerConfig.SelectionMode = ESelectionMode::Single;
		AssetPickerConfig.Filter = ARFilter;
		AssetPickerConfig.SaveSettingsName = TEXT("CameraParameterVariablePropertyPicker");
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
		AssetPickerConfig.InitialAssetSelection = FAssetData(InitialVariableCollection);
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SCameraVariableBrowser::OnAssetSelected);
		AssetPickerConfig.GetCurrentSelectionDelegates.Add(&GetCurrentAssetPickerSelection);

		return ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig);
	}

	void OnAssetSelected(const FAssetData& SelectedAsset)
	{
		UpdateVariableListItemsSource();
	}

	void UpdateVariableListItemsSource()
	{
		TArray<FAssetData> SelectedAssets;
		if (GetCurrentAssetPickerSelection.IsBound())
		{
			SelectedAssets = GetCurrentAssetPickerSelection.Execute();
		}

		CameraVariableItemsSource.Reset();
		if (!SelectedAssets.IsEmpty())
		{
			if (UCameraVariableCollection* VariableCollection = Cast<UCameraVariableCollection>(SelectedAssets[0].GetAsset()))
			{
				CameraVariableItemsSource = VariableCollection->Variables.FilterByPredicate(
						[this](UCameraVariableAsset* Item) { return Item->GetClass() == VariableClass; });
			}
		}

		CameraVariableListView->RequestListRefresh();
	}

	TSharedRef<ITableRow> OnVariableListGenerateRow(UCameraVariableAsset* Item, const TSharedRef<STableViewBase>& OwnerTable)
	{
		TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasStyle = FGameplayCamerasEditorStyle::Get();

		const FText DisplayName = Item->DisplayName.IsEmpty() ?
			FText::FromName(Item->GetFName()) : FText::FromString(Item->DisplayName);

		return SNew(STableRow<UCameraVariableAsset*>, OwnerTable)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(GameplayCamerasStyle->GetBrush("CameraParameter.VariableBrowser"))
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(4.f, 2.f)
				[
					SNew(STextBlock)
					.Text(DisplayName)
				]
			];
	}

	void OnVariableListSelectionChanged(UCameraVariableAsset* Item, ESelectInfo::Type SelectInfo) const
	{
		if (!bSuppressVariableListSelectionChanged && Item)
		{
			RequestSetVariable.ExecuteIfBound(Item);
		}
		// else, the user should click the "Clear" button to actually remove the variable.
	}

private:

	TSharedPtr<SListView<UCameraVariableAsset*>> CameraVariableListView;
	TArray<UCameraVariableAsset*> CameraVariableItemsSource;

	UClass* VariableClass = nullptr;
	UCameraVariableAsset* InitialVariable = nullptr;

	FGetCurrentSelectionDelegate GetCurrentAssetPickerSelection;
	bool bSuppressVariableListSelectionChanged = false;

	FExecuteSetParameterVariable RequestSetVariable;
};

void FCameraParameterDetailsCustomization::Register(FPropertyEditorModule& PropertyEditorModule)
{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(\
			F##ValueName##CameraParameter::StaticStruct()->GetFName(),\
			FOnGetPropertyTypeCustomizationInstance::CreateLambda(\
				[]{ return MakeShared<F##ValueName##CameraParameterDetailsCustomization>(); }));
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
}

void FCameraParameterDetailsCustomization::Unregister(FPropertyEditorModule& PropertyEditorModule)
{
	if (UObjectInitialized())
	{
#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(\
				F##ValueName##CameraParameter::StaticStruct()->GetFName());
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE
	}
}

void FCameraParameterDetailsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	PropertyUtilities = CustomizationUtils.GetPropertyUtilities();

	StructProperty = PropertyHandle;
	StructProperty->SetOnPropertyResetToDefault(
			FSimpleDelegate::CreateSP(this, &FCameraParameterDetailsCustomization::OnResetToDefault));

	FName ValuePropertyName, VariablePropertyName;
	GetValueAndVariablePropertyNames(ValuePropertyName, VariablePropertyName);

	ValueProperty = PropertyHandle->GetChildHandle(ValuePropertyName);
	VariableProperty = PropertyHandle->GetChildHandle(VariablePropertyName);

	VariableClass = nullptr;
	if (FObjectProperty* VariableObjectProperty = CastField<FObjectProperty>(VariableProperty->GetProperty()))
	{
		VariableClass = VariableObjectProperty->PropertyClass;
	}

	UpdateVariableInfo();

	// The value widget is enabled (i.e. the user can change the value) if the parameter isn't driven by
	// a variable, or if that variable is a private variable meant to expose the parameter on the rig interface.
	const bool bShowVariableText = !VariableInfoText.IsEmpty() && !bIsExposedParameterVariable;
	const bool bShowVariableError = !VariableErrorText.IsEmpty() && !bIsExposedParameterVariable;
	const bool bEnableVariableWidget = bShowVariableText || bShowVariableError;

	TSharedRef<SWidget> ValueWidget = ValueProperty->CreatePropertyValueWidgetWithCustomization(nullptr);
	ValueWidget->SetEnabled(!bEnableVariableWidget);

	// TODO: change SStandaloneCustomizedValueWidget so that it can tell us about the layout requirements
	//		 of the value widget (min/max desired width/height, H/V alignment, etc.)
	const float MaxValueWidgetDesiredWidth = bEnableVariableWidget ? 300.f : 0.f;

	TSharedRef<FGameplayCamerasEditorStyle> GameplayCamerasStyle = FGameplayCamerasEditorStyle::Get();

	HeaderRow
	.NameContent()
	[
		StructProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MaxDesiredWidth(MaxValueWidgetDesiredWidth)
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(0)
		.FillWidth(1.f)
		[
			ValueWidget
		]
		+SHorizontalBox::Slot()
		.Padding(0)
		.AutoWidth()
		.HAlign(HAlign_Right)
		[
			ValueProperty->CreateDefaultPropertyButtonWidgets()
		]
	]
	.ExtensionContent()
	.HAlign(HAlign_Right)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(2.f)
		.AutoWidth()
		[
			SAssignNew(VariableBrowserButton, SComboButton)
			.HasDownArrow(true)
			.ContentPadding(1.f)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ToolTipText(LOCTEXT("SetVariable_ToolTip", "Selects a camera variable to drive this parameter"))
			.IsEnabled(VariableClass != nullptr && !bIsExposedParameterVariable)
			.ButtonContent()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(GameplayCamerasStyle->GetBrush("CameraParameter.VariableBrowser"))
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Visibility(bShowVariableText ? EVisibility::Visible : EVisibility::Collapsed)
					[
						SNew(STextBlock)
						.Text(VariableInfoText)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(2.f)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Visibility(bShowVariableError ? EVisibility::Visible : EVisibility::Collapsed)
					[
						SNew(STextBlock)
						.Text(VariableErrorText)
						.ColorAndOpacity(FStyleColors::Error)
					]
				]
			]
			.OnGetMenuContent(this, &FCameraParameterDetailsCustomization::BuildCameraVariableBrowser)
		]
	];
}

void FCameraParameterDetailsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

TSharedRef<SWidget> FCameraParameterDetailsCustomization::BuildCameraVariableBrowser()
{
	const bool bShouldCloseWindowAfterMenuSelection = true;
	const bool bCloseSelfOnly = true;
	const bool bSearchable = false;
	FMenuBuilder MenuBuilder(true, nullptr, nullptr, bCloseSelfOnly, &FCoreStyle::Get(), bSearchable);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CameraVariableOperations", "Current Parameter"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ClearVariable", "Clear"),
			LOCTEXT("ClearVariable_ToolTip", "Clears the variable from the camera parameter"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Delete"),
			FUIAction(
				FExecuteAction::CreateSP(this, &FCameraParameterDetailsCustomization::OnClearVariable),
				FCanExecuteAction::CreateSP(this, &FCameraParameterDetailsCustomization::CanClearVariable))
			);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CameraVariableBrowser", "Browse"));
	{
		TSharedRef<SWidget> VariableBrowser = SNew(SBox)
			.MinDesiredWidth(300.f)
			.MinDesiredHeight(300.f)
			[
				SNew(SCameraVariableBrowser)
				.VariableClass(VariableClass)
				.InitialVariable(CommonVariable)
				.RequestSetVariable(
						FExecuteSetParameterVariable::CreateSP(this, &FCameraParameterDetailsCustomization::OnSetVariable))
			];
		MenuBuilder.AddWidget(VariableBrowser, FText(), true, false);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void FCameraParameterDetailsCustomization::UpdateVariableInfo()
{
	CommonVariable = nullptr;
	VariableInfoText = FText::GetEmpty();
	VariableErrorText = FText::GetEmpty();
	bIsExposedParameterVariable = false;

	UObject* VariableObject = nullptr;
	FPropertyAccess::Result PropertyAccessResult = VariableProperty->GetValue(VariableObject);
	if (PropertyAccessResult == FPropertyAccess::Success)
	{
		if (VariableObject)
		{
			if (UCameraVariableAsset* Variable = Cast<UCameraVariableAsset>(VariableObject))
			{
				CommonVariable = Variable;
				VariableInfoText = Variable->DisplayName.IsEmpty() ?
					FText::FromName(Variable->GetFName()) :
					FText::FromString(Variable->DisplayName);
				bIsExposedParameterVariable = CommonVariable->bIsPrivate;
			}
			else
			{
				VariableErrorText = LOCTEXT("InvalidVariableObject", "Invalid Variable");
			}
		}
		// else: no variable set
	}
	else if (PropertyAccessResult == FPropertyAccess::MultipleValues)
	{
		VariableInfoText = LOCTEXT("MultipleVariableValues", "Multiple Variables");
	}
	else
	{
		VariableErrorText = LOCTEXT("ErrorReadingVariable", "Error Reading Variable");
	}
}

bool FCameraParameterDetailsCustomization::HasVariableInfoText() const
{
	return !VariableInfoText.IsEmpty() || !VariableErrorText.IsEmpty();
}

bool FCameraParameterDetailsCustomization::CanClearVariable() const
{
	return VariableProperty->CanResetToDefault();
}

void FCameraParameterDetailsCustomization::OnClearVariable()
{
	VariableProperty->ResetToDefault();
	PropertyUtilities->RequestForceRefresh();
}

void FCameraParameterDetailsCustomization::OnSetVariable(UCameraVariableAsset* InVariable)
{
	TArray<void*> RawData;
	StructProperty->AccessRawData(RawData);

	TArray<UObject*> OuterObjects;
	StructProperty->GetOuterObjects(OuterObjects);

	check(!OuterObjects.Num() || OuterObjects.Num() == RawData.Num());
	
	{
		FScopedTransaction Transaction(FText::Format(
					LOCTEXT("SetPropertyValue", "Set {0}"), StructProperty->GetPropertyDisplayName()));

		StructProperty->NotifyPreChange();

		for (int32 ValueIndex = 0; ValueIndex < RawData.Num(); ++ValueIndex)
		{
			SetParameterVariable(RawData[ValueIndex], InVariable);
		}

		StructProperty->NotifyPostChange(EPropertyChangeType::ValueSet);
	}

	FPropertyChangedEvent ChangeEvent(StructProperty->GetProperty(), EPropertyChangeType::ValueSet, OuterObjects);
	PropertyUtilities->NotifyFinishedChangingProperties(ChangeEvent);

	PropertyUtilities->RequestForceRefresh();
	VariableBrowserButton->SetIsOpen(false);
}

void FCameraParameterDetailsCustomization::OnResetToDefault()
{
	PropertyUtilities->RequestForceRefresh();
}

#define UE_CAMERA_VARIABLE_FOR_TYPE(ValueType, ValueName)\
void F##ValueName##CameraParameterDetailsCustomization::SetParameterVariable(void* InRawData, UCameraVariableAsset* InVariable)\
{\
	F##ValueName##CameraParameter* TypedData = reinterpret_cast<F##ValueName##CameraParameter*>(InRawData);\
	TypedData->Variable = CastChecked<U##ValueName##CameraVariable>(InVariable);\
}
UE_CAMERA_VARIABLE_FOR_ALL_TYPES()
#undef UE_CAMERA_VARIABLE_FOR_TYPE

}  // namespace UE::Cameras

#undef LOCTEXT_NAMESPACE
