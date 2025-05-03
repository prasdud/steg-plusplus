#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <filesystem>


/**
 * @author prasdud
 * 
 * @brief converts a pdf file to its binary equivalent
 * 
 * @while 
 * 
 * @return int 
 */


int main(int argc, char const *argv[])
{
	//int const size = 20;
	std::uintmax_t size = std::filesystem::file_size("../../assets/data.pdf");
	std::cout<<"Size of PDF file in bytes = "<<size<<std::endl;

	std::vector<char> ndata(size);
	
//getting input data.pdf
	std::ifstream infile("../../assets/data.pdf", std::ios::binary);
    if (!infile.read(ndata.data(), size)) {
        std::cerr << "Error reading file." << std::endl;
        return -1;
    }

//writing to output
    std::ofstream outfile("../../assets/output/data.bin");
    if (!outfile) {
        std::cerr << "Error opening output file." << std::endl;
        return -1;
    }

// below snippet writes text, not raw binary data so replaced

    // for (char c : ndata) {
	// 	std::bitset<8> x(c);
	// 	outfile<<x<<std::endl;
	// 	//std::cout << x<<std::endl;
    // }

    outfile.write(ndata.data(), ndata.size());

    std::cout << "Encoding complete." << std::endl;
	std::uintmax_t sizeF = std::filesystem::file_size("../../assets/output/data.bin");
	std::cout<<"size of binary file in bytes = "<<sizeF<<std::endl;

	return 0;
}