#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <zlib.h>
#include <arpa/inet.h>
#include <cstring>

uint32_t crcTable[256];

void generateCRCTable() {
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int j = 0; j < 8; ++j) {
            c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
        }
        crcTable[i] = c;
    }
}

uint32_t calculate_crc(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; ++i) {
        crc = crcTable[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
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
    std::cout << "PNG file created successfully." << std::endl;
}

int main() {
    // Example: Create a 2x2 image with different colored pixels
std::vector<uint8_t> pixelData = {
237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137,237,162,90,240,165,101,234,170,106,227,171,107,217,174,111,214,173,117,216,174,124,215,172,130,
215,172,128,219,174,127,216,173,130,221,175,129,220,173,131,218,175,133,221,174,129,216,177,128,
216,175,131,218,176,130,218,174,131,218,178,128,218,178,129,216,175,130,217,177,132,213,175,130,
214,175,131,214,176,133,215,175,132,218,178,128,218,178,132,216,175,131,217,174,132,220,176,134,
217,175,132,219,178,132,214,173,135,216,176,135,212,175,131,212,177,133,219,175,128,219,178,131,
216,176,129,219,178,133,218,176,135,218,176,133,218,177,133,220,175,136,218,174,136,219,178,136,
221,180,136,219,179,135,219,176,133,221,176,133,222,175,133,207,182,133,225,176,133,211,179,136,
221,175,136,211,179,133,205,181,134,229,176,134,219,176,133,228,178,136,224,178,135,221,176,135,
223,180,139,222,176,135,224,175,135,226,175,135,224,178,136,223,180,136,219,179,138,217,181,138,
218,179,135,221,182,137,225,177,130,235,181,136,219,178,136,222,177,134,218,180,134,217,179,136,
220,181,135,220,181,137,217,180,135,217,178,136,221,180,133,221,181,134,223,182,135,221,177,136,
220,177,136,218,179,136,215,178,136,224,181,138,215,179,136,217,179,139,223,179,141,230,182,131,
226,179,135,222,181,139,221,179,139,224,178,137
};
    generateCRCTable();

    uint32_t width = 100;
    uint32_t height = 10;

    write_png(pixelData, width, height);
    
    return 0;
}