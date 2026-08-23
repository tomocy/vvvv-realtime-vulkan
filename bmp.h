#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "error.h"
#include "result.h"

namespace vvvv {
// [BITMAPFILEHEADER structure](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader)
#pragma pack(push, 1)
struct BMPFileHeader {
    std::array<uint8_t, 2> type;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
};
#pragma pack(pop)
static_assert(offsetof(BMPFileHeader, type) == 0);
static_assert(offsetof(BMPFileHeader, fileSize) == 2);
static_assert(offsetof(BMPFileHeader, reserved1) == 6);
static_assert(offsetof(BMPFileHeader, reserved2) == 8);
static_assert(offsetof(BMPFileHeader, dataOffset) == 10);
static_assert(sizeof(BMPFileHeader) == 14);

enum class BMPDIBHeaderType {
    info,
};

// [BITMAPINFOHEADER structure](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader)
#pragma pack(push, 1)
struct BMPInfoHeader {
    uint32_t dibHeaderSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)
static_assert(offsetof(BMPInfoHeader, dibHeaderSize) == 0);
static_assert(offsetof(BMPInfoHeader, width) == 4);
static_assert(offsetof(BMPInfoHeader, height) == 8);
static_assert(offsetof(BMPInfoHeader, planes) == 12);
static_assert(offsetof(BMPInfoHeader, bitsPerPixel) == 14);
static_assert(offsetof(BMPInfoHeader, compression) == 16);
static_assert(offsetof(BMPInfoHeader, imageSize) == 20);
static_assert(offsetof(BMPInfoHeader, xPixelsPerMeter) == 24);
static_assert(offsetof(BMPInfoHeader, yPixelsPerMeter) == 28);
static_assert(offsetof(BMPInfoHeader, colorsUsed) == 32);
static_assert(offsetof(BMPInfoHeader, colorsImportant) == 36);
static_assert(sizeof(BMPInfoHeader) == 40);
} // namespace vvvv

namespace vvvv {
struct BMPEncoder {
public:
    BMPEncoder() = default;
    explicit BMPEncoder(BMPDIBHeaderType dibHeaderType)
        : dibHeaderType(dibHeaderType)
    {
    }

public:
    [[nodiscard]] Result::Either<std::vector<std::byte>, Error> encode(
        std::span<const std::byte> data,
        const uint32_t width,
        const uint32_t height,
        const uint32_t bytesPerPixel
    ) const
    {
        // BMP scan lines are DWORD-aligned (4 bytes).
        // https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-header-types
        constexpr uint32_t rowAlignment = 4;

        const auto rowSize = width * bytesPerPixel;
        const auto rowStride = (rowSize + rowAlignment - 1) & ~(rowAlignment - 1);

        const auto dataSize = rowStride * height;
        const uint32_t fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize;

        std::vector<std::byte> bmp {};
        bmp.reserve(fileSize);

        {
            const auto header = BMPFileHeader {
                .type = { 'B', 'M' },
                .fileSize = fileSize,
                .reserved1 = 0,
                .reserved2 = 0,
                .dataOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader),
            };

            const auto bytes = std::as_bytes(std::span<const BMPFileHeader>(&header, 1));
            bmp.insert(bmp.end(), bytes.begin(), bytes.end());
        }

        switch (dibHeaderType) {
        case BMPDIBHeaderType::info: {
            BMPEncoder::writeInfoHeader(bmp, width, height, bytesPerPixel);
            break;
        }
        default:
            return Result::Error(Error("unsupported DIB header type"));
        }

        // Each scan line is padded to a 4-byte boundary.
        // https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-header-types
        const auto padding = std::vector<std::byte>(rowStride - rowSize, std::byte(0));

        // We currently only support positive heights, which encode the rows as bottom-up.
        // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader
        for (size_t y = height; y > 0; --y) {
            const auto rawStart = (y - 1) * rowSize;
            const auto raw = data.subspan(rawStart, rowSize);
            bmp.insert(bmp.end(), raw.begin(), raw.end());
            bmp.insert(bmp.end(), padding.begin(), padding.end());
        }

        return Result::OK(bmp);
    }

protected:
    static void writeInfoHeader(std::vector<std::byte>& bmp, const uint32_t width, const uint32_t height, const uint32_t bytesPerPixel)
    {
        const auto header = BMPInfoHeader {
            .dibHeaderSize = sizeof(BMPInfoHeader),
            .width = static_cast<int32_t>(width),
            .height = static_cast<int32_t>(height),
            .planes = 1,
            .bitsPerPixel = static_cast<uint16_t>(bytesPerPixel * 8),
            .compression = 0,
            .imageSize = 0,
            .xPixelsPerMeter = 0,
            .yPixelsPerMeter = 0,
            .colorsUsed = 0,
            .colorsImportant = 0,
        };

        const auto bytes = std::as_bytes(std::span<const BMPInfoHeader>(&header, 1));
        bmp.insert(bmp.end(), bytes.begin(), bytes.end());
    }

public:
    BMPDIBHeaderType dibHeaderType = BMPDIBHeaderType::info;
};
} // namespace vvvv
