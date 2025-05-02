#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>

// Function to read binary data from a file
std::vector<uint8_t> readBinaryFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
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
    const std::string pdfFilePath = "data.pdf";
    const std::string rgbCsvFilePath = "input.csv";
    const std::string outputCsvFilePath = "output.csv";

    // Read binary data from PDF
    std::vector<uint8_t> binaryData = readBinaryFile(pdfFilePath);

    // Read RGB data from CSV
    std::vector<std::vector<int>> rgbData = readRGBFromCSV(rgbCsvFilePath);

    // Embed binary data into RGB data
    embedData(rgbData, binaryData);

    // Write modified RGB data back to CSV
    writeRGBToCSV(outputCsvFilePath, rgbData);

    std::cout << "Data embedding completed. Output written to " << outputCsvFilePath << std::endl;
    return 0;
}