#include <iostream>
#include <chrono>
#include "createarray.cpp"
#include "mergesort.cpp"
#include "quicksort.cpp"
#include "isBinSorted.cpp"

void runExperiment(const std::string& filename, size_t size, int min, int max, int memoryMB, int partitions) {
    // Crear archivo binario con enteros aleatorios
    std::cout << "Creando archivo binario...\n";
    createArray(filename, size, min, max);

    // Verificar si el archivo está desordenado
    std::cout << "Verificando si el archivo está desordenado...\n";
    if (isBinaryFileSorted(filename)) {
        std::cerr << "Error: El archivo ya está ordenado.\n";
        return;
    }

    // Ordenar con MergeSort Externo
    std::cout << "Ordenando con MergeSort Externo...\n";
    auto startMerge = std::chrono::high_resolution_clock::now();
    std::string mergeSortedFile = extMergeSort(filename, memoryMB, partitions);
    auto endMerge = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> mergeDuration = endMerge - startMerge;
    std::cout << "MergeSort completado en " << mergeDuration.count() << " segundos.\n";

    // Verificar si el archivo ordenado por MergeSort está correctamente ordenado
    if (!isBinaryFileSorted(mergeSortedFile)) {
        std::cerr << "Error: El archivo ordenado por MergeSort no está correctamente ordenado.\n";
        return;
    }

    // Ordenar con QuickSort Externo
    std::cout << "Ordenando con QuickSort Externo...\n";
    auto startQuick = std::chrono::high_resolution_clock::now();
    std::string quickSortedFile = extQuickSort(filename, memoryMB, partitions);
    auto endQuick = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> quickDuration = endQuick - startQuick;
    std::cout << "QuickSort completado en " << quickDuration.count() << " segundos.\n";

    // Verificar si el archivo ordenado por QuickSort está correctamente ordenado
    if (!isBinaryFileSorted(quickSortedFile)) {
        std::cerr << "Error: El archivo ordenado por QuickSort no está correctamente ordenado.\n";
        return;
    }

    // Mostrar resultados
    std::cout << "\nResultados del experimento:\n";
    std::cout << "Tamaño del archivo: " << size << " enteros.\n";
    std::cout << "Tiempo de MergeSort: " << mergeDuration.count() << " segundos.\n";
    std::cout << "Tiempo de QuickSort: " << quickDuration.count() << " segundos.\n";
}

int main(){
    runExperiment("../bin/unsorted9.bin", 200000000, -1000000, 1000000, 50, 5);
    return 1;
}
