#pragma once
#include <iostream>
#include <functional>
#include <list>

namespace yc
{
	template <typename T>
	struct CE
	{
		std::list<std::function<void()>> pfs;
		std::list<std::function<void()>> fs;
		T val;

		void on_change(std::function<void()> f)
		{
			fs.push_back(f);
		}

		void prev_add(std::function<void()> f)
		{
			pfs.push_back(f);
		}

		CE() {}
		CE(T& o)
		{
			operator=(o);
		}
		CE(T&& o)
		{
			operator=(std::move(o));
		}
		T& operator=(T& o)
		{
			for (auto& i : pfs) i();
			val = o;
			for (auto& i : fs) i();
			return val;
		}
		T& operator=(T&& o)
		{
			for (auto& i : pfs) i();
			val = std::move(o);
			for (auto& i : fs) i();
			return val;
		}
		operator T()
		{
			return val;
		}
	};
}