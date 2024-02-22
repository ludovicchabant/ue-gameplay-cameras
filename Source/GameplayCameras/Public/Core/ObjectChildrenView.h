// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "Misc/TVariant.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/ObjectPtr.h"

#include <initializer_list>

namespace UE::Cameras
{

/**
 * Structure that describes a list of children of an object.
 *
 * This structure can either provide a TArrayView<> on an existing container of children,
 * or store within itself a list of arbitrary camera node children.
 */
template<typename ChildType>
struct TObjectChildrenView
{
public:

	using FArrayView = TArrayView<ChildType>;
	using FArray = TArray<ChildType, TInlineAllocator<4>>;

	/** An empty view. */
	TObjectChildrenView()
	{
	}

	/** Sets the view to the given TArrayView<>. */
	TObjectChildrenView(FArrayView&& InArrayView)
	{
		Storage.template Set<FArrayView>(MoveTemp(InArrayView));
	}

	/** Sets the view to pointer storage and adds the given list of children pointers. */
	TObjectChildrenView(std::initializer_list<ChildType> InChildren)
	{
		Storage.template Set<FArray>(FArray(InChildren));
	}

	/**
	 * Sets the view to pointer storage (if not already done) and adds the given
	 * pointer to the list.
	 */
	void Add(typename TCallTraits<ChildType>::ParamType InChild)
	{
		if (Storage.GetIndex() != FStorage::template IndexOfType<FArray>())
		{
			Storage.template Set<FArray>(FArray());
		}
		FArray& Array = Storage.template Get<FArray>();
		Array.Add(InChild);
	}

	/** Whether this view has any children. */
	bool IsEmpty() const
	{
		switch (Storage.GetIndex())
		{
			case FStorage::template IndexOfType<FArrayView>():
				return Storage.template Get<FArrayView>().IsEmpty();
			case FStorage::template IndexOfType<FArray>():
				return Storage.template Get<FArray>().IsEmpty();
			default:
				return true;
		}
	}

	/** Returns the number of children. */
	int32 Num() const
	{
		switch (Storage.GetIndex())
		{
			case FStorage::template IndexOfType<FArrayView>():
				return Storage.template Get<FArrayView>().Num();
			case FStorage::template IndexOfType<FArray>():
				return Storage.template Get<FArray>().Num();
			default:
				return 0;
		}
	}

	/** Gets the i'th child. */
	ChildType operator[](int32 Index) const
	{
		switch (Storage.GetIndex())
		{
			case FStorage::template IndexOfType<FArrayView>():
				return Storage.template Get<FArrayView>()[Index];
			case FStorage::template IndexOfType<FArray>():
				return Storage.template Get<FArray>()[Index];
			default:
				return nullptr;
		}
	}

public:

	// Range iteration

	struct FBaseIterator
	{
		const TObjectChildrenView* Owner;
		int32 Index;

		FORCEINLINE ChildType operator*()
		{
			return (*Owner)[Index];
		}

		FORCEINLINE bool operator== (const FBaseIterator& Other) const
		{
			return Owner == Other.Owner
				&& Index == Other.Index;
		}

		FORCEINLINE bool operator!= (const FBaseIterator& Other) const
		{
			return !(*this == Other);
		}
	};

	struct FIterator : FBaseIterator
	{
		FORCEINLINE FIterator& operator++()
		{
			++FBaseIterator::Index;
			return *this;
		}
	};

	FORCEINLINE FIterator begin() const { return FIterator{ this, 0 }; }
	FORCEINLINE FIterator end() const { return FIterator{ this, Num() }; }

	struct FReverseIterator : FBaseIterator
	{
		FORCEINLINE FReverseIterator& operator++()
		{
			--FBaseIterator::Index;
			return *this;
		}
	};

	FORCEINLINE FReverseIterator rbegin() const { return FReverseIterator{ this, Num() - 1 }; }
	FORCEINLINE FReverseIterator rend() const { return FReverseIterator{ this, -1 }; }

private:

	using FStorage = TVariant<FArrayView, FArray>;
	FStorage Storage;
};

}  // namespace UE::Cameras

