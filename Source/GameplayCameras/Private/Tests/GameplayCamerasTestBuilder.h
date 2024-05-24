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
class FCameraRigAssetTestBuilder : public TCameraObjectInitializer<UCameraRigAsset>
{
public:

	using ThisType = FCameraRigAssetTestBuilder;

	FCameraRigAssetTestBuilder(FName Name = NAME_None, UObject* Outer = nullptr);

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
	 */
	FCameraRigAssetTestBuilder& ExposeParameter(const FString& ParameterName, UCameraNode* Target, FName TargetPropertyName);

private:

	UCameraRigAsset* CameraRig;
};

}  // namespace UE::Cameras::Test

