#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

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
