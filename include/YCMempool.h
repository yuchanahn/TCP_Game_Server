#pragma once
#include <iostream>
#include <assert.h>
#include <list>
#include "YCStack.h"

template <typename T>
class YCMempool
{
	YCStack<T> pool;
	std::list<T*> new_pool;
public:

	template<typename... args_t>
	T* YCNew(args_t&& ...args)
	{
		if (pool.size() == 0)
		{
			pool.push(new T(args...));
		}
		auto v = pool.pop();
		*v = T( args... );
		new_pool.push_back(v);
		return v;
	}
	void Delete(T*& value)
	{
		pool.push(value);
		new_pool.remove(value);
		value = nullptr;
	}
	void DeleteAll()
	{

	}
};