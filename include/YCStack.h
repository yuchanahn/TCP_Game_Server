#pragma once
#include <iostream>
#include <assert.h>

template <typename T>
class YCStack
{
	const int add_size = 10;

	int add_stack = 0;

	void** buf = nullptr;
	int current_idx = 0;
public:
	const int size() { return current_idx; }
	const bool empty() { return size() == 0; }
	void push(T* v)
	{
		if (add_stack == current_idx)
		{
			auto garbage = buf;

			buf = new void* [add_stack += add_size];
			if (garbage != nullptr)
			{
				std::copy(buf, buf + add_stack - add_size, garbage);
			}
		}
		buf[current_idx++] = v;
	}
	T* pop()
	{
		if (current_idx == 0)
		{
			assert("current idx가 0 이하입니다.");
		}
		return (T*)(buf[--current_idx]);
	}


	~YCStack()
	{
		if (buf == nullptr) return;
		for (int i = 0; i < current_idx; i++)
		{
			delete buf[i];
		}
		delete[] buf;
	}
};