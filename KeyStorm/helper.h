#pragma once
#include <vector>
#include <cstddef>
#include <string>
#include <iostream>
#include <fstream>
#include <zip.h>

void displayProgressBar(unsigned long long current, unsigned long long total);
void uint32_to_uint8_array(uint32_t num, uint8_t arr[4]);
void write_vector_to_zip(const std::vector<uint8_t>& data, const std::string& file_path);
std::vector<uint8_t> readFile(const std::string& file_path);
int bruteforceZipPasswords(zip_t* archive, const std::string& passwordlist);
zip_t* openZip(const char* filename);

