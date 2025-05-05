#include <vector>
#include <iostream>
#include <fstream>

/*
    Imprime el contenido de un archivo binario de enteros en la consola.

    Para usar compilar con: g++ readBinFile.cpp -o ex/readBinFile

    Para leer un archivo: ./readBinFile <nombreArchivo.bin>
*/

void readBinFile(const std::string &filename){
    std::ifstream inputFile(filename, std::ios::binary); // abre archivo 1
    
    if (!inputFile) {
        std::cerr << "Error: could not open file '" << filename << "' for reading.\n";
        exit(1);
    }

    // Get file size
    inputFile.seekg(0, std::ios::end);
    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Determine number of ints
    size_t numInts = fileSize / sizeof(int);
    std::cout << "Este archivo tiene " << numInts << " enteros\n";
    
    std::vector<int> buffer(numInts);

    // Read data
    if (!inputFile.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "Error reading file\n";
        return;
    }

    // Print contents
    for (int val : buffer) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <binary_file.bin>\n";
        return 1;
    }
    
    std::string filename = argv[1];
    readBinFile(filename);

    return 0;
}