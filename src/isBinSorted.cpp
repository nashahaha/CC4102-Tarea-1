#include <iostream>
#include <fstream>
#include <string>

/*
Dice si  un archivo binario de enteros está ordenado.

Para usar compilar con: g++ isBinSorted.cpp -o isSorted

Para leer un archivo: ./isSorted <nombreArchivo>.bin
*/

bool isBinaryFileSorted(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: could not open file '" << filename << "' for reading.\n";
        exit(1);
    }

    int prev, current;
    if (!infile.read(reinterpret_cast<char*>(&prev), sizeof(int))) {
        // File is empty or unreadable → consider it sorted
        return true;
    }

    while (infile.read(reinterpret_cast<char*>(&current), sizeof(int))) {
        if (current < prev) {
            return false;  // Not sorted
        }
        prev = current;
    }

    return true;
}

int main(int argc, char* argv[]){

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <binary_file>\n";
        return 1;
    }
    std::string filename = argv[1];

    bool is_sorted = isBinaryFileSorted(filename);
    
    if(is_sorted == false)
        std::cout << filename << " está desordenado\n";
    else
        std::cout << filename << " está ordenado :)\n";

    return is_sorted;
}