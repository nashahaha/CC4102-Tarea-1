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

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <binary_file> <fileSizeKB>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string filePath = "../bin/" + filename + ".bin";
    size_t sizeKB = std::stoul(argv[2]);

    size_t numInts = (sizeKB * 1024) / sizeof(int);

    makeUnsortedBinFile(filePath, sizeKB);

    std::cout << "Se creó un archivo desordenado de " << sizeKB << "KB con "<< numInts <<" enteros en la ruta " << filePath <<"\n";
    
    return 0;
}