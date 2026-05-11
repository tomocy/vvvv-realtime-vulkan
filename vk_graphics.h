#pragma once

#include "error.h"
#include "result.h"

namespace vvvv {
struct VkGraphicsKernel { };
} // namespace vvvv

namespace vvvv {
struct CreateVkGraphicsKernel {
public:
    Result::Either<VkGraphicsKernel, Error> operator()() const
    {
        return Result::OK(VkGraphicsKernel());
    }
};
} // namespace vvvv
