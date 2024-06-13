#pragma once

#include <cstdint>

namespace MagicNumbers {

    enum FileType {
        UNKNOWN,
        BMP,
        EXE,
        GIF87A,
        GIF89A,
        JPEG,
        MP3,
        MP4,
        PDF,
        PNG,
        RAR,
        RIFF,
        SEVEN_ZIP,
        TIFF_BE,
        TIFF_LE,
        ZIP
    };

    // Big-endian format (most significant byte first)
    const uint32_t JPEG_SIGNATURE = 0xFFD8FF;
    const uint64_t PNG_SIGNATURE = 0x89504E470D0A1A0A;
    const uint64_t GIF87A_SIGNATURE = 0x474946383761;
    const uint64_t GIF89A_SIGNATURE = 0x474946383961;
    const uint32_t PDF_SIGNATURE = 0x25504446;
    const uint32_t ZIP_SIGNATURE = 0x504B0304;
    const uint64_t RAR_SIGNATURE = 0x526172211A0700;
    const uint64_t SEVEN_ZIP_SIGNATURE = 0x377ABCAF271C;
    const uint32_t MP3_SIGNATURE = 0x494433;
    const uint64_t MP4_SIGNATURE = 0x0000001866747970;
    const uint32_t RIFF_SIGNATURE = 0x52494646;
    const uint16_t EXE_SIGNATURE = 0x4D5A;
    const uint16_t BMP_SIGNATURE = 0x424D;
    const uint32_t TIFF_LE_SIGNATURE = 0x49492A00;
    const uint32_t TIFF_BE_SIGNATURE = 0x4D4D002A;

    // Little-endian format (least significant byte first)
    // For reference, not used in identification directly
    const uint32_t JPEG_SIGNATURE_LE = 0xFFD8FF00;
    const uint64_t PNG_SIGNATURE_LE = 0x0A1A0A0D474E5089;
    const uint64_t GIF87A_SIGNATURE_LE = 0x6137384947460000;
    const uint64_t GIF89A_SIGNATURE_LE = 0x6139384947460000;
    const uint32_t PDF_SIGNATURE_LE = 0x46445025;
    const uint32_t ZIP_SIGNATURE_LE = 0x04034B50;
    const uint32_t ZIP_EOCD_SIGNATURE_LE = 0x06054b50;
    const uint64_t RAR_SIGNATURE_LE = 0x00071A2121726152;
    const uint64_t SEVEN_ZIP_SIGNATURE_LE = 0x1C27AFBC7A370000;
    const uint32_t MP3_SIGNATURE_LE = 0x33494400;
    const uint64_t MP4_SIGNATURE_LE = 0x70797466680A1A00;
    const uint32_t RIFF_SIGNATURE_LE = 0x46464952;
    const uint16_t EXE_SIGNATURE_LE = 0x5A4D;
    const uint16_t BMP_SIGNATURE_LE = 0x4D42;
    const uint32_t TIFF_LE_SIGNATURE_LE = 0x002A4949;
    const uint32_t TIFF_BE_SIGNATURE_LE = 0x2A004D4D;

}

