// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"

class ISequencer;
class UGameplayCameraComponent;
class UMovieScenePropertyTrack;
struct FCameraRigParameterDefinition;

class FGameplayCameraComponentTrackEditor : public FMovieSceneTrackEditor
{
public:

	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer);

	FGameplayCameraComponentTrackEditor(TSharedRef<ISequencer> InSequencer);

protected:

	// ISequencerTrackEditor interface.
	virtual void BindCommands(TSharedRef<FUICommandList> SequencerCommandBindings) override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	virtual void BuildTrackContextMenu(FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track) override;
	virtual void ExtendObjectBindingTrackMenu(TSharedRef<FExtender> Extender, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass) override;
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
	virtual TSharedPtr<SWidget> BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& Params, const FName& ColumnName) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
	virtual void OnRelease() override;
	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;
	virtual void Tick(float DeltaTime) override;
	virtual const FSlateBrush* GetIconBrush() const override;
	virtual bool OnAllowDrop(const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams) override;
	virtual FReply OnDrop(const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams) override;

private:

	UGameplayCameraComponent* GetCameraComponentForBinding(const FGuid& ObjectBinding) const;

	void OnExtendObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings);

	void AddCameraParameterTrack(FCameraRigParameterDefinition Definition, TArray<FGuid> ObjectBindings);
	bool CanAddCameraParameterTrack(FCameraRigParameterDefinition Definition, FGuid ObjectBinding) const;

	void InitializeNewTrack(UMovieScenePropertyTrack* NewTrack, const FCameraRigParameterDefinition& Definition) const;

	TSubclassOf<UMovieScenePropertyTrack> GetParameterTrackFromDefinition(const FCameraRigParameterDefinition& Definition) const;
};

