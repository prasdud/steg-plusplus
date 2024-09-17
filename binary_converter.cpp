#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <filesystem>

int main(int argc, char const *argv[])
{
	//int const size = 20;
	std::uintmax_t size = std::filesystem::file_size("data.txt");
	std::cout<<size<<std::endl;

	std::vector<char> ndata(size);
	std::ifstream myfile;
	myfile.open("data.txt");

    if (!myfile) {
        std::cerr << "Error opening file." << std::endl;
        return -1;
    }

    myfile.read(ndata.data(), size);

    if (!myfile) {
        std::cerr << "Error reading file." << std::endl;
        return -1;
    }

	myfile.close();
	for (char c : ndata) {
		std::bitset<8> x(c);
        std::cout << x<<std::endl;
    }

    std::cout << std::endl;
	return 0;
}