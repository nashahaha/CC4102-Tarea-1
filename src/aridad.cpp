#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <cstdint>
#include <random>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include "mergesort.h" // Incluir el encabezado de mergesort

// Tamaño del bloque en bytes (4KB por defecto)
const size_t BLOCK_SIZE = 4096;

// Clase para contar accesos a disco
class DiskAccessCounter {
private:
    size_t readAccesses;
    size_t writeAccesses;

public:
    DiskAccessCounter() : readAccesses(0), writeAccesses(0) {}

    void incrementRead() { readAccesses++; }
    void incrementWrite() { writeAccesses++; }

    size_t getReadAccesses() const { return readAccesses; }
    size_t getWriteAccesses() const { return writeAccesses; }
    size_t getTotalAccesses() const { return readAccesses + writeAccesses; }

    void reset() { readAccesses = 0; writeAccesses = 0; }
};

// Genera un archivo binario con N enteros desordenados
void generateTestFile(const std::string& filename, size_t numIntegers) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: no se pudo crear el archivo " << filename << "\n";
        return;
    }

    // Inicializar generador de números aleatorios
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int64_t> dist(std::numeric_limits<int64_t>::min(),
                                                std::numeric_limits<int64_t>::max());

    for (size_t i = 0; i < numIntegers; ++i) {
        int64_t value = dist(gen);
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(int64_t));
    }

    outfile.close();
    std::cout << "Archivo de prueba generado: " << filename << " con " << numIntegers << " enteros.\n";
}

// Calcula el número de enteros que caben en un bloque de tamaño B
size_t calculateIntegersPerBlock(size_t blockSize) {
    return blockSize / sizeof(int64_t);
}

// Evalúa una aridad específica y devuelve el número de accesos a disco
size_t evaluateArity(const std::string& filename, int arity, int memoryMB) {
    // Crear una copia temporal del archivo de entrada para no modificarlo
    std::string tempInput = "temp_input_" + std::to_string(arity) + ".bin";
    std::ifstream src(filename, std::ios::binary);
    std::ofstream dst(tempInput, std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();

    // Ejecutar Mergesort externo y medir accesos a disco
    DiskAccessCounter counter;
    std::string sortedFile = extMergeSort(tempInput, memoryMB, arity); // Usar la implementación real

    // Limpiar archivos temporales
    std::remove(tempInput.c_str());
    std::remove(sortedFile.c_str());

    return counter.getTotalAccesses();
}

// Búsqueda ternaria para encontrar la aridad óptima
int ternarySearchOptimalArity(const std::string& filename, int low, int high, int memoryMB) {
    const int MIN_INTERVAL_SIZE = 5; // Cuando el intervalo es pequeño, evaluamos todos los valores

    while (high - low > MIN_INTERVAL_SIZE) {
        int m1 = low + (high - low) / 3;
        int m2 = high - (high - low) / 3;

        std::cout << "Evaluando aridades: " << m1 << " y " << m2 << "\n";

        size_t accesses_m1 = evaluateArity(filename, m1, memoryMB);
        size_t accesses_m2 = evaluateArity(filename, m2, memoryMB);

        std::cout << "Accesos para a=" << m1 << ": " << accesses_m1
                  << ", a=" << m2 << ": " << accesses_m2 << "\n";

        if (accesses_m1 < accesses_m2) {
            high = m2 - 1;
        } else {
            low = m1 + 1;
        }
    }

    // Evaluar todos los valores en el intervalo pequeño final
    std::cout << "Evaluando valores finales entre " << low << " y " << high << "\n";

    int bestArity = low;
    size_t minAccesses = evaluateArity(filename, low, memoryMB);

    for (int a = low + 1; a <= high; ++a) {
        size_t currentAccesses = evaluateArity(filename, a, memoryMB);
        if (currentAccesses < minAccesses) {
            minAccesses = currentAccesses;
            bestArity = a;
        }
    }

    return bestArity;
}

int main() {
    const std::string testFilename = "../bin/test_60M.bin";
    const size_t numIntegers = 60000000; // 60M enteros
    const int memoryMB = 50;            // Memoria disponible en MB

    // Generar archivo de prueba (solo si no existe)
    if (!std::filesystem::exists(testFilename)) {
        generateTestFile(testFilename, numIntegers);
    } else {
        std::cout << "El archivo de prueba ya existe, se usará el existente.\n";
    }

    // Calcular el número de enteros que caben en un bloque
    size_t integersPerBlock = calculateIntegersPerBlock(BLOCK_SIZE);
    int maxArity = integersPerBlock;
    std::cout << "Enteros por bloque: " << integersPerBlock << "\n";
    std::cout << "Rango de búsqueda para a: [2, " << maxArity << "]\n";

    // Encontrar la aridad óptima usando búsqueda ternaria
    std::cout << "\nIniciando búsqueda ternaria para encontrar la aridad óptima...\n";
    int optimalA = ternarySearchOptimalArity(testFilename, 2, maxArity, memoryMB);

    // Evaluar la aridad óptima para confirmar
    size_t finalAccesses = evaluateArity(testFilename, optimalA, memoryMB);
    std::cout << "\nLa aridad óptima encontrada es: " << optimalA
              << " con " << finalAccesses << " accesos a disco\n";

    return 0;
}