#pragma once

#include <concepts>
#include <type_traits>
#include <vulkan/vulkan_core.h>

namespace vvvv {
template <typename T>
struct GetVkProcessAddressTrait;

template <typename T>
concept GetVkProcessAddressConcept = requires(T v, const char* name) {
    { GetVkProcessAddressTrait<T>::get(v, name) } -> std::same_as<PFN_vkVoidFunction>;
};

template <typename T>
    requires GetVkProcessAddressConcept<T>
struct GetVkProcessAddress {
public:
    explicit GetVkProcessAddress(T context) noexcept
        : context(context)
    {
    }

public:
    template <typename U>
        requires std::is_function_v<std::remove_pointer_t<U>>
    U invoke() const noexcept
    {
        const auto addr = GetVkProcessAddressTrait<T>::get(context, name);
        return reinterpret_cast<U>(addr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    }

public:
    template <typename F>
        requires std::invocable<F&, GetVkProcessAddress&>
    GetVkProcessAddress& with(F f)
    {
        f(*this);
        return *this;
    }

public:
    T context {};
    const char* name = nullptr;
};
} // namespace vvvv

namespace vvvv {
template <>
struct GetVkProcessAddressTrait<VkInstance> {
public:
    static PFN_vkVoidFunction get(VkInstance instance, const char* const name)
    {
        return vkGetInstanceProcAddr(instance, name);
    }
};
} // namespace vvvv
