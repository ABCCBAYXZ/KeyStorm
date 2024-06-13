#include "helper.h"

// Function to display the progress bar
void displayProgressBar(unsigned long long current, unsigned long long total) {
    const int barWidth = 70; // Width of the progress bar

    float progress = (float)current / total;
    int pos = barWidth * progress;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    std::cout << "] " << int(progress * 100.0) << " % (" << current << "/" << total << " Keys)\r";
    std::cout.flush();
}

void uint32_to_uint8_array(uint32_t num, uint8_t arr[4]) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&num);
    for (int i = 0; i < 4; i++) {
        arr[i] = ptr[i];  // Copy each byte
    }
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

// Function to open a zip file and check if it opened successfully
zip_t* openZip(const char* filename) {
    int err;
    zip_t* archive = zip_open(filename, 0, &err);

    // Check if the zip file was opened successfully
    if (archive == NULL) {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, err);
        printf("Failed to open the zip file: %s\n", zip_error_strerror(&ziperror));
        zip_error_fini(&ziperror);
    }

    return archive;
}

int bruteforceZipPasswords(zip_t* archive, const std::string& passwordlist) {

    std::cout << "[+] Starting to bruteforce ZIP Password" << std::endl;

    zip_int64_t num_entries = zip_get_num_entries(archive, 0);

    std::ifstream password_file(passwordlist);
    if (!password_file.is_open()) {
        std::cerr << "[!] Failed to open password file: " << passwordlist << std::endl;
        return 0;
    }


    std::string password;
    while (std::getline(password_file, password)) {
        bool passfound = true;
        for (zip_int64_t i = 0; i < num_entries; i++) {
            zip_stat_t stat;
            if (zip_stat_index(archive, i, 0, &stat) != 0) {
                passfound = false;// Failed to get file stats
                continue;
            }

            zip_file_t* zf = zip_fopen_index_encrypted(archive, i, 0, password.c_str());
            if (zf == NULL) {
                passfound = false;// Password failed
                continue;
            }

            std::vector<uint8_t> buffer(stat.size);
            if (zip_fread(zf, buffer.data(), buffer.size()) != buffer.size()) {
                passfound = false;// Failed to read the correct amount of data
                continue;
            }

            zip_fclose(zf);
        }

        if (passfound) {
            std::cout << "[+] Password found: " << password << std::endl;
            return 1; // Password succeeded
        }

    }

    std::cout << "[-] Password not found!" << std::endl;
}