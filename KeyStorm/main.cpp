#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <thread>
#include <omp.h>
#include <zip.h>
#include <string>

const uint32_t MAX_KEYS = UINT32_MAX;  // 2^32 - 1 keys, covering 0 to 4294967295

omp_lock_t console_lock;  // OpenMP lock for synchronizing console output

void uint32_to_uint8_array(uint32_t num, uint8_t arr[4]) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&num);
    for (int i = 0; i < 4; i++) {
        arr[i] = ptr[i];  // Copy each byte
    }
}

bool is_valid_zip(const std::vector<uint8_t>& data) {
    //replace with other file signature possible
    const uint32_t LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
    const uint32_t EOCD_SIGNATURE = 0x06054b50;

    if (data.size() < 30) {
        // Not enough data for a local file header
        return false;
    }

    // Check the local file header signature
    uint32_t local_file_header_sig = *reinterpret_cast<const uint32_t*>(&data[0]);
    if (local_file_header_sig != LOCAL_FILE_HEADER_SIGNATURE) {
        return false;
    }

    // Check the end of central directory record signature
    if (data.size() < 22) {
        // Not enough data for the EOCD record
        return false;
    }

    for (size_t i = data.size() - 22; i >= 0; --i) {
        uint32_t eocd_sig = *reinterpret_cast<const uint32_t*>(&data[i]);
        if (eocd_sig == EOCD_SIGNATURE) {
            return true;
        }
    }

    return false;
}

// Function to write a std::vector<uint8_t> to a ZIP file
void write_vector_to_zip(const std::vector<uint8_t>& data, const std::string& file_path) {
    std::ofstream output_file(file_path, std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "[!] Failed to open " << file_path << " for writing." << std::endl;
        return;
    }

    output_file.write(reinterpret_cast<const char*>(data.data()), data.size());
    output_file.close();

    if (output_file) {
        std::cout << "[+] Successfully wrote to " << file_path << std::endl;
    }
    else {
        std::cerr << "[!] Failed to write to " << file_path << std::endl;
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

    if (is_valid_zip(c)) {
        std::cout << "[+] ZIP file saved as: " << output_path << std::endl;
        write_vector_to_zip(c, "geheim.zip");
    }
    else
    {
        std::cout << "bad key or file corrupted!" << output_path << std::endl;
    }

}


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
        //key = 16842753;
        uint8_t k[4];
        uint32_to_uint8_array(key, k);

        size_t s = 4;
        std::vector<uint8_t> c;


        //std::cout << "cipher size " << cipher.size() << std::endl;
        //only decrypt header for faster results
        //here goes any decryption algo
        for (size_t i = 0; i < 129; i += s) {
            for (size_t j = 0; j < s && (i + j) < cipher.size(); ++j) {
                c.push_back(cipher[i + j] ^ k[j % 4]);
            }
        }

        //check first bytes if zip could be any other file format
        if (is_valid_zip(c)) {
            //std::cout << "[+] The ZIP file is valid." << std::endl;
            std::cout << "\n[+] Key Found: " << std::hex << key << std::endl;

            #pragma omp critical
            if (!found) {
                found = 1;
            }

            decryptWithKey(k, cipher, "geheim.zip");
           
        }

        if (!found) {
            count++;
            if (count % 10000000 == 0)
                #pragma omp critical
                std::cout << "[-] Thread " << thread_id << ": tried " << count << " Keys but nothing Found" << std::endl;
        }
        }  
        
    }



std::vector<uint8_t> readFile(const std::string& file_path) {
    std::ifstream input_file(file_path, std::ios::binary);
    if (!input_file.is_open()) {
        std::cerr << "[!] Failed to open " << file_path << " for reading." << std::endl;
        std::exit(-1);
    }

    std::vector<uint8_t> c((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();

    return c;
}

int try_password(zip_t* archive, const std::string& password) {
    zip_int64_t num_entries = zip_get_num_entries(archive, 0);
    for (zip_int64_t i = 0; i < num_entries; i++) {
        zip_stat_t stat;
        if (zip_stat_index(archive, i, 0, &stat) != 0) {
            return 0; // Failed to get file stats
        }

        zip_file_t* zf = zip_fopen_index_encrypted(archive, i, 0, password.c_str());
        if (zf == NULL) {
            zip_fclose(zf);
            return 0; // Password failed
        }

        std::vector<uint8_t> buffer(stat.size);
        if (zip_fread(zf, buffer.data(), buffer.size()) != buffer.size()) {
            zip_fclose(zf);
            return 0; // Failed to read the correct amount of data
        }

        zip_fclose(zf);
    }
    return 1; // Password succeeded
}
// Function to try opening the ZIP file with a given password
int try_passwords(zip_t* archive, const std::string& passwordlist) {
    std::ifstream password_file(passwordlist);
    if (!password_file.is_open()) {
        std::cerr << "[!] Failed to open password file: " << passwordlist << std::endl;
        return 0;
    }

    std::string password;
    while (std::getline(password_file, password)) {
        if (try_password(archive, password)) {
            std::cout << "[+] Password found: " << password << std::endl;
            return 1;
        }
    }

    return 0;
}

int main() {
   // omp_init_lock(&console_lock);

    // Calculate the optimal number of threads
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 2;  // Fallback to 2 threads if hardware_concurrency returns 0
    }

    uint32_t range_per_thread = MAX_KEYS / num_threads + 1;

    std::vector<uint8_t> zipCipher = readFile("geheim.zip.crypt");

    std::cout << "[+] Starting OpenMP parallel region with " << num_threads << " threads..." << std::endl;

    #pragma omp parallel for num_threads(num_threads) schedule(static)
    for (int i = 0; i < num_threads; ++i) {
        uint32_t start_key = i * range_per_thread; 
        uint32_t num_keys = (i == num_threads - 1) ? (MAX_KEYS - start_key + 1) : range_per_thread;
        #pragma omp critical
        std::cout << "[#] Thread " << omp_get_thread_num() << " processing range [" << start_key << ", " << (start_key + num_keys) << "]" << std::endl;
        process_keys(start_key, num_keys, zipCipher, "outputfile.zip", omp_get_thread_num());

    }

    omp_destroy_lock(&console_lock);

    std::cout << "\n[+] Key generation and processing complete." << std::endl;
    std::cout << "[+] Starting to bruteforce ZIP Password" << std::endl;

    int err;
    zip_t* archive = zip_open("geheim.zip", 0, &err);
    try_passwords(archive, "rockyou.txt");

   

    return 0;
}
