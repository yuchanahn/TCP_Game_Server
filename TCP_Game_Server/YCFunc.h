#pragma once
#include <optional>
#include <queue>

template <typename... Ts> struct overloaded : Ts... { using Ts::operator()...;  };
template <typename... Ts> overloaded(Ts...)->overloaded<Ts...>;

namespace yc
{
	template <typename T>
	static std::optional<T> val_pop(std::queue<std::optional<T>>& o)
	{
		auto r = o.empty() ? std::nullopt : o.back();
		if(r.has_value()) o.pop();
		return r;
	}
}



#define lamda 