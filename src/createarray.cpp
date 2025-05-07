#include <iostream>
#include <fstream>
#include <random>
#include <string>

/**
 * @brief Crea un archivo binario con un arreglo de enteros aleatorios.
 * 
 * @param filename Nombre del archivo binario a crear.
 * @param size Número de enteros a generar.
 * @param min Valor mínimo de los enteros generados.
 * @param max Valor máximo de los enteros generados.
 */
void createArray(const std::string& filename, size_t size, int min, int max) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: no se pudo crear el archivo " << filename << "\n";
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);

    for (size_t i = 0; i < size; ++i) {
        int value = dist(gen);
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(int));
    }

    outfile.close();
    std::cout << "Archivo creado: " << filename << " con " << size << " enteros.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Uso: " << argv[0] << " <nombreArchivo> <tamaño> <min> <max>\n";
        return 1;
    }

    std::string filename = argv[1];
    size_t size = std::stoul(argv[2]);
    int min = std::stoi(argv[3]);
    int max = std::stoi(argv[4]);

    createArray(filename, size, min, max);

    return 0;
}