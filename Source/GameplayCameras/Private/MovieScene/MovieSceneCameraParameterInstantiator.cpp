// Copyright Epic Games, Inc. All Rights Reserved.

#include "MovieScene/MovieSceneCameraParameterInstantiator.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,6,0)

#include "Core/CameraAsset.h"
#include "Core/CameraAssetReference.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigAssetReference.h"
#include "Core/CameraObjectInterfaceParameterDefinition.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/MovieSceneEntityManager.h"
#include "EntitySystem/MovieSceneEntitySystemTask.h"
#include "EntitySystem/MovieScenePropertyBinding.h"
#include "EntitySystem/MovieSceneSequenceInstanceHandle.h"
#include "Evaluation/PreAnimatedState/MovieScenePreAnimatedStateStorage.h"
#include "Evaluation/PreAnimatedState/MovieScenePreAnimatedStorageID.inl"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/GameplayCameraRigComponent.h"
#include "MovieScene/MovieSceneGameplayCamerasComponentTypes.h"
#include "Tracks/MovieScenePropertyTrack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MovieSceneCameraParameterInstantiator)

namespace UE::Cameras
{

struct FPreAnimatedCameraParameterStateTraits : UE::MovieScene::FPreAnimatedStateTraits
{
	using KeyType = TTuple<FObjectKey, FGuid>;
	using StorageType = bool;

	bool CachePreAnimatedValue(UGameplayCameraComponentBase* CameraComponentBase, const FGuid& ParameterGuid)
	{
		if (ParameterGuid.IsValid())
		{
			if (UGameplayCameraComponent* CameraComponent = Cast<UGameplayCameraComponent>(CameraComponentBase))
			{
				return CameraComponent->CameraReference.IsParameterAnimated(ParameterGuid);
			}
			else if (UGameplayCameraRigComponent* CameraRigComponent = Cast<UGameplayCameraRigComponent>(CameraComponentBase))
			{
				return CameraRigComponent->CameraRigReference.IsParameterAnimated(ParameterGuid);
			}
		}
		return false;
	}

	void RestorePreAnimatedValue(const KeyType& InKey, bool bWasAnimated, const UE::MovieScene::FRestoreStateParams& Params)
	{
		UGameplayCameraComponentBase* CameraComponentBase = Cast<UGameplayCameraComponentBase>(InKey.Get<0>().ResolveObjectPtr());
		const FGuid& ParameterGuid = InKey.Get<1>();
		if (ParameterGuid.IsValid())
		{
			if (UGameplayCameraComponent* CameraComponent = Cast<UGameplayCameraComponent>(CameraComponentBase))
			{
				CameraComponent->CameraReference.SetParameterAnimated(ParameterGuid, bWasAnimated);
			}
			else if (UGameplayCameraRigComponent* CameraRigComponent = Cast<UGameplayCameraRigComponent>(CameraComponentBase))
			{
				CameraRigComponent->CameraRigReference.SetParameterAnimated(ParameterGuid, bWasAnimated);
			}
		}
	}
};

struct FPreAnimatedCameraParameterStorage : UE::MovieScene::TPreAnimatedStateStorage<FPreAnimatedCameraParameterStateTraits>
{
	static UE::MovieScene::TAutoRegisterPreAnimatedStorageID<FPreAnimatedCameraParameterStorage> StorageID;
};

UE::MovieScene::TAutoRegisterPreAnimatedStorageID<FPreAnimatedCameraParameterStorage> FPreAnimatedCameraParameterStorage::StorageID;

struct FSetupCameraParameterOverrideTask
{
	TSharedPtr<FPreAnimatedCameraParameterStorage> PreAnimatedStorage;

	FSetupCameraParameterOverrideTask(TSharedPtr<FPreAnimatedCameraParameterStorage> InPreAnimatedStorage)
		: PreAnimatedStorage(InPreAnimatedStorage)
	{}

	void ForEachEntity(
			UE::MovieScene::FMovieSceneEntityID EntityID, 
			UE::MovieScene::FRootInstanceHandle RootInstanceHandle,
			UObject* BoundObject, 
			const FMovieScenePropertyBinding& PropertyBinding, 
			FGuid& CameraParameterOverrideID)
	{
		using namespace UE::MovieScene;

		UGameplayCameraComponentBase* CameraComponentBase = Cast<UGameplayCameraComponentBase>(BoundObject);
		if (!ensure(CameraComponentBase))
		{
			return;
		}

		FGuid ParameterGuid;
		bool bWasAnimated = false;

		if (UGameplayCameraComponent* CameraComponent = Cast<UGameplayCameraComponent>(CameraComponentBase))
		{
			ParameterGuid = GetParameterGuid(CameraComponent->CameraReference, PropertyBinding);
			bWasAnimated = CameraComponent->CameraReference.IsParameterAnimated(ParameterGuid);
		}
		else if (UGameplayCameraRigComponent* CameraRigComponent = Cast<UGameplayCameraRigComponent>(CameraComponentBase))
		{
			ParameterGuid = GetParameterGuid(CameraRigComponent->CameraRigReference, PropertyBinding);
			bWasAnimated = CameraRigComponent->CameraRigReference.IsParameterAnimated(ParameterGuid);
		}

		if (!ParameterGuid.IsValid())
		{
			return;
		}

		using PreAnimatedKeyType = TTuple<FObjectKey, FGuid>;

		PreAnimatedStorage->BeginTrackingEntity(EntityID, true, RootInstanceHandle, CameraComponentBase, ParameterGuid);
		PreAnimatedStorage->CachePreAnimatedValue(
				PreAnimatedKeyType(CameraComponentBase, ParameterGuid),
				[bWasAnimated](const PreAnimatedKeyType& InKey) { return bWasAnimated; });

		CameraParameterOverrideID = ParameterGuid;

		if (UGameplayCameraComponent* CameraComponent = Cast<UGameplayCameraComponent>(CameraComponentBase))
		{
			CameraComponent->CameraReference.SetParameterAnimated(ParameterGuid, true);
		}
		else if (UGameplayCameraRigComponent* CameraRigComponent = Cast<UGameplayCameraRigComponent>(CameraComponentBase))
		{
			CameraRigComponent->CameraRigReference.SetParameterAnimated(ParameterGuid, true);
		}
	}

	FGuid GetParameterGuid(const FCameraAssetReference& CameraReference, const FMovieScenePropertyBinding& PropertyBinding)
	{
		const UCameraAsset* CameraAsset = CameraReference.GetCameraAsset();
		if (ensure(CameraAsset))
		{
			return GetParameterGuid(CameraAsset->GetParameterDefinitions(), TEXT("CameraReference.Parameters.Value"), PropertyBinding);
		}
		return FGuid();
	}

	FGuid GetParameterGuid(const FCameraRigAssetReference& CameraRigReference, const FMovieScenePropertyBinding& PropertyBinding)
	{
		const UCameraRigAsset* CameraRigAsset = CameraRigReference.GetCameraRig();
		if (ensure(CameraRigAsset))
		{
			return GetParameterGuid(CameraRigAsset->GetParameterDefinitions(), TEXT("CameraRigReference.Parameters.Value"), PropertyBinding);
		}
		return FGuid();
	}

	FGuid GetParameterGuid(TConstArrayView<FCameraObjectInterfaceParameterDefinition> ParameterDefinitions, const FString& PropertyPathStartsWith, const FMovieScenePropertyBinding& PropertyBinding)
	{
		// This isn't ideal but we know all camera parameters are bound to the "Parameters" property bag
		// of the camera component, so use that to figure out the parameter name and find is ID.
		const FString PropertyPath = PropertyBinding.PropertyPath.ToString();
		if (!ensure(PropertyPath.StartsWith(PropertyPathStartsWith)))
		{
			return FGuid();
		}
		
		TArray<FString> PropertyNames;
		PropertyPath.ParseIntoArray(PropertyNames, TEXT("."), true);
		if (!ensure(PropertyNames.Num() >= 4))
		{
			return FGuid();
		}

		FString& ParameterName = PropertyNames[3];
		
		// Strip out the array element index from the name, if we are animating something inside an
		// array data parameter.
		if (ParameterName.Len() > 0 && ParameterName.GetCharArray()[ParameterName.Len() - 1] == ']')
		{
			int32 OpenIndex = 0;
			if (ParameterName.FindLastChar('[', OpenIndex))
			{
				ParameterName = FString::ConstructFromPtrSize(*ParameterName, OpenIndex);
			}
		}

		for (const FCameraObjectInterfaceParameterDefinition& ParameterDefintion : ParameterDefinitions)
		{
			if (ParameterDefintion.ParameterName == ParameterName)
			{
				return ParameterDefintion.ParameterGuid;
			}
		}

		return FGuid();
	}
};

}  // namespace UE::Cameras

