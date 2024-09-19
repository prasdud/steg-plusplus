#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <zlib>

uint32_t calculateCRC(const std::vector<uint8_t>& data) {
    
    return 0;
}

void write_png(const std::vector<uint8_t> &pixel_data, int width, int height){
	std::ofstream file("output.png", std::ios::binary);

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
	uint32_t ihdr_crc = calulate_crc();

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
	        uncompressedData.push_back(blueComponent[y * width + x]);
	        uncompressedData.push_back(greenComponent[y * width + x]);
	        uncompressedData.push_back(redComponent[y * width + x]);
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


}


int main(int argc, char const *argv[])
{
	
	return 0;
}