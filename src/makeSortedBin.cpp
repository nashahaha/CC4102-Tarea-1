#include <iostream>
#include <fstream>
#include <string>

/*
    Crea un archivo binario de enteros ordenado en la carpeta bin.

    Para usar compilar con: g++ makeSortedBin.cpp -o ex/makeSorted

    Para crear un archivo: ./makeSorted <nombreArchivo> <tamaño en KB>

    IMPORTANTE: El nombre del archivo no debe incluir .bin ni la ruta, solo el nombre.
*/

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


