#include <iostream>
#include <fstream>
#include <string>

/*
Dice si  un archivo binario de enteros está ordenado.

Para usar compilar con: g++ isBinSorted.cpp -o ex/isSorted

Para leer un archivo: ./isSorted <nombreArchivo.bin>
*/

bool isBinaryFileSorted(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: could not open file '" << filename << "' for reading.\n";
        exit(1);
    }

    int64_t prev, current;
    if (!infile.read(reinterpret_cast<char*>(&prev), sizeof(int64_t))) {
        // File is empty or unreadable → consider it sorted
        return true;
    }

    while (infile.read(reinterpret_cast<char*>(&current), sizeof(int64_t))) {
        if (current < prev) {
            return false;  // Not sorted
        }
        prev = current;
    }

    return true;
}


