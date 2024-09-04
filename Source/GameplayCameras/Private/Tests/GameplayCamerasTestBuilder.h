// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/BlendCameraNode.h"
#include "Core/CameraNode.h"
#include "Core/CameraRigAsset.h"
#include "Core/CameraRigTransition.h"
#include "Nodes/Common/ArrayCameraNode.h"
#include "Templates/PointerIsConvertibleFromTo.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/Package.h"

#include <type_traits>

namespace UE::Cameras::Test
{

class FCameraRigAssetTestBuilder;

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
class FNamedObjectRegistry
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

	virtual FNamedObjectRegistry* GetNamedObjectRegistry() = 0;
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
		FNamedObjectRegistry* NamedObjectRegistry = GetNamedObjectRegistry();
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
		FNamedObjectRegistry* NamedObjectRegistry = GetNamedObjectRegistry();
		SetupCallback(CameraNode, NamedObjectRegistry);
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
	virtual FNamedObjectRegistry* GetNamedObjectRegistry() override
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
		FNamedObjectRegistry* NamedObjectRegistry = GetNamedObjectRegistry();
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
	virtual FNamedObjectRegistry* GetNamedObjectRegistry() override
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
class FCameraRigAssetTestBuilder 
	: public TCameraObjectInitializer<UCameraRigAsset>
	, public IHasNamedObjectRegistry
{
public:

	using ThisType = FCameraRigAssetTestBuilder;

	FCameraRigAssetTestBuilder(FName Name = NAME_None, UObject* Outer = nullptr);
	FCameraRigAssetTestBuilder(TSharedRef<FNamedObjectRegistry> InNamedObjectRegistry, FName Name = NAME_None, UObject* Outer = nullptr);

	/** Gets the built camera rig. */
	UCameraRigAsset* Get() { return CameraRig; }

	/**
	 * Creates a new camera node and sets it as the root node of the rig.
	 * Returns the builder for the root camera node. You can come back to the rig builder
	 * by calling Done() on the node builder.
	 */
	template<typename NodeType>
	TCameraNodeTestBuilder<ThisType, NodeType> MakeRootNode()
	{
		TCameraNodeTestBuilder<ThisType, NodeType> NodeBuilder(*this, CameraRig);
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
	TCameraRigTransitionTestBuilder<ThisType> AddEnterTransition();
	/**
	 * Adds a new exit transition and returns a builder for it. You can come back to the
	 * rig builder by calling Done() on the transition builder.
	 */
	TCameraRigTransitionTestBuilder<ThisType> AddExitTransition();

	/**
	 * Creates a new exposed rig parameter and hooks it up to the given camera node's property.
	 * When building the node hierarchy, you can use the Pin() method on the node builders to
	 * save a pointer to nodes you need for ExposeParameter().
	 *
	 * The created parameter is automatically stored in the named object registry under its name.
	 */
	FCameraRigAssetTestBuilder& ExposeParameter(const FString& ParameterName, UCameraNode* Target, FName TargetPropertyName);

	/**
	 * A variant of ExposeParameter that retrieves the target node from the named registry.
	 *
	 * The created parameter is automatically stored in the named object registry under its name.
	 */
	FCameraRigAssetTestBuilder& ExposeParameter(const FString& ParameterName, const FString& TargetName, FName TargetPropertyName);

	/** Gets the named object registry. */
	virtual FNamedObjectRegistry* GetNamedObjectRegistry() override
	{
		return NamedObjectRegistry.Get();
	}

private:

	void Initialize(TSharedPtr<FNamedObjectRegistry> InNamedObjectRegistry, FName Name, UObject* Outer);

private:

	UCameraRigAsset* CameraRig;

	TSharedPtr<FNamedObjectRegistry> NamedObjectRegistry;
};

}  // namespace UE::Cameras::Test

