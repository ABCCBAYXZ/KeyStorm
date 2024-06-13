#include "decrypt.h"

void xor_cipher(const std::vector<uint8_t>& cipher, const uint8_t* key, std::vector<uint8_t>& output) {
    for (size_t i = 0; i < 129; i += 4) {
        for (size_t j = 0; j < 4 && (i + j) < cipher.size(); ++j) {
            output.push_back(cipher[i + j] ^ key[j % 4]);
        }
    }
}

void decryptWithKey(uint8_t* key, const std::vector<uint8_t>& cipher, const std::string& output_path) {

    size_t s = 4;
    std::vector<uint8_t> c;

    for (size_t i = 0; i < cipher.size(); i += s) {
        for (size_t j = 0; j < s && (i + j) < cipher.size(); ++j) {
            c.push_back(cipher[i + j] ^ key[j % 4]);
        }
    }

    write_vector_to_zip(c, output_path);
        std::cout << "[+] ZIP file saved as: " << output_path << std::endl;
        
}
