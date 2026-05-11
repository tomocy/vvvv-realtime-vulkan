#pragma once

#include "error.h"
#include "result.h"

namespace vvvv {
struct Renderer { };
} // namespace vvvv

namespace vvvv {
struct CreateRenderer {
public:
    Result::Either<Renderer, Error> operator()() const noexcept
    {
        return Result::OK(Renderer {});
    }
};
} // namespace vvvv
