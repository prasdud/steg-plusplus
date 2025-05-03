#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>

// Function to read binary data from a file
std::vector<uint8_t> readBinaryFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    std::cout<< "File read, returning to main..."<< std::endl;
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

// Function to read RGB data from a CSV file
std::vector<std::vector<int>> readRGBFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    std::vector<std::vector<int>> rgbData;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> rgb;
        size_t pos = 0;
        while ((pos = line.find(',')) != std::string::npos) {
            rgb.push_back(std::stoi(line.substr(0, pos)));
            line.erase(0, pos + 1);
        }
        rgb.push_back(std::stoi(line));
        rgbData.push_back(rgb);
    }
    return rgbData;
}

// Function to embed binary data into RGB data using LSB
void embedData(std::vector<std::vector<int>>& rgbData, const std::vector<uint8_t>& binaryData) {
    size_t dataIndex = 0, bitIndex = 0;

    for (auto& pixel : rgbData) {
        for (int& color : pixel) {
            if (dataIndex >= binaryData.size()) return;

            // Get the current bit from the binary data
            uint8_t bit = (binaryData[dataIndex] >> (7 - bitIndex)) & 1;

            // Embed the bit into the LSB of the color
            color = (color & ~1) | bit;

            // Move to the next bit
            if (++bitIndex == 8) {
                bitIndex = 0;
                ++dataIndex;
            }
        }
    }
}

// Function to write RGB data back to a CSV file
void writeRGBToCSV(const std::string& filePath, const std::vector<std::vector<int>>& rgbData) {
    std::ofstream file(filePath);
    for (const auto& pixel : rgbData) {
        for (size_t i = 0; i < pixel.size(); ++i) {
            file << pixel[i];
            if (i < pixel.size() - 1) file << ",";
        }
        file << "\n";
    }
}

int main() {
    // File paths
    const std::string binFilePath = "../../assets/output/data.bin";
    const std::string rgbCsvFilePath = "../../assets/output/image_dat.csv";
    const std::string outputCsvFilePath = "../../assets/output/output.csv";

    // Read binary data from PDF
    std::cout<< "Reading binary data..."<< std::endl;
    std::vector<uint8_t> binaryData = readBinaryFile(binFilePath);
    std::cout<< "Binary data read."<< std::endl;

    // Read RGB data from CSV
    std::cout<< "Reading RGB data..."<< std::endl;
    std::vector<std::vector<int>> rgbData = readRGBFromCSV(rgbCsvFilePath);
    std::cout<< "RGB data read."<< std::endl;

    // Embed binary data into RGB data
    std::cout<< "Embedding data..."<< std::endl;
    embedData(rgbData, binaryData);
    std::cout<< "Embedding done."<< std::endl;


    // Write modified RGB data back to CSV
    std::cout<< "Writing output RGB..."<< std::endl;
    writeRGBToCSV(outputCsvFilePath, rgbData);
    std::cout<< "Output RGB written."<< std::endl;

    std::cout << "Data embedding completed. Output written to " << outputCsvFilePath << std::endl;
    return 0;
}