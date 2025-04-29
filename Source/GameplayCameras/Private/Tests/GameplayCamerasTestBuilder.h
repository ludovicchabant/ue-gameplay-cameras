// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BlendCameraNode.h"
#include "Core/CameraAsset.h"
#include "Core/CameraDirector.h"
#include "Core/CameraEvaluationContext.h"
#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigTransition.h"
#include "Core/CameraSystemEvaluator.h"
#include "Core/CameraVariableReferences.h"
#include "Core/CameraVariableTableFwd.h"
#include "Directors/SingleCameraDirector.h"
#include "Nodes/Common/ArrayCameraNode.h"
#include "Templates/PointerIsConvertibleFromTo.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/Package.h"

#include <type_traits>

namespace UE::Cameras::Test
{

class FCameraEvaluationContextTestBuilder;

/**
 * Template mix-in for adding "go back to parent" support to a builder class.
 */
template<typename ParentType>
struct TScopedConstruction
{
	TScopedConstruction(ParentType& InParent)
		: Parent(InParent)
	{}

	/** Return the parent builder instance. */
	ParentType& Done() { return Parent; }

protected:

	ParentType& Parent;
};

/**
 * A generic utility class that defines a fluent interface for setting properties and adding items to
 * array properties on a given object.
 */
template<typename ObjectType>
struct TCameraObjectInitializer
{
	/** Sets a value on the given public property (via its member field). */
	template<typename PropertyType>
	TCameraObjectInitializer<ObjectType>& Set(PropertyType ObjectType::*Field, typename TCallTraits<PropertyType>::ParamType Value)
	{
		PropertyType& FieldPtr = (Object->*Field);
		FieldPtr = Value;
		return *this;
	}
	
	/** Adds an item to a given public array property (via its member field). */
	template<typename ItemType>
	TCameraObjectInitializer<ObjectType>& Add(TArray<ItemType> ObjectType::*Field, typename TCallTraits<ItemType>::ParamType NewItem)
	{
		TArray<ItemType>& ArrayPtr = (Object->*Field);
		ArrayPtr.Add(NewItem);
		return *this;
	}

protected:

	void SetObject(ObjectType* InObject)
	{
		Object = InObject;
	}

private:

	ObjectType* Object = nullptr;
};

/**
 * A simple repository matching UObject instances to names.
 */
class FNamedObjectRegistry : public TSharedFromThis<FNamedObjectRegistry>
{
public:

	/** Adds an object to the repository. */
	void Register(UObject* InObject, const FString& InName)
	{
		ensure(InObject && !InName.IsEmpty());
		NamedObjects.Add(InName, InObject);
	}

	/** Gets an object from the repository. */
	UObject* Get(const FString& InName) const
	{
		if (UObject* const* Found = NamedObjects.Find(InName))
		{
			return *Found;
		}
		return nullptr;
	}

	/** Gets an object from the repository with a call to CastChecked. */
	template<typename ObjectClass>
	ObjectClass* Get(const FString& InName) const
	{
		return CastChecked<ObjectClass>(Get(InName));
	}

private:

	TMap<FString, UObject*> NamedObjects;
};

/**
 * Interface for something that has access to a named object repository.
 */
struct IHasNamedObjectRegistry
{
	virtual ~IHasNamedObjectRegistry() {}

	virtual TSharedPtr<FNamedObjectRegistry> GetNamedObjectRegistry() = 0;
};

/**
 * A builder class for camera nodes.
 */
template<
	typename ParentType,
	typename NodeType,
	typename V = std::enable_if_t<TPointerIsConvertibleFromTo<NodeType, UCameraNode>::Value>
	>
class TCameraNodeTestBuilder 
	: public TScopedConstruction<ParentType>
	, public TCameraObjectInitializer<NodeType>
	, public IHasNamedObjectRegistry
{
public:

	using ThisType = TCameraNodeTestBuilder<ParentType, NodeType, V>;

	/** Creates a new instance of this builder class. */
	TCameraNodeTestBuilder(ParentType& InParent, UObject* Outer = nullptr)
		: TScopedConstruction<ParentType>(InParent)
	{
		if (Outer == nullptr)
		{
			Outer = GetTransientPackage();
		}
		CameraNode = NewObject<NodeType>(Outer);
		TCameraObjectInitializer<NodeType>::SetObject(CameraNode);
	}

	/** Gets the built camera node. */
	NodeType* Get() const { return CameraNode; }

	/** Pins the built camera node to a given pointer, for being able to later refer to it. */
	ThisType& Pin(NodeType*& OutPtr) { OutPtr = CameraNode; return *this; }

	/** Give a name to the built camera node, to be recalled later. */
	template<typename VV = std::enable_if_t<TPointerIsConvertibleFromTo<ParentType, IHasNamedObjectRegistry>::Value>>
	ThisType& Named(const TCHAR* InName)
	{
		TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry = GetNamedObjectRegistry();
		if (ensure(NamedObjectRegistry))
		{
			NamedObjectRegistry->Register(CameraNode, InName);
		}
		return *this;
	}

	/** Sets the value of a camera parameter field on the camera node. */
	template<typename ParameterType>
	ThisType& SetParameter(
			ParameterType NodeType::*ParameterField,
			typename TCallTraits<typename ParameterType::ValueType>::ParamType Value)
	{
		ParameterType& ParameterRef = (CameraNode->*ParameterField);
		ParameterRef.Value = Value;
		return *this;
	}

	/**
	 * Runs a custom setup callback on the camera node.
	 */
	ThisType& Setup(TFunction<void(NodeType*)> SetupCallback)
	{
		SetupCallback(CameraNode);
		return *this;
	}

	/**
	 * Runs a custom setup callback on the camera node with the named object registry provided.
	 */
	ThisType& Setup(TFunction<void(NodeType*, FNamedObjectRegistry*)> SetupCallback)
	{
		TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry = GetNamedObjectRegistry();
		SetupCallback(CameraNode, NamedObjectRegistry.Get());
		return *this;
	}

	/**
	 * Adds a child camera node via a public array member field on the camera node.
	 * Returns a builder for the child. You can go back to the current builder by
	 * calling Done() on the child builder.
	 */
	template<
		typename ChildNodeType, 
		typename ArrayItemType,
		typename = std::enable_if_t<TPointerIsConvertibleFromTo<ChildNodeType, ArrayItemType>::Value>
		>
	TCameraNodeTestBuilder<ThisType, ChildNodeType>
	AddChild(TArray<TObjectPtr<ArrayItemType>> NodeType::*ArrayField)
	{
		TCameraNodeTestBuilder<ThisType, ChildNodeType> ChildBuilder(*this, CameraNode->GetOuter());
		TArray<TObjectPtr<ArrayItemType>>& ArrayRef = (CameraNode->*ArrayField);
		ArrayRef.Add(ChildBuilder.Get());
		return ChildBuilder;
	}

	/**
	 * Convenience implementation of AddChild specifically for array nodes.
	 */
	template<
		typename ChildNodeType,
		typename = std::enable_if_t<
			TPointerIsConvertibleFromTo<NodeType, UArrayCameraNode>::Value &&
			TPointerIsConvertibleFromTo<ChildNodeType, UCameraNode>::Value>
		>
	TCameraNodeTestBuilder<ThisType, ChildNodeType>
	AddArrayChild()
	{
		TCameraNodeTestBuilder<ThisType, ChildNodeType> ChildBuilder(*this, CameraNode->GetOuter());
		CastChecked<UArrayCameraNode>(CameraNode)->Children.Add(ChildBuilder.Get());
		return ChildBuilder;
	}

	/** 
	 * Casting operator that returns a builder for the same camera node, but typed
	 * around a parent class of the camera node's class. Mostly useful for implicit casting
	 * when using AddChild().
	 */
	template<
		typename OtherNodeType,
		typename = std::enable_if_t<TPointerIsConvertibleFromTo<NodeType, OtherNodeType>::Value>
		>
	operator TCameraNodeTestBuilder<ParentType, OtherNodeType>() const
	{
		return TCameraNodeTestBuilder<ParentType, OtherNodeType>(
				EForceReuseCameraNode::Yes, 
				TScopedConstruction<ParentType>::Parent, 
				CameraNode);
	}

	/** Gets the named object registry from the parent. */
	virtual TSharedPtr<FNamedObjectRegistry> GetNamedObjectRegistry() override
	{
		if constexpr (TPointerIsConvertibleFromTo<ParentType, IHasNamedObjectRegistry>::Value)
		{
			return TScopedConstruction<ParentType>::Parent.GetNamedObjectRegistry();
		}
		else
		{
			return nullptr;
		}
	}

private:

	enum class EForceReuseCameraNode { Yes };

	TCameraNodeTestBuilder(EForceReuseCameraNode ForceReuse, ParentType& InParent, NodeType* ExistingCameraNode)
		: TScopedConstruction<ParentType>(InParent)
		, CameraNode(ExistingCameraNode)
	{
		TCameraObjectInitializer<NodeType>::SetObject(CameraNode);
	}

private:

	NodeType* CameraNode;
};

/**
 * Builder class for camera rig transitions.
 */
template<typename ParentType>
class TCameraRigTransitionTestBuilder 
	: public TScopedConstruction<ParentType>
	, public TCameraObjectInitializer<UCameraRigTransition>
	, public IHasNamedObjectRegistry
{
public:

	using ThisType = TCameraRigTransitionTestBuilder<ParentType>;

	/** Creates a new instance of this builder class. */
	TCameraRigTransitionTestBuilder(ParentType& InParent, UObject* Outer = nullptr)
		: TScopedConstruction<ParentType>(InParent)
	{
		if (Outer == nullptr)
		{
			Outer = GetTransientPackage();
		}

		Transition = NewObject<UCameraRigTransition>(Outer);
		TCameraObjectInitializer<UCameraRigTransition>::SetObject(Transition);
	}

	/** Gets the built transition object. */
	UCameraRigTransition* Get() const { return Transition; }

	/** Pins the built transition to a given pointer, for being able to later refer to it. */
	ThisType& Pin(UCameraRigTransition*& OutPtr) { OutPtr = Transition; return *this; }

	/** Give a name to the built transition, to be recalled later. */
	template<typename VV = std::enable_if_t<TPointerIsConvertibleFromTo<ParentType, IHasNamedObjectRegistry>::Value>>
	ThisType& Named(const TCHAR* InName)
	{
		TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry = GetNamedObjectRegistry();
		if (ensure(NamedObjectRegistry))
		{
			NamedObjectRegistry->Register(Transition, InName);
		}
		return *this;
	}

	/** 
	 * Creates a blend node of the given type, and returns a builder for it.
	 * You can go back to this transition builder by calling Done() on the blend builder.
	 */
	template<
		typename BlendType,
		typename = std::enable_if_t<TPointerIsConvertibleFromTo<BlendType, UBlendCameraNode>::Value>
		>
	TCameraNodeTestBuilder<ThisType, BlendType> MakeBlend()
	{
		TCameraNodeTestBuilder<ThisType, BlendType> BlendBuilder(*this, Transition->GetOuter());
		Transition->Blend = BlendBuilder.Get();
		return BlendBuilder;
	}

	/** Adds a transition condition. */
	template<
		typename ConditionType,
		typename = std::enable_if_t<TPointerIsConvertibleFromTo<ConditionType, UCameraRigTransitionCondition>::Value>
		>
	ThisType& AddCondition()
	{
		ConditionType* NewCondition = NewObject<ConditionType>(Transition->GetOuter());
		Transition->Conditions.Add(NewCondition);
		return *this;
	}

	/** Adds a transition condition. */
	template<
		typename ConditionType,
		typename = std::enable_if_t<TPointerIsConvertibleFromTo<ConditionType, UCameraRigTransitionCondition>::Value>
		>
	ThisType& AddCondition(TFunction<void(ConditionType*)> SetupCallback)
	{
		ConditionType* NewCondition = NewObject<ConditionType>(Transition->GetOuter());
		SetupCallback(NewCondition);
		Transition->Conditions.Add(NewCondition);
		return *this;
	}

	/** Gets the named object registry from the parent. */
	virtual TSharedPtr<FNamedObjectRegistry> GetNamedObjectRegistry() override
	{
		if constexpr (TPointerIsConvertibleFromTo<ParentType, IHasNamedObjectRegistry>::Value)
		{
			return TScopedConstruction<ParentType>::Parent.GetNamedObjectRegistry();
		}
		else
		{
			return nullptr;
		}
	}

private:

	UCameraRigTransition* Transition;
};

/**
 * The root builder class for building a camera rig. Follow the fluent interface to construct the
 * hierarchy of camera nodes, add transitions, etc.
 *
 * For instance:
 *
 *		UCameraRigAsset* CameraRig = FCameraRigAssetTestBuilder(TEXT("SimpleTest"))
 *			.MakeRootNode<UArrayCameraNode>()
 *				.AddChild<UOffsetCameraNode>(&UArrayCameraNode::Children)
 *					.SetParameter(&UOffsetCameraNode::TranslationOffset, FVector3d{ 1, 0, 0 })
 *					.Done()
 *				.AddChild<ULensParametersCameraNode>(&UArrayCameraNode::Children)
 *					.SetParameter(&ULensParametersCameraNode::FocalLenght, 18.f)
 *					.Done()
 *				.Done()
 *			.AddEnterTransition()
 *				.MakeBlend<USmoothBlendCameraNode>()
 *				.Done()
 *			.Get();
 */
template<typename ThisType>
class TCameraRigAssetTestBuilderBase 
	: public TCameraObjectInitializer<UCameraRigAsset>
	, public IHasNamedObjectRegistry
{
public:

	/** Gets the built camera rig. */
	UCameraRigAsset* Get() { return CameraRig; }

	/** Pins the built camera rig to a given pointer, for being able to later refer to it. */
	ThisType& Pin(UCameraRigAsset*& OutPtr)
	{
		OutPtr = CameraRig; 
		return *static_cast<ThisType*>(this);
	}

	/** Give a name to the built camera rig, to be recalled later. */
	ThisType& Named(const TCHAR* InName)
	{
		if (ensure(NamedObjectRegistry))
		{
			NamedObjectRegistry->Register(CameraRig, InName);
		}
		return *static_cast<ThisType*>(this);
	}

	/**
	 * Creates a new camera node and sets it as the root node of the rig.
	 * Returns the builder for the root camera node. You can come back to the rig builder
	 * by calling Done() on the node builder.
	 */
	template<typename NodeType>
	TCameraNodeTestBuilder<ThisType, NodeType> MakeRootNode()
	{
		ThisType* ActualThis = static_cast<ThisType*>(this);
		TCameraNodeTestBuilder<ThisType, NodeType> NodeBuilder(*ActualThis, CameraRig);
		CameraRig->RootNode = NodeBuilder.Get();
		return NodeBuilder;
	}

	/**
	 * A convenience method that calls MakeRootNode with a UArrayCameraNode.
	 */
	TCameraNodeTestBuilder<ThisType, UArrayCameraNode> MakeArrayRootNode()
	{
		return MakeRootNode<UArrayCameraNode>();
	}

	/**
	 * Adds a new enter transition and returns a builder for it. You can come back to the
	 * rig builder by calling Done() on the transition builder.
	 */
	TCameraRigTransitionTestBuilder<ThisType> AddEnterTransition()
	{
		TCameraRigTransitionTestBuilder<ThisType> TransitionBuilder(*this, CameraRig);
		CameraRig->EnterTransitions.Add(TransitionBuilder.Get());
		return TransitionBuilder;
	}

	/**
	 * Adds a new exit transition and returns a builder for it. You can come back to the
	 * rig builder by calling Done() on the transition builder.
	 */
	TCameraRigTransitionTestBuilder<ThisType> AddExitTransition()
	{
		TCameraRigTransitionTestBuilder<ThisType> TransitionBuilder(*this, CameraRig);
		CameraRig->ExitTransitions.Add(TransitionBuilder.Get());
		return TransitionBuilder;
	}

	/**
	 * Creates a new exposed rig parameter and hooks it up to the given camera node's property.
	 * When building the node hierarchy, you can use the Pin() method on the node builders to
	 * save a pointer to nodes you need for ExposeParameter().
	 *
	 * The created parameter is automatically stored in the named object registry under its name.
	 */
	ThisType& AddBlendableParameter(const FString& ParameterName, ECameraVariableType ParameterType, UCameraNode* Target, FName TargetPropertyName)
	{
		UCameraObjectInterfaceBlendableParameter* BlendableParameter = NewObject<UCameraObjectInterfaceBlendableParameter>(CameraRig);
		BlendableParameter->InterfaceParameterName = ParameterName;
		BlendableParameter->ParameterType = ParameterType;
		BlendableParameter->Target = Target;
		BlendableParameter->TargetPropertyName = TargetPropertyName;

		NamedObjectRegistry->Register(BlendableParameter, ParameterName);
		CameraRig->Interface.BlendableParameters.Add(BlendableParameter);
		return *static_cast<ThisType*>(this);
	}

	/**
	 * A variant of ExposeParameter that retrieves the target node from the named registry.
	 *
	 * The created parameter is automatically stored in the named object registry under its name.
	 */
	ThisType& AddBlendableParameter(const FString& ParameterName, ECameraVariableType ParameterType, const FString& TargetName, FName TargetPropertyName)
	{
		UCameraNode* Target = NamedObjectRegistry->Get<UCameraNode>(TargetName);
		ensure(Target);
		return AddBlendableParameter(ParameterName, ParameterType, Target, TargetPropertyName);
	}

	/** Gets the named object registry. */
	virtual TSharedPtr<FNamedObjectRegistry> GetNamedObjectRegistry() override
	{
		return NamedObjectRegistry;
	}

protected:

	TCameraRigAssetTestBuilderBase(TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name = NAME_None, UObject* Outer = nullptr)
	{
		Initialize(InNamedObjectRegistry, Name, Outer);
	}

private:

	void Initialize(TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name, UObject* Outer)
	{
		if (Outer == nullptr)
		{
			Outer = GetTransientPackage();
		}

		CameraRig = NewObject<UCameraRigAsset>(Outer, Name);
		TCameraObjectInitializer<UCameraRigAsset>::SetObject(CameraRig);

		NamedObjectRegistry = InNamedObjectRegistry;
		if (!NamedObjectRegistry)
		{
			NamedObjectRegistry = MakeShared<FNamedObjectRegistry>();
		}

		NamedObjectRegistry->Register(CameraRig, Name.ToString());
	}

private:

	UCameraRigAsset* CameraRig;

	TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry;
};

/**
 * Default version of the camera rig asset builder.
 */
class FCameraRigAssetTestBuilder 
	: public TCameraRigAssetTestBuilderBase<FCameraRigAssetTestBuilder>
{
public:

	FCameraRigAssetTestBuilder(FName Name = NAME_None, UObject* Outer = nullptr);
	FCameraRigAssetTestBuilder(TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name = NAME_None, UObject* Outer = nullptr);
};

/**
 * Version of the camera rig asset builder that has a scoped parent, with a Done() method exposed
 * to go back to it.
 */
template<typename ParentType>
class TScopedCameraRigAssetTestBuilder
	: public TScopedConstruction<ParentType>
	, public TCameraRigAssetTestBuilderBase<TScopedCameraRigAssetTestBuilder<ParentType>>
{
public:

	TScopedCameraRigAssetTestBuilder(ParentType& InParent, FName Name = NAME_None, UObject* Outer = nullptr)
		: TScopedConstruction<ParentType>(InParent)
		, TCameraRigAssetTestBuilderBase<TScopedCameraRigAssetTestBuilder<ParentType>>(Name, Outer)
	{
	}

	TScopedCameraRigAssetTestBuilder(ParentType& InParent, TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name = NAME_None, UObject* Outer = nullptr)
		: TScopedConstruction<ParentType>(InParent)
		, TCameraRigAssetTestBuilderBase<TScopedCameraRigAssetTestBuilder<ParentType>>(InNamedObjectRegistry, Name, Outer)
	{
	}
};

/**
 * Builder class for a camera director.
 */
template<
	typename ParentType,
	typename DirectorType,
	typename V = std::enable_if_t<TPointerIsConvertibleFromTo<DirectorType, UCameraDirector>::Value>
	>
class TCameraDirectorTestBuilder
	: public TScopedConstruction<ParentType>
	, public TCameraObjectInitializer<DirectorType>
	, public IHasNamedObjectRegistry
{
public:

	using ThisType = TCameraDirectorTestBuilder<ParentType, DirectorType, V>;

	/** Creates a new instance of this builder class. */
	TCameraDirectorTestBuilder(ParentType& InParent, UObject* Outer = nullptr)
		: TScopedConstruction<ParentType>(InParent)
	{
		if (Outer == nullptr)
		{
			Outer = GetTransientPackage();
		}
		CameraDirector = NewObject<DirectorType>(Outer);
		TCameraObjectInitializer<DirectorType>::SetObject(CameraDirector);
	}

	/** Gets the build camera director. */
	UCameraDirector* Get() const { return CameraDirector; }

	/** Pins the built camera director to a given pointer, for being able to later refer to it. */
	ThisType& Pin(DirectorType*& OutPtr) { OutPtr = CameraDirector; return *this; }

	/** Give a name to the built camera drector, to be recalled later. */
	template<typename VV = std::enable_if_t<TPointerIsConvertibleFromTo<ParentType, IHasNamedObjectRegistry>::Value>>
	ThisType& Named(const TCHAR* InName)
	{
		TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry = GetNamedObjectRegistry();
		if (ensure(NamedObjectRegistry))
		{
			NamedObjectRegistry->Register(CameraDirector, InName);
		}
		return *this;
	}

	/** Set a parameter on the camera director. */
	template<typename ParameterType>
	ThisType& SetParameter(
			ParameterType DirectorType::*ParameterField,
			typename TCallTraits<typename ParameterType::ValueType>::ParamType Value)
	{
		ParameterType& ParameterRef = (CameraDirector->*ParameterField);
		ParameterRef.Value = Value;
		return *this;
	}

	/** Runs arbitrary setup logic on the camera director. */
	ThisType& Setup(TFunction<void(DirectorType*)> SetupCallback)
	{
		SetupCallback(CameraDirector);
		return *this;
	}

	/** Runs arbitrary setup logic on the camera director. */
	ThisType& Setup(TFunction<void(DirectorType*, FNamedObjectRegistry*)> SetupCallback)
	{
		TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry = GetNamedObjectRegistry();
		SetupCallback(CameraDirector, NamedObjectRegistry.Get());
		return *this;
	}

	/** Gets the named object registry from the parent. */
	virtual TSharedPtr<FNamedObjectRegistry> GetNamedObjectRegistry() override
	{
		if constexpr (TPointerIsConvertibleFromTo<ParentType, IHasNamedObjectRegistry>::Value)
		{
			return TScopedConstruction<ParentType>::Parent.GetNamedObjectRegistry();
		}
		else
		{
			return nullptr;
		}
	}

private:

	DirectorType* CameraDirector;
};

/**
 * Builder class for a camera evaluation context and its camera asset.
 */
class FCameraEvaluationContextTestBuilder
	: public TCameraObjectInitializer<FCameraEvaluationContext>
	, public IHasNamedObjectRegistry
{
public:

	using ThisType = FCameraEvaluationContextTestBuilder;

	/** Creates a new instance of this builder class. */
	FCameraEvaluationContextTestBuilder(UObject* Owner = nullptr);

	/** Gets the created evaluation context. */
	TSharedRef<FCameraEvaluationContext> Get() const { return EvaluationContext.ToSharedRef(); }

	/** Pins the created camera asset. */
	ThisType& PinCameraAsset(UCameraAsset*& OutPtr) { OutPtr = CameraAsset; return *this; }

	/** Builds the camera asset. */
	ThisType& BuildCameraAsset() { CameraAsset->BuildCamera(); return *this; }

	/** Builds a new camera director of the given type and returns a builder object for its. */
	template<typename DirectorType>
	TCameraDirectorTestBuilder<ThisType, DirectorType> MakeDirector()
	{
		TCameraDirectorTestBuilder<ThisType, DirectorType> DirectorBuilder(*this, EvaluationContext->GetOwner());
		CameraAsset->SetCameraDirector(DirectorBuilder.Get());
		return DirectorBuilder;
	}

	/** Builds a new single camera director and returns a builder object for its. */
	TCameraDirectorTestBuilder<ThisType, USingleCameraDirector> MakeSingleDirector()
	{
		return MakeDirector<USingleCameraDirector>();
	}

	/** Creates a new camera rig asset builder and adds its camera rig to the camera asset.*/
	TScopedCameraRigAssetTestBuilder<ThisType> AddCameraRig(FName Name = NAME_None)
	{
		TScopedCameraRigAssetTestBuilder<ThisType> CameraRigBuilder(*this, GetNamedObjectRegistry(), Name, CameraAsset);
		return CameraRigBuilder;
	}

	/** Gets the named object registry. */
	virtual TSharedPtr<FNamedObjectRegistry> GetNamedObjectRegistry() override
	{
		return NamedObjectRegistry;
	}

private:

	UCameraAsset* CameraAsset;

	TSharedPtr<FCameraEvaluationContext> EvaluationContext;

	TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry;
};

/**
 * Builder class for a camera system evaluator.
 */
class FCameraSystemEvaluatorBuilder
{
public:

	/** Makes a new camera system evaluator. */
	static TSharedRef<FCameraSystemEvaluator> Build(UObject* OwnerObject = nullptr)
	{
		TSharedRef<FCameraSystemEvaluator> NewEvaluator = MakeShared<FCameraSystemEvaluator>();
		NewEvaluator->Initialize(OwnerObject);
		return NewEvaluator;
	}
};

}  // namespace UE::Cameras::Test

