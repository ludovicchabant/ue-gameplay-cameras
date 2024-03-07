// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Containers/SparseArray.h"
#include "CoreTypes.h"
#include "UObject/NameTypes.h"

namespace UE::Cameras
{

/**
 * Identifier for a given RTTI-enabled class.
 */
struct FCameraObjectTypeID
{
	friend uint32 GetTypeHash(FCameraObjectTypeID In)
	{
		return In.ID;
	}

	friend bool operator<(FCameraObjectTypeID A, FCameraObjectTypeID B)
	{
		return A.ID < B.ID;
	}

	friend bool operator==(FCameraObjectTypeID A, FCameraObjectTypeID B)
	{
		return A.ID == B.ID;
	}

	uint32 GetTypeID() const
	{
		return ID;
	}

	const FName& GetTypeName() const
	{
		return Name;
	}

	FCameraObjectTypeID(const FName& InName, uint32 InID)
		: Name(InName)
		, ID(InID)
	{}

protected:

	static uint32 RegisterNewID();

protected:

	FName Name;
	uint32 ID;
};

/**
 * Type information about an RTTI-enabled class.
 */
struct FCameraObjectTypeInfo
{
	using FConstructor = void(*)(void*);
	FConstructor Constructor;

	using FDestructor = void(*)(void*);
	FDestructor Destructor;
};

/**
 * Type registry of known RTTI-enabled classes.
 */
class FCameraObjectTypeRegistry
{
public:

	static FCameraObjectTypeRegistry& Get();

	void RegisterType(FCameraObjectTypeID TypeID, FCameraObjectTypeInfo&& TypeInfo);
	void ConstructObject(FCameraObjectTypeID TypeID, void* Ptr);

private:

	TMap<FName, uint32> TypeIDsByName;
	TSparseArray<FCameraObjectTypeInfo> TypeInfos;
};

/**
 * Strongly-typed ID wrapper for an RTTI-enabled class.
 */
template<typename T>
struct TCameraObjectTypeID : FCameraObjectTypeID
{
private:

	TCameraObjectTypeID(const FName& InName, uint32 InID) : FCameraObjectTypeID(InName, InID) {}

	static TCameraObjectTypeID RegisterType(const FName& InClassName)
	{
		TCameraObjectTypeID NewTypeID(InClassName, FCameraObjectTypeID::RegisterNewID());
		FCameraObjectTypeInfo NewTypeInfo { 
			&TCameraObjectTypeID<T>::StaticConstructor,
			&TCameraObjectTypeID<T>::StaticDestructor
		};
		FCameraObjectTypeRegistry::Get().RegisterType(NewTypeID, MoveTemp(NewTypeInfo));
		return NewTypeID;
	}

	static void StaticConstructor(void* Ptr)
	{
		new(Ptr) T();
	}

	static void StaticDestructor(void* Ptr)
	{
		reinterpret_cast<T*>(Ptr)->~T();
	}

	friend T;
};

}  // namespace UE::Cameras

// Macros for enabling simple RTTI information on a class hierarchy.
//
// The first macro is for the root of the class hierarchy, while the second is for all
// other classes below it. The third macro goes in the cpp file of each class.
//
#define UE_GAMEPLAY_CAMERAS_DECLARE_RTTI_BASE(ClassName)\
	public:\
		static const ::UE::Cameras::TCameraObjectTypeID<ClassName>& StaticTypeID() { return ClassName::PrivateTypeID; }\
		virtual const FCameraObjectTypeID& GetTypeID() const { return ClassName::PrivateTypeID; }\
		virtual bool IsKindOf(const FCameraObjectTypeID& InTypeID) const { return InTypeID == ClassName::PrivateTypeID; }\
		template<typename Type> bool IsKindOf() const { return IsKindOf(Type::StaticTypeID()); }\
		template<typename Type> Type* CastThis() { return IsKindOf<Type>() ? static_cast<Type*>(this) : nullptr; }\
		template<typename Type> const Type* CastThis() const { return IsKindOf<Type>() ? static_cast<Type*>(this) : nullptr; }\
		template<typename Type> Type* CastThisChecked() { check(IsKindOf<Type>()); return static_cast<Type*>(this); }\
		template<typename Type> const Type* CastThisChecked() const { check(IsKindOf<Type>()); return static_cast<Type*>(this); }\
	private:\
		static const ::UE::Cameras::TCameraObjectTypeID<ClassName> PrivateTypeID;

#define UE_GAMEPLAY_CAMERAS_DECLARE_RTTI(ClassName, BaseClassName)\
	public:\
		static const ::UE::Cameras::TCameraObjectTypeID<ClassName>& StaticTypeID() { return ClassName::PrivateTypeID; }\
		virtual const FCameraObjectTypeID& GetTypeID() const override { return ClassName::PrivateTypeID; }\
		virtual bool IsKindOf(const FCameraObjectTypeID& InTypeID) const override { return (InTypeID == ClassName::PrivateTypeID) || BaseClassName::IsKindOf(InTypeID); }\
	private:\
		static const ::UE::Cameras::TCameraObjectTypeID<ClassName> PrivateTypeID;

#define UE_GAMEPLAY_CAMERAS_DEFINE_RTTI(ClassName)\
	const ::UE::Cameras::TCameraObjectTypeID<ClassName> ClassName::PrivateTypeID = ::UE::Cameras::TCameraObjectTypeID<ClassName>::RegisterType(#ClassName);