void UMovieSceneCameraParameterDecoration::ExtendEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity)
{
	using namespace UE::Cameras;
	using namespace UE::MovieScene;

	FMovieSceneGameplayCamerasComponentTypes* CameraComponents = FMovieSceneGameplayCamerasComponentTypes::Get();

	OutImportedEntity->AddBuilder(
			FEntityBuilder()
			.Add(CameraComponents->CameraParameterOverrideID, FGuid()));
}

UMovieSceneCameraParameterInstantiator::UMovieSceneCameraParameterInstantiator(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	using namespace UE::Cameras;
	using namespace UE::MovieScene;

	FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();
	FMovieSceneGameplayCamerasComponentTypes* CameraComponents = FMovieSceneGameplayCamerasComponentTypes::Get();
	RelevantComponent = CameraComponents->CameraParameterOverrideID;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		DefineComponentConsumer(GetClass(), BuiltInComponents->BoundObject);
	}
}

#if WITH_EDITORONLY_DATA

void UMovieSceneCameraParameterInstantiator::OnMovieSceneSectionAddedToTrack(UMovieSceneTrack* Track, UMovieSceneSection* NewSection)
{
	UMovieScenePropertyTrack* PropertyTrack = Cast<UMovieScenePropertyTrack>(Track);
	if (!PropertyTrack)
	{
		return;
	}

	// This isn't ideal but we know all camera parameters are bound to the "Parameters" property bag
	// of the camera component, so use that to determine if this is a camera parameter track.
	const FMovieScenePropertyBinding& PropertyBinding = PropertyTrack->GetPropertyBinding();
	const FString& PropertyPath = PropertyBinding.PropertyPath.ToString();
	if (PropertyPath.StartsWith(TEXT("CameraReference.Parameters.Value")))
	{
		UMovieSceneCameraParameterDecoration* Decoration = NewSection->GetOrCreateDecoration<UMovieSceneCameraParameterDecoration>();
	}
}

#endif

void UMovieSceneCameraParameterInstantiator::OnLink()
{
	using namespace UE::Cameras;

	PreAnimatedStorage = Linker->PreAnimatedState.GetOrCreateStorage<FPreAnimatedCameraParameterStorage>();
}

void UMovieSceneCameraParameterInstantiator::OnRun(FSystemTaskPrerequisites& InPrerequisites, FSystemSubsequentTasks& Subsequents)
{
	using namespace UE::Cameras;
	using namespace UE::MovieScene;

	FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();
	FMovieSceneGameplayCamerasComponentTypes* CameraComponents = FMovieSceneGameplayCamerasComponentTypes::Get();

	FEntityTaskBuilder()
	.ReadEntityIDs()
	.Read(BuiltInComponents->RootInstanceHandle)
	.Read(BuiltInComponents->BoundObject)
	.Read(BuiltInComponents->PropertyBinding)
	.Write(CameraComponents->CameraParameterOverrideID)
	.FilterAny({ BuiltInComponents->Tags.NeedsLink })
	.FilterNone({ BuiltInComponents->Tags.NeedsUnlink, BuiltInComponents->Tags.Ignored })
	.RunInline_PerEntity(&Linker->EntityManager, FSetupCameraParameterOverrideTask(PreAnimatedStorage));
}

#endif  // 5.6.0

