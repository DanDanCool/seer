#pragma once

#include "CoreMinimal.h"

template <typename T>
class ResourcePool {
	using Type = T;

	public:

	ResourcePool() {

	}

	template <class... Args>
	ResourcePool(int size, Args... args) {
		for (int i = 0; i < size; i++) {
			int idx = _Resource.Emplace(args...);
			_Free.Add(&_Resource[idx]);
		}
	}

	~ResourcePool() {

	}

	ResourcePool<Type>& operator=(ResourcePool<Type>&& other) {
		_Resource = MoveTempIfPossible(other._Resource);
		_Free = MoveTempIfPossible(other._Free);
		return *this;
	}

	template <class... Args>
	Type& GetResource(Args... args) {
		if (_Free.IsEmpty()) {
			_Resource.Emplace(args...);
			int idx = _Resource.Num() - 1;
			return _Resource[idx];
		}

		int idx = _Free.Num() - 1;
		Type* resource = _Free[idx];
		_Free.RemoveAt(idx);
		return *resource;
	}

	void FreeResource(Type& resource) {
		_Free.Add(&resource);
	}

	TArray<Type> _Resource;
	TArray<Type*> _Free;
};
