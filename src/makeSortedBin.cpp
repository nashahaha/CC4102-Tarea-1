#include <iostream>
#include <fstream>
#include <string>

void generateSortedBinaryFile(const std::string& filename, size_t sizeKB, int start = 0, int step = 1) {
    size_t totalBytes = sizeKB * 1024;
    size_t numIntegers = totalBytes / sizeof(int);

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: could not open file for writing.\n";
        return;
    }

    int current = start;
    for (size_t i = 0; i < numIntegers; ++i) {
        outfile.write(reinterpret_cast<const char*>(&current), sizeof(int));
        current += step;
    }

    outfile.close();
    std::cout << "Sorted file '" << filename << "' created with " << numIntegers << " integers (" << sizeKB << " KB).\n";
}

int main() {
    //generateSortedBinaryFile("sorted1.bin", 4, 392, 12);             // 10 KB sorted file, start at 0, step 1
    //generateSortedBinaryFile("sorted2.bin", 4, 123, 2);
    //generateSortedBinaryFile("sorted2.bin", 5, 100, 2);      // 5 KB file, start at 100, step 2
    generateSortedBinaryFile("sorted3.bin", 1, 3, 2); 
    return 0;
}