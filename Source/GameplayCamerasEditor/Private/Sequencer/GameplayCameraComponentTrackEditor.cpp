// Copyright Epic Games, Inc. All Rights Reserved.

#include "Sequencer/GameplayCameraComponentTrackEditor.h"

#include "Core/CameraAsset.h"
#include "Core/CameraRigParameterDefinition.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Styles/GameplayCamerasEditorStyle.h"
#include "Tracks/MovieSceneActorReferenceTrack.h"
#include "Tracks/MovieSceneBoolTrack.h"
#include "Tracks/MovieSceneDoubleTrack.h"
#include "Tracks/MovieSceneEnumTrack.h"
#include "Tracks/MovieSceneFloatTrack.h"
#include "Tracks/MovieSceneIntegerTrack.h"
#include "Tracks/MovieSceneObjectPropertyTrack.h"
#include "Tracks/MovieSceneRotatorTrack.h"
#include "Tracks/MovieSceneStringTrack.h"
#include "Tracks/MovieSceneTransformTrack.h"
#include "Tracks/MovieSceneVectorTrack.h"

#define LOCTEXT_NAMESPACE "GameplayCameraComponentTrackEditor"

TSharedRef<ISequencerTrackEditor> FGameplayCameraComponentTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer)
{
	return MakeShareable(new FGameplayCameraComponentTrackEditor(OwningSequencer));
}

FGameplayCameraComponentTrackEditor::FGameplayCameraComponentTrackEditor(TSharedRef<ISequencer> InSequencer)
	: FMovieSceneTrackEditor(InSequencer) 
{
}

void FGameplayCameraComponentTrackEditor::BindCommands(TSharedRef<FUICommandList> SequencerCommandBindings)
{
}

void FGameplayCameraComponentTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
}

void FGameplayCameraComponentTrackEditor::BuildTrackContextMenu(FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track)
{
}

void FGameplayCameraComponentTrackEditor::ExtendObjectBindingTrackMenu(TSharedRef<FExtender> Extender, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass)
{
	if (ObjectClass && ObjectClass->IsChildOf<UGameplayCameraComponent>())
	{
		Extender->AddMenuExtension(
				TEXT("Tracks"), EExtensionHook::After, nullptr, 
				FMenuExtensionDelegate::CreateSP(this, &FGameplayCameraComponentTrackEditor::OnExtendObjectBindingTrackMenu, ObjectBindings));
	}
}

void FGameplayCameraComponentTrackEditor::OnExtendObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings)
{
	TMap<FName, FCameraRigParameterDefinition> DefinitionsByName;
	TMap<FName, int32> NumEqualDefinitions;

	for (const FGuid& ObjectBinding : ObjectBindings)
	{
		UGameplayCameraComponent* CameraComponent = GetCameraComponentForBinding(ObjectBinding);
		if (CameraComponent && CameraComponent->CameraReference.GetCameraAsset())
		{
			const UCameraAsset* CameraAsset = CameraComponent->CameraReference.GetCameraAsset();
			for (const FCameraRigParameterDefinition& Definition : CameraAsset->GetParameterDefinitions())
			{
				if (FCameraRigParameterDefinition* ExistingDefinition = DefinitionsByName.Find(Definition.ParameterName))
				{
					if (Definition == *ExistingDefinition)
					{
						++NumEqualDefinitions[Definition.ParameterName];
					}
				}
				else
				{
					DefinitionsByName.Add(Definition.ParameterName, Definition);
					NumEqualDefinitions.Add(Definition.ParameterName, 1);
				}
			}
		}
	}

	const int32 NumObjectBindings = ObjectBindings.Num();
	TArray<FCameraRigParameterDefinition> DefinitionsToAdd;

	for (auto It = DefinitionsByName.CreateConstIterator(); It; ++It)
	{
		if (NumEqualDefinitions.FindRef(It.Key()) == NumObjectBindings)
		{
			DefinitionsToAdd.Add(It.Value());
		}
	}

	DefinitionsToAdd.StableSort([](const FCameraRigParameterDefinition& A, const FCameraRigParameterDefinition& B)
			{
				return A.ParameterName.Compare(B.ParameterName) < 0;
			});

	if (DefinitionsToAdd.Num() > 0)
	{
		MenuBuilder.BeginSection(TEXT("CameraParameters"), LOCTEXT("AddCameraParametersMenuSection", "Camera Parameters"));

		for (const FCameraRigParameterDefinition& Definition : DefinitionsToAdd)
		{
			MenuBuilder.AddMenuEntry(
					FText::FromName(Definition.ParameterName),
					LOCTEXT("AddCameraParameterMenuToolTip", "Adds a track for controlling this camera parameter."),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateSP(this, &FGameplayCameraComponentTrackEditor::AddCameraParameterTrack, Definition, ObjectBindings),
						FCanExecuteAction::CreateSP(this, &FGameplayCameraComponentTrackEditor::CanAddCameraParameterTrack, Definition, ObjectBindings[0]))
					);
		}

		MenuBuilder.EndSection();
	}
}

TSharedPtr<SWidget> FGameplayCameraComponentTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params)
{
	return FMovieSceneTrackEditor::BuildOutlinerEditWidget(ObjectBinding, Track, Params);
}

TSharedPtr<SWidget> FGameplayCameraComponentTrackEditor::BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& Params, const FName& ColumnName)
{
	return FMovieSceneTrackEditor::BuildOutlinerColumnWidget(Params, ColumnName);
}

TSharedRef<ISequencerSection> FGameplayCameraComponentTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding)
{
	return FMovieSceneTrackEditor::MakeSectionInterface(SectionObject, Track, ObjectBinding);
}

void FGameplayCameraComponentTrackEditor::OnRelease()
{
}

bool FGameplayCameraComponentTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
	// This track editor doesn't support any track, it just extends object bindings.
	return false;
}

void FGameplayCameraComponentTrackEditor::Tick(float DeltaTime)
{
}

const FSlateBrush* FGameplayCameraComponentTrackEditor::GetIconBrush() const
{
	using namespace UE::Cameras;
	TSharedRef<FGameplayCamerasEditorStyle> CamerasEditorStyle = FGameplayCamerasEditorStyle::Get();
	return CamerasEditorStyle->GetBrush("Sequencer.CameraRigTrack");
}

bool FGameplayCameraComponentTrackEditor::OnAllowDrop(const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams)
{
	return false;
}

FReply FGameplayCameraComponentTrackEditor::OnDrop(const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams)
{
	return FReply::Unhandled();
}

UGameplayCameraComponent* FGameplayCameraComponentTrackEditor::GetCameraComponentForBinding(const FGuid& ObjectBinding) const
{
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
	if (SequencerPtr.IsValid())
	{
		UObject* BoundObject = SequencerPtr->FindSpawnedObjectOrTemplate(ObjectBinding);
		return Cast<UGameplayCameraComponent>(BoundObject);
	}
	return nullptr;
}

void FGameplayCameraComponentTrackEditor::AddCameraParameterTrack(FCameraRigParameterDefinition Definition, TArray<FGuid> ObjectBindings)
{
	TSubclassOf<UMovieSceneTrack> ParameterTrackType = GetParameterTrackFromDefinition(Definition);
	if (ParameterTrackType)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddCameraParameterTrack", "Add camera parameter track"));

		for (FGuid ObjectBinding : ObjectBindings)
		{
			FFindOrCreateTrackResult Result = FindOrCreateTrackForObject(ObjectBinding, ParameterTrackType, Definition.ParameterName, true);
			if (Result.bWasCreated)
			{
				UMovieScenePropertyTrack* NewTrack = CastChecked<UMovieScenePropertyTrack>(Result.Track);
				NewTrack->SetPropertyNameAndPath(Definition.ParameterName, Definition.ParameterName.ToString());
				InitializeNewTrack(NewTrack, Definition);
			}
		}

		GetSequencer()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemAdded);
	}
}

bool FGameplayCameraComponentTrackEditor::CanAddCameraParameterTrack(FCameraRigParameterDefinition Definition, FGuid ObjectBinding) const
{
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
	const UMovieScene* FocusedMovieScene = SequencerPtr->GetFocusedMovieSceneSequence()->GetMovieScene();

	TSubclassOf<UMovieSceneTrack> ParameterTrackType = GetParameterTrackFromDefinition(Definition);
	if (!ParameterTrackType)
	{
		return false;
	}
	
	UMovieSceneTrack* ExistingParameterTrack = FocusedMovieScene->FindTrack(ParameterTrackType, ObjectBinding, Definition.ParameterName);
	return ExistingParameterTrack == nullptr;
}

void FGameplayCameraComponentTrackEditor::InitializeNewTrack(UMovieScenePropertyTrack* NewTrack, const FCameraRigParameterDefinition& Definition) const
{
	if (Definition.ParameterType == ECameraRigInterfaceParameterType::Blendable)
	{
		switch (Definition.VariableType)
		{
			case ECameraVariableType::Vector2f:
				CastChecked<UMovieSceneFloatVectorTrack>(NewTrack)->SetNumChannelsUsed(2);
				break;
			case ECameraVariableType::Vector3f:
				CastChecked<UMovieSceneFloatVectorTrack>(NewTrack)->SetNumChannelsUsed(3);
				break;
			case ECameraVariableType::Vector4f:
				CastChecked<UMovieSceneFloatVectorTrack>(NewTrack)->SetNumChannelsUsed(4);
				break;
			case ECameraVariableType::Vector2d:
				CastChecked<UMovieSceneDoubleVectorTrack>(NewTrack)->SetNumChannelsUsed(2);
				break;
			case ECameraVariableType::Vector3d:
				CastChecked<UMovieSceneDoubleVectorTrack>(NewTrack)->SetNumChannelsUsed(3);
				break;
			case ECameraVariableType::Vector4d:
				CastChecked<UMovieSceneDoubleVectorTrack>(NewTrack)->SetNumChannelsUsed(4);
				break;
			default:
				break;
		}
	}
	else if (Definition.ParameterType == ECameraRigInterfaceParameterType::Data)
	{
		switch (Definition.DataType)
		{
			case ECameraContextDataType::Enum:
				{
					const UEnum* Enum = Cast<const UEnum>(Definition.DataTypeObject);
					CastChecked<UMovieSceneEnumTrack>(NewTrack)->SetEnum(const_cast<UEnum*>(Enum));
				}
				break;
			case ECameraContextDataType::Object:
				{
					const UClass* ObjectClass = Cast<const UClass>(Definition.DataTypeObject);
					if (!ObjectClass || !ObjectClass->IsChildOf<AActor>())
					{
						CastChecked<UMovieSceneObjectPropertyTrack>(NewTrack)->PropertyClass = const_cast<UClass*>(ObjectClass);
					}
				}
				break;
			default:
				break;
		}
	}
}

TSubclassOf<UMovieScenePropertyTrack> FGameplayCameraComponentTrackEditor::GetParameterTrackFromDefinition(const FCameraRigParameterDefinition& Definition) const
{
	if (Definition.ParameterType == ECameraRigInterfaceParameterType::Blendable)
	{
		switch (Definition.VariableType)
		{
			case ECameraVariableType::Boolean:
				return UMovieSceneBoolTrack::StaticClass();
			case ECameraVariableType::Integer32:
				return UMovieSceneIntegerTrack::StaticClass();
			case ECameraVariableType::Float:
				return UMovieSceneFloatTrack::StaticClass();
			case ECameraVariableType::Double:
				return UMovieSceneDoubleTrack::StaticClass();
			case ECameraVariableType::Vector2f:
			case ECameraVariableType::Vector3f:
			case ECameraVariableType::Vector4f:
				return UMovieSceneFloatVectorTrack::StaticClass();
			case ECameraVariableType::Vector2d:
			case ECameraVariableType::Vector3d:
			case ECameraVariableType::Vector4d:
				return UMovieSceneDoubleVectorTrack::StaticClass();
			case ECameraVariableType::Rotator3f:
			case ECameraVariableType::Rotator3d:
				return UMovieSceneRotatorTrack::StaticClass();
			case ECameraVariableType::Transform3f:
			case ECameraVariableType::Transform3d:
				return UMovieSceneTransformTrack::StaticClass();
			default:
				break;
		}
	}
	else if (Definition.ParameterType == ECameraRigInterfaceParameterType::Data)
	{
		// TODO
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE

