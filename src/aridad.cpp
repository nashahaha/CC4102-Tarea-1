#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <cstdint>
#include "mergesort.cpp" // Asegúrate de que extMergeSort esté implementado

// Tamaño del bloque en bytes (4KB por defecto)
const size_t BLOCK_SIZE = 4096;

// Genera un archivo binario con 60M enteros desordenados
void generateTestFile(const std::string& filename, size_t numIntegers) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: no se pudo crear el archivo " << filename << "\n";
        return;
    }

    // Inicializar generador de números aleatorios
    std::mt19937 gen(std::random_device{}()); // Generador Mersenne Twister con semilla
    std::uniform_int_distribution<int64_t> dist(-1000000, 1000000); // Rango ajustado

    for (size_t i = 0; i < numIntegers; ++i) {
        int64_t value = dist(gen); // Generar número aleatorio
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(int64_t));
    }

    outfile.close();
    std::cout << "Archivo de prueba generado: " << filename << " con " << numIntegers << " enteros.\n";
}

// Calcula el número de enteros que caben en un bloque de tamaño B
size_t calculateIntegersPerBlock(size_t blockSize) {
    return blockSize / sizeof(int64_t);
}

// Realiza una búsqueda binaria para encontrar la aridad óptima
int findOptimalArity(const std::string& filename, int minA, int maxA, int memoryMB) {
    if (minA > maxA) {
        std::cerr << "Error: minA no puede ser mayor que maxA.\n";
        return -1;
    }

    int optimalA = minA;
    size_t minDiskAccesses = std::numeric_limits<size_t>::max();

    for (int a = minA; a <= maxA; ++a) {
        std::cout << "Probando aridad a = " << a << "...\n";

        // Ejecutar MergeSort externo con la aridad actual
        std::string sortedFile = extMergeSort(filename, memoryMB, a);

        // Aquí deberías medir los accesos a disco (esto depende de cómo lo implementes en extMergeSort)
        size_t diskAccesses = /* función para medir accesos a disco */ 0;

        std::cout << "Accesos a disco para a = " << a << ": " << diskAccesses << "\n";

        // Actualizar el valor óptimo si se encuentra un mejor resultado
        if (diskAccesses < minDiskAccesses) {
            minDiskAccesses = diskAccesses;
            optimalA = a;
        }
    }

    return optimalA;
}

int main() {
    const std::string testFilename = "../bin/test_60M.bin";
    const size_t numIntegers = 60000000; // 60M enteros
    const int memoryMB = 50;            // Memoria disponible en MB

    // Generar archivo de prueba
    generateTestFile(testFilename, numIntegers);

    // Calcular el número de enteros que caben en un bloque
    size_t integersPerBlock = calculateIntegersPerBlock(BLOCK_SIZE);
    std::cout << "Enteros por bloque: " << integersPerBlock << "\n";

    // Encontrar la aridad óptima
    int optimalA = findOptimalArity(testFilename, 2, integersPerBlock, memoryMB);
    std::cout << "La aridad óptima es: " << optimalA << "\n";

    return 0;
}