#include <iostream>
#include <chrono>
#include "createarray.cpp"
#include "mergesort.cpp"
#include "quicksort.cpp"
#include "isBinSorted.cpp"

void runExperiment(const std::string& filename, size_t size, int64_t min, int64_t max, int memoryMB, int partitions) {
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

    std::uintmax_t fileSize = std::filesystem::file_size(filename);
    std::uintmax_t sizeMB = fileSize / 1'000'000;

    // Mostrar resultados
    std::cout << "\n-------------------------Resultados del experimento:-------------------------\n";
    std::cout <<   "|   Tamaño del archivo: " << sizeMB << " MB con "<< size << " enteros.       \n";
    std::cout <<   "|   Tiempo de MergeSort: " << mergeDuration.count() << " segundos.           \n";
    std::cout <<   "|   Lecturas MergeSort: " << disk_reads_merge << "\n";
    std::cout <<   "|   Escrituras MergeSort: " << disk_writes_merge << "\n";
    std::cout <<   "|   Tiempo de QuickSort: " << quickDuration.count() << " segundos.           \n";
    std::cout <<   "|   Lecturas QuickSort: " << disk_reads << "\n";
    std::cout <<   "|   Escrituras QuickSort: " << disk_writes << "\n";
    std::cout <<   "-----------------------------------------------------------------------------\n";

    // Guardar resumen en archivo de texto (modo append)
    std::ofstream summary("resumen_experimentos.txt", std::ios::app);
    if (summary) {
        summary << "----- Resultado del experimento -----\n";
        summary << "Archivo original: " << filename << "\n";
        summary << "Tamaño: " << sizeMB << " MB (" << size << " enteros)\n";
        summary << "MergeSort:\n";
        summary << "  Tiempo: " << mergeDuration.count() << " s\n";
        summary << "  Lecturas: " << disk_reads_merge << "\n";
        summary << "  Escrituras: " << disk_writes_merge << "\n";
        summary << "QuickSort:\n";
        summary << "  Tiempo: " << quickDuration.count() << " s\n";
        summary << "  Lecturas: " << disk_reads << "\n";
        summary << "  Escrituras: " << disk_writes << "\n";
        summary << "--------------------------------------\n\n";
        summary.close();
    } else {
        std::cerr << "No se pudo escribir el resumen en 'resumen_experimentos.txt'\n";
    }
}


int main(){
    //primer experimento con 25.000.000 de enteros = 200 MB = 4M
    //runExperiment("../bin/unsorted9.bin", 25000000, -6000000, 1000000, 50, 50);

    //segundo experimento con 50.000.000 de enteros = 400 MB = 8M
    //runExperiment("../bin/unsorted10.bin", 50000000, -6000000, 1000000, 50, 50);


    for(int i=1; i<=15; i++){
        for (int j=0; j<5; j++){ // se ejecuta 5 veces por cada tamaño de archivo
            std::string file = "../bin/unsorted_test" + std::to_string(i) + "_" + std::to_string(j) + ".bin";
            runExperiment(file, 25000000*i, -25000000, 25000000, 50, 50);
        }
    }


    return 1;
}
