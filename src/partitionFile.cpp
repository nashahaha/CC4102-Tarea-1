#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <string>
std::vector<std::string> partitionFile(const std::string& filename, int a, int M) {
    std::ifstream entrada(filename, std::ios::binary | std::ios::ate);
    std::vector<std::string> particiones;

    if (!entrada) {
        std::cerr << "No se pudo abrir el archivo de entrada\n";
        exit(1);
    }

    std::uintmax_t tamanoBytes = std::filesystem::file_size(filename);
    entrada.seekg(0, std::ios::beg);


    size_t totalInts = tamanoBytes / sizeof(int);

    // Cantidad de ints por parte, con reparto del sobrante
    size_t intsPorParte = totalInts / a;
    size_t resto = totalInts % a;

    // Cantidad máxima de ints que caben en M megabytes
    size_t maxIntsEnMemoria = (M * 1024 * 1024) / sizeof(int);
    if (maxIntsEnMemoria == 0) {
        std::cerr << "Memoria disponible demasiado pequeña para un int\n";
        exit(1);
    }

    std::string baseName = std::filesystem::path(filename).stem().string();

    for (int i = 0; i < a; ++i) {
        size_t cantidad = intsPorParte + (i < resto ? 1 : 0);
        std::string nombreParte = "../bin/" + baseName + "_parte_" + std::to_string(i) + ".bin";
        std::ofstream salida(nombreParte, std::ios::binary);

        if (!salida) {
            std::cerr << "No se pudo crear " << nombreParte << "\n";
            break;
        }

        while (cantidad > 0) {
            size_t bloque = std::min(cantidad, maxIntsEnMemoria);
            std::vector<int> buffer(bloque);

            entrada.read(reinterpret_cast<char*>(buffer.data()), bloque * sizeof(int));
            std::streamsize leidos = entrada.gcount() / sizeof(int);

            if (leidos == 0) break;

            salida.write(reinterpret_cast<char*>(buffer.data()), leidos * sizeof(int));
            cantidad -= leidos;
        }

        salida.close();
        particiones.push_back(nombreParte);
    }

    entrada.close();
    return particiones;
}


int main(int argc, char* argv[]){

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <binary_file.bin> <partitionNumber>\n";
        return 1;
    }

    std::string filename = argv[1];
    size_t n = std::stoul(argv[2]);

    std::vector<std::string> part = partitionFile(filename, n, 1);
    std::cout << "Se crearon las particiones:\n";
    for (auto val : part) {
        std::cout << val << "\n";
    }
    
    return 0;
}