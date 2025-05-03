#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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

// Function to extract binary data from RGB data using LSB
std::vector<uint8_t> extractData(const std::vector<std::vector<int>>& rgbData, size_t expectedBytes) {
    std::vector<uint8_t> binaryData;
    uint8_t currentByte = 0;
    int bitCount = 0;

    for (const auto& pixel : rgbData) {
        for (int color : pixel) {
            // Extract the LSB
            currentByte = (currentByte << 1) | (color & 1);
            ++bitCount;

            if (bitCount == 8) {
                binaryData.push_back(currentByte);
                currentByte = 0;
                bitCount = 0;

                if (binaryData.size() >= expectedBytes) {
                    return binaryData;
                }
            }
        }
    }

    return binaryData; // In case expectedBytes is too high, return whatever was extracted
}

// Function to write binary data to a file
void writeBinaryFile(const std::string& filePath, const std::vector<uint8_t>& data) {
    std::ofstream file(filePath, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

int main() {
    const std::string inputCsvFilePath = "../../assets/output/output_image_dat.csv";
    const std::string outputBinFilePath = "../../assets/output/extracted.bin";

    std::cout << "Reading stego RGB CSV..." << std::endl;
    std::vector<std::vector<int>> rgbData = readRGBFromCSV(inputCsvFilePath);
    std::cout << "RGB CSV loaded." << std::endl;

    size_t expectedBytes = 18810; // size of pdf (expected)

    std::cout << "Extracting binary data..." << std::endl;
    std::vector<uint8_t> binaryData = extractData(rgbData, expectedBytes);
    std::cout << "Binary data extracted." << std::endl;

    std::cout << "Writing to output file..." << std::endl;
    writeBinaryFile(outputBinFilePath, binaryData);
    std::cout << "Output binary written to " << outputBinFilePath << std::endl;

    return 0;
}
