#include <iostream>
#include <fstream>
#include <random>
#include <string>

/*
    Crea un archivo binario de enteros desordenados en la carpeta bin.

    Para usar compilar con: g++ makeUnsortedBin.cpp -o ex/makeUnsorted

    Para crear un archivo: ./makeUnsorted <nombreArchivo> <tamaño en KB>

    IMPORTANTE: El nombre del archivo no debe incluir .bin ni la ruta, solo el nombre.
*/

void makeUnsortedBinFile(const std::string& filename, size_t sizeKB) {
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
}


