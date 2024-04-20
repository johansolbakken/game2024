#pragma once

#include <string>
#include <vector>
#include <map>

#include <memory>
#include <functional>
#include <algorithm>

#include <iostream>

namespace JohanGame2024
{
    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename ... Args>
    constexpr Ref<T> createRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}