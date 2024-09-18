#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <filesystem>

int main(int argc, char const *argv[])
{
	//int const size = 20;
	std::uintmax_t size = std::filesystem::file_size("data.pdf");
	std::cout<<size<<std::endl;

	std::vector<char> ndata(size);
	
//getting input data.pdf
	std::ifstream infile("data.pdf", std::ios::binary);
    if (!infile.read(ndata.data(), size)) {
        std::cerr << "Error reading file." << std::endl;
        return -1;
    }

//writing to output
    std::ofstream outfile("data.bin");
    if (!outfile) {
        std::cerr << "Error opening output file." << std::endl;
        return -1;
    }
    for (char c : ndata) {
		std::bitset<8> x(c);
		outfile<<x<<std::endl;
		//std::cout << x<<std::endl;
    }

    std::cout << "Encoding complete." << std::endl;
	std::uintmax_t sizeF = std::filesystem::file_size("data.bin");
	std::cout<<sizeF<<std::endl;

	return 0;
}