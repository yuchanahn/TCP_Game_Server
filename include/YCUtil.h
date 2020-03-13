#pragma once
#include <list>
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>

namespace YC 
{
	template<class T>
	static std::vector<T> Filter(std::vector<T> m, std::function<bool(T&)> f)
	{
		std::vector<T> l;

		std::for_each(m.begin(), m.end(),
			[&l, &f](T& data)
			{
				if (f(data))
				{
					l.push_back(data);
				}
			});
		return l;
	}

	template<class T>
	static std::vector<T> Map(std::vector<T> m, std::function<T(T&)> f)
	{
		std::vector<T> l;

		std::for_each(m.begin(), m.end(),
			[&l, &f](T& data)
			{
				l.push_back(f(data));
			});
		return l;
	}
}

class YCList
{
public:

};
