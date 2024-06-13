#include "fileChecker.h"

bool is_valid_zip(const std::vector<uint8_t>& data) {
    //replace with other file signature possible
    const uint32_t LOCAL_FILE_HEADER_SIGNATURE = MagicNumbers::ZIP_SIGNATURE_LE;
    const uint32_t EOCD_SIGNATURE = MagicNumbers::ZIP_EOCD_SIGNATURE_LE;


    if (data.size() < 30) {
        // Not enough data for a local file header
        return false;
    }

    // Check the local file header signature
    uint32_t local_file_header_sig = *reinterpret_cast<const uint32_t*>(&data[0]);
    if (local_file_header_sig != LOCAL_FILE_HEADER_SIGNATURE) {
        return false;
    }

    for (size_t i = data.size() - 22; i >= 0; --i) {
        uint32_t eocd_sig = *reinterpret_cast<const uint32_t*>(&data[i]);
        if (eocd_sig == EOCD_SIGNATURE) {
            std::cout << "[+] The ZIP file is valid." << std::endl;
            return true;
        }
    }

    return false;
}