#pragma once
#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include <type_traits>
#include <utility>

template <typename Function, typename ...CapturedArgs>
class curried
{
public:
    curried(Function function, CapturedArgs... args)
        : m_function(function)
        , m_captured(std::make_tuple(args...))
    {
    }

    curried(Function function, std::tuple<CapturedArgs...> args)
        : m_function(function)
        , m_captured(args)
    {
    }

    template <typename ...NewArgs>
    auto operator()(NewArgs&& ...args) const
    {
        auto new_args = std::make_tuple(std::forward<NewArgs>(args)...);
        auto all_args = std::tuple_cat(m_captured, std::move(new_args));

        if constexpr (std::is_invocable_v<Function, CapturedArgs...,
            NewArgs...>) {
            return std::apply(m_function, all_args);
        }
        else {
            return curried<Function, CapturedArgs..., NewArgs...>(
                m_function, all_args);
        }
    }

private:
    Function m_function;
    std::tuple<CapturedArgs...> m_captured;
};


// Needed for pre-C++17 compilers
template <typename Function>
curried<Function> make_curried(Function&& f)
{
    return curried<Function>(std::forward<Function>(f));
}