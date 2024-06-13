#include <iostream>
#include <vector>
#include <thread>
#include <omp.h>

#include "decrypt.h"
#include "fileChecker.h"
#include "helper.h"


const uint32_t MAX_KEYS = UINT32_MAX;  // 2^32 - 1 keys, covering 0 to 4294967295
omp_lock_t console_lock;  // OpenMP lock for synchronizing console output

// Shared flag
volatile int found = 0;

//key bruteforce
void process_keys(uint32_t start_key, uint32_t num_keys, const std::vector<uint8_t>& cipher, const std::string& output_path, int thread_id) {
    uint32_t keys_processed = 0;
    int count = 0;

    for (uint32_t i = 0; i < static_cast<int32_t>(num_keys); ++i) {

        if (found)
            continue;


        uint32_t key = start_key + i;
        uint8_t k[4];
        uint32_to_uint8_array(key, k);

        std::vector<uint8_t> c;

        //function from decrypt.cpp
        //add any function to use ur own decrypt algo
        xor_cipher(cipher, k, c);

        //check first bytes if zip could be any other file format
        if (is_valid_zip(c)) {
            //
            std::cout << "\n[+] Key Found: " << std::hex << key << std::endl;

#pragma omp critical
            if (!found) {
                found = 1;
            }

            decryptWithKey(k, cipher, output_path);
            std::cout << "\n[+] Key generation and processing complete." << std::endl;
        }

        if (!found) {
            count++;
            if (count % 10000000 == 0)
#pragma omp critical
                std::cout << "[-] Thread " << thread_id << ": tried " << count << " Keys but nothing Found" << std::endl;
        }
    }

}

int main() {
    
     // Calculate the optimal number of threads
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 2;  // Fallback to 2 threads if hardware_concurrency returns 0
    }

    uint32_t range_per_thread = MAX_KEYS / num_threads + 1;

    const char* encryptedFile = "geheim.zip.crypt";
    const char* outputFile = "geheim.zip";
    const char* passwordlist = "rockyou.txt";


    std::vector<uint8_t> zipCipher = readFile(encryptedFile);

    std::cout << "[+] Starting OpenMP parallel region with " << num_threads << " threads..." << std::endl;

#pragma omp parallel for num_threads(num_threads) schedule(static)
    for (int i = 0; i < num_threads; ++i) {
        uint32_t start_key = i * range_per_thread;
        uint32_t num_keys = (i == num_threads - 1) ? (MAX_KEYS - start_key + 1) : range_per_thread;
#pragma omp critical
        std::cout << "[#] Thread " << omp_get_thread_num() << " processing range [" << start_key << ", " << (start_key + num_keys) << "]" << std::endl;
        process_keys(start_key, num_keys, zipCipher, outputFile, omp_get_thread_num());

    }

    omp_destroy_lock(&console_lock);

    bruteforceZipPasswords(openZip(outputFile), passwordlist);



    return 0;
}
