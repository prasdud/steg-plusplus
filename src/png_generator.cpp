#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <zlib.h>
#include <arpa/inet.h>



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


uint32_t calculate_crc(const std::vector<uint8_t>& data) {
    uint32_t crc = 0xFFFFFFFF;  // Initial value

    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t byte = data[i];
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;  // Final XOR value
}

void write_png(const std::vector<uint8_t> &pixel_data, int width, int height){
	std::ofstream file("output.png", std::ios::binary);
    
    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

	//png signature
	uint8_t signature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	file.write(reinterpret_cast<char*>(signature), 8);


	//IHDR chunks
	uint32_t widthNet = htonl(width);
	uint32_t heightNet = htonl(height);
	uint8_t ihdr_data[13] = {
		(uint8_t)(widthNet >> 24), (uint8_t)(widthNet >> 16), (uint8_t)(widthNet >> 8), (uint8_t)widthNet,
	    (uint8_t)(heightNet >> 24), (uint8_t)(heightNet >> 16), (uint8_t)(heightNet >> 8), (uint8_t)heightNet,
	    8,  // Bit depth (8 bits)
	    2,  // Color type (RGB)
	    0,  // Compression method (always 0)
	    0,  // Filter method (always 0)
	    0   // Interlace method (0 = no interlacing)
	};
	/*IHDR specs
		Width:              4 bytes
	   	Height:             4 bytes
	   	Bit depth:          1 byte
	   	Color type:         1 byte
	   	Compression method: 1 byte
	   	Filter method:      1 byte
	   	Interlace method:   1 byte
	*/


	//function to calculate CRC, wtf is CRC
    std::vector<uint8_t> ihdr_chunk;
    ihdr_chunk.insert(ihdr_chunk.end(), {'I', 'H', 'D', 'R'});
    ihdr_chunk.insert(ihdr_chunk.end(), std::begin(ihdr_data), std::end(ihdr_data));
    uint32_t ihdr_crc = htonl(calculate_crc(ihdr_chunk));

	uint32_t ihdr_length = htonl(13);
	file.write(reinterpret_cast<char*>(&ihdr_length), 4); //length
	file.write("IHDR", 4); //chunk type
	file.write(reinterpret_cast<char*>(ihdr_data), 13); //chunk data
	file.write(reinterpret_cast<char*>(ihdr_crc), 4); //CRC


	//using zlib for deflate alg and writing idat chunk
	std::vector<uint8_t> uncompressedData;
	for (int y = 0; y < height; ++y) {
	    uncompressedData.push_back(0);
	    for (int x = 0; x < width; ++x) {
	    uncompressedData.push_back(pixel_data[(y * width + x) * 3 + 2]); // Blue
        uncompressedData.push_back(pixel_data[(y * width + x) * 3 + 1]); // Green
        uncompressedData.push_back(pixel_data[(y * width + x) * 3 + 0]); // Red
		}
	}

	uLongf compressedDataSize = compressBound(uncompressedData.size());
	std::vector<uint8_t> compressedData(compressedDataSize);

	int result = compress(compressedData.data(), &compressedDataSize, uncompressedData.data(), uncompressedData.size());
	if (result != Z_OK) {
	    std::cerr << "Compression failed!" << std::endl;
	}

	// Resize the vector to the actual compressed size
	compressedData.resize(compressedDataSize);

	//write idat chunk
	uint32_t compressedDataLength = htonl(compressedData.size());

	// Write IDAT chunk length
	file.write(reinterpret_cast<char*>(&compressedDataLength), 4);

	// Write IDAT chunk type ("IDAT")
	file.write("IDAT", 4);

	// Write compressed image data
	file.write(reinterpret_cast<char*>(compressedData.data()), compressedData.size());

	// Calculate and write IDAT CRC
    std::vector<uint8_t> idat_chunk;
    idat_chunk.insert(idat_chunk.end(), {'I', 'D', 'A', 'T'});
    idat_chunk.insert(idat_chunk.end(), compressedData.begin(), compressedData.end());
    uint32_t idat_crc = htonl(calculate_crc(idat_chunk));
    file.write(reinterpret_cast<char*>(&idat_crc), 4);


	//write IEND chunk
	uint32_t iendLength = 0;
    uint32_t iendCRC = calculate_crc({0x49, 0x45, 0x4E, 0x44});
    file.write(reinterpret_cast<char*>(&iendLength), 4);
    file.write("IEND", 4);
    file.write(reinterpret_cast<char*>(&iendCRC), 4);


    file.close();
}


int main(int argc, char const *argv[])
{
	// Example pixel data for a small image (fill with your real pixel data)
    std::vector<uint8_t> pixelData = {255,  0, 255};
    generateCRCTable();

    int width = 1;
    int height = 1;

    write_png(pixelData, width, height);
	
	return 0;
}