#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <filesystem>


int main() {
	std::uintmax_t size = std::filesystem::file_size("data.bin");
	std::cout<<size<<std::endl;

    std::ifstream infile("data.bin");
    if (!infile) {
        std::cerr << "Error opening input file." << std::endl;
        return -1;
    }

    std::ofstream outfile("decoded.pdf");
    if (!outfile) {
        std::cerr << "Error opening output file." << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(infile, line)) {
        std::bitset<8> bits(line);
        char c = static_cast<char>(bits.to_ulong());
        outfile << c;
    }

    std::cout << "Decoding complete." << std::endl;
    std::uintmax_t sizeF = std::filesystem::file_size("decoded.pdf");
	std::cout<<sizeF<<std::endl;
    return 0;
}
