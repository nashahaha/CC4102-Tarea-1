#include <iostream>
#include <fstream>
#include <random>
#include <string>

void makeBinaryFile(const std::string& filename, size_t sizeKB) {
    size_t totalBytes = sizeKB * 1024;
    size_t numIntegers = totalBytes / sizeof(int);

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: could not open file for writing.\n";
        return;
    }

    std::random_device rd;   // seed
    std::mt19937 gen(rd());  // Mersenne Twister RNG
    std::uniform_int_distribution<int> dist(-1000000, 1000000);  // range of random ints

    for (size_t i = 0; i < numIntegers; ++i) {
        int n = dist(gen);
        outfile.write(reinterpret_cast<const char*>(&n), sizeof(int));
    }

    outfile.close();
    std::cout << "File '" << filename << "' created with " << sizeKB << " KB of random integers.\n";
}

int main() {
    makeBinaryFile("unsorted.bin", 10);  // 10 KB file
    return 0;
}