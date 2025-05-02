#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <zlib.h>
#include <arpa/inet.h>
#include <cstring>

uint32_t crcTable[256];

// Initialize the CRC table with the standard CRC-32 polynomial
void generateCRCTable() {
    uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; ++j) {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
        crcTable[i] = crc;
    }
}


// uint32_t calculate_crc(const uint8_t* data, size_t length) {
//     uint32_t crc = 0xFFFFFFFF;
//     for (size_t i = 0; i < length; ++i) {
//         crc = crcTable[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
//     }
//     return crc ^ 0xFFFFFFFF;
// }

uint32_t calculate_crc(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;  // Initial value

    for (size_t i = 0; i < length; ++i) {
        uint8_t byte = data[i];
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;  // Final XOR value
}

void write_chunk(std::ofstream& file, const char* type, const uint8_t* data, uint32_t length) {
    uint32_t length_be = htonl(length);
    file.write(reinterpret_cast<const char*>(&length_be), 4);

    file.write(type, 4);
    if (length > 0) {
        file.write(reinterpret_cast<const char*>(data), length);
    }

    std::vector<uint8_t> crc_data(4 + length);
    memcpy(crc_data.data(), type, 4);
    if (length > 0) {
        memcpy(crc_data.data() + 4, data, length);
    }
    
    uint32_t crc = calculate_crc(crc_data.data(), crc_data.size());
    uint32_t crc_be = htonl(crc);
    file.write(reinterpret_cast<const char*>(&crc_be), 4);
}

void write_png(const std::vector<uint8_t> &pixel_data, uint32_t width, uint32_t height) {
    std::ofstream file("output.png", std::ios::binary);
    
    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    // PNG signature
    const uint8_t signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    file.write(reinterpret_cast<const char*>(signature), 8);

    // IHDR chunk
    uint8_t ihdr_data[13];
    uint32_t width_be = htonl(width);
    uint32_t height_be = htonl(height);
    memcpy(ihdr_data, &width_be, 4);
    memcpy(ihdr_data + 4, &height_be, 4);
    ihdr_data[8] = 8;  // Bit depth
    ihdr_data[9] = 2;  // Color type (RGB)
    ihdr_data[10] = 0; // Compression method
    ihdr_data[11] = 0; // Filter method
    ihdr_data[12] = 0; // Interlace method

    write_chunk(file, "IHDR", ihdr_data, 13);

    // Prepare uncompressed data
    std::vector<uint8_t> uncompressedData;
    uncompressedData.reserve(height * (1 + width * 3));

    for (uint32_t y = 0; y < height; ++y) {
        uncompressedData.push_back(0);  // Filter byte
        for (uint32_t x = 0; x < width; ++x) {
            size_t index = (y * width + x) * 3;
            uncompressedData.push_back(pixel_data[index]);     // Red
            uncompressedData.push_back(pixel_data[index + 1]); // Green
            uncompressedData.push_back(pixel_data[index + 2]); // Blue
        }
    }

    // Compress data
    uLongf compressedDataSize = compressBound(uncompressedData.size());
    std::vector<uint8_t> compressedData(compressedDataSize);

    int result = compress2(compressedData.data(), &compressedDataSize, 
                           uncompressedData.data(), uncompressedData.size(),
                           Z_BEST_COMPRESSION);
    if (result != Z_OK) {
        std::cerr << "Compression failed!" << std::endl;
        return;
    }

    compressedData.resize(compressedDataSize);

    // Write IDAT chunk
    write_chunk(file, "IDAT", compressedData.data(), compressedDataSize);

    // Write IEND chunk
    write_chunk(file, "IEND", nullptr, 0);

    file.close();
    std::cout << "All Chunks written." << std::endl;
}

int main() {

    std::ifstream file("output.csv");
    if (!file.is_open()){
        std::cerr<<"File cannot be opened"<<std::endl;
        return 0;
    }
    
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::string value;
    std::vector<uint8_t> pixelData;


    while (std::getline(ss, value, ',')) {
        pixelData.push_back(static_cast<uint8_t>(std::stoi(value)));
    }

    file.close();

    generateCRCTable();

    uint32_t width = 912;
    uint32_t height = 513;

    write_png(pixelData, width, height);
    
    return 0;
}