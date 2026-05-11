#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <vector>

#include "error.h"
#include "result.h"

namespace vvvv {
struct FileReader {
public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] Result::Either<std::vector<std::byte>, Error> read(const std::filesystem::path& path) const
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            return Result::Error(Error(std::format("failed to open the file: {}", path.string())));
        }

        const auto size = file.tellg();
        if (size < 0) {
            return Result::Error(Error(std::format("failed to get the file size: {}", path.string())));
        }

        file.seekg(0);
        if (!file) {
            return Result::Error(Error(std::format("failed to seek to the beginning of the file: {}", path.string())));
        }

        std::vector<std::byte> data(static_cast<size_t>(size));
        file.read(reinterpret_cast<char*>(data.data()), size); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        if (!file) {
            return Result::Error(Error(std::format("failed to read from the file: {}", path.string())));
        }

        return Result::OK(std::move(data));
    }
};
} // namespace vvvv
