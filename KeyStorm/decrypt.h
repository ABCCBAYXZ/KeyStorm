#pragma once
#include <vector>
#include <cstddef>
#include <string>


#include "helper.h"

void xor_cipher(const std::vector<uint8_t>& cipher, const uint8_t* key, std::vector<uint8_t>& output);
void decryptWithKey(uint8_t* key, const std::vector<uint8_t>& cipher, const std::string& output_path);


