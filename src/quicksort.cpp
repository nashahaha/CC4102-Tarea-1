#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <string>

size_t B_ = 1024; // Se asume que el bloque es de tamaño 4KB, por ahora
#define INT_MAX 99999999


/**
 * @brief Selecciona pivotes aleatorios para el particionamiento del Quicksort externo.
 *
 * Esta función selecciona un bloque aleatorio del archivo de entrada, ordena los valores,
 * y elige pivotes equiespaciados a partir del bloque para dividir el archivo original.
 *
 * @param filename Nombre del archivo binario a analizar.
 * @param a Número de particiones que se desean generar.
 * 
 * @return Un vector con los pivotes seleccionados.
 * 
 * @throws Termina la ejecución si no se puede abrir el archivo de entrada.
 */
std::vector<int> selectPivots(const std::string &filename, int a) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo de entrada para seleccionar pivotes\n";
        exit(1);
    }

    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    size_t numInts = fileSize / sizeof(int);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, numInts - 1);

    size_t randomPos = dist(gen);
    size_t blockPos = (randomPos / B_) * B_ * sizeof(int);

    inputFile.seekg(blockPos, std::ios::beg);
    std::vector<int> buffer(B_);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), B_ * sizeof(int));
    size_t readCount = inputFile.gcount() / sizeof(int);
    buffer.resize(readCount);

    std::sort(buffer.begin(), buffer.end());
    std::vector<int> pivots;

    size_t step = buffer.size() / a;
    for (int i = 1; i < a; ++i) {
        if (i * step < buffer.size()) {
            pivots.push_back(buffer[i * step]);
        } else {
            pivots.push_back(buffer.back());
        }
    }

    std::cout << "Pivotes seleccionados: ";
    for (int p : pivots) std::cout << p << " ";
    std::cout << std::endl;

    inputFile.close();
    return pivots;
}

/**
 * @brief Particiona un archivo binario de enteros usando un conjunto de pivotes.
 *
 * Esta función lee bloques del archivo original y los distribuye en distintas 
 * particiones según el valor de cada entero comparado con los pivotes.
 *
 * @param archivoOriginal Nombre del archivo a particionar.
 * @param pivots Vector con los valores pivotes que delimitan cada partición.
 * 
 * @return Un vector con los nombres de los archivos de partición generados.
 * 
 * @throws Termina la ejecución si no se puede abrir el archivo de entrada o de salida.
 */
std::vector<std::string> partitionFileQS(const std::string& archivoOriginal, const std::vector<int>& pivots) {
    std::ifstream entrada(archivoOriginal, std::ios::binary);
    std::vector<std::string> particiones(pivots.size() + 1);

    std::vector<std::ofstream> partFiles(pivots.size() + 1);
    std::string baseName = std::filesystem::path(archivoOriginal).stem().string();

    for (size_t i = 0; i <= pivots.size(); ++i) {
        particiones[i] = "../bin/" + baseName + "_part_" + std::to_string(i) + ".bin";
        partFiles[i].open(particiones[i], std::ios::binary);
    }

    if (!entrada) {
        std::cerr << "No se pudo abrir el archivo para particionar\n";
        exit(1);
    }

    std::vector<int> buffer(B_);
    while (entrada) {
        entrada.read(reinterpret_cast<char*>(buffer.data()), B_ * sizeof(int));
        size_t leidos = entrada.gcount() / sizeof(int);
        if (leidos == 0) break;

        for (size_t i = 0; i < leidos; ++i) {
            int val = buffer[i];
            size_t partIdx = 0;
            while (partIdx < pivots.size() && val > pivots[partIdx]) partIdx++;
            partFiles[partIdx].write(reinterpret_cast<char*>(&val), sizeof(int));
        }
    }

    for (auto& file : partFiles) file.close();
    entrada.close();

    std::cout << "Archivo particionado en " << particiones.size() << " partes.\n";
    return particiones;
}

/**
 * @brief Concatena múltiples archivos binarios ordenados en un único archivo de salida.
 * 
 * Esta función simplemente concatena las particiones
 * asumidas como ya ordenadas (por lo visto en Quicksort), ya que
 * los pivotes garantizaron el orden entre bloques.
 *
 * @param partitions Vector con los nombres de los archivos de entrada.
 * @param outputFileName Nombre del archivo de salida resultante.
 * 
 * @return Nombre del archivo generado.
 * 
 * @throws Termina la ejecución si no se puede abrir algún archivo.
 */
std::string concatenateFiles(const std::vector<std::string> &partitions, const std::string &outputFileName) {
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error al abrir el archivo de salida " << outputFileName << "\n";
        exit(1);
    }

    std::vector<char> buffer(B_ * sizeof(int));

    for (const auto &partition : partitions) {
        std::ifstream inputFile(partition, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo de partición " << partition << "\n";
            continue;
        }

        while (inputFile) {
            inputFile.read(buffer.data(), buffer.size());
            std::streamsize bytesRead = inputFile.gcount();
            if (bytesRead > 0) outputFile.write(buffer.data(), bytesRead);
        }

        inputFile.close();
        std::filesystem::remove(partition); // Eliminar archivo temporal
    }

    outputFile.close();
    return outputFileName;
}

/**
 * @brief Ordena un archivo binario de enteros utilizando QuickSort externo.
 *
 * Si el archivo es lo suficientemente pequeño para caber en memoria, se ordena 
 * completamente en RAM usando una implementación recursiva de QuickSort. 
 * Si es más grande que la RAM disponible, se seleccionan los pivotes aleatorios, 
 * se particiona el archivo y se aplica QuickSort recursivamente sobre cada partición.
 *
 * Finalmente, se concatenan los archivos ya ordenados en un único archivo de salida.
 *
 * @param filename Nombre del archivo a ordenar.
 * @param M Tamaño de la memoria RAM disponible (en MB).
 * @param a Aridad del algoritmo de ordenamiento externo.
 * 
 * @return Nombre del archivo binario resultante con los datos ordenados.
 * 
 * @throws Termina la ejecución si no se puede abrir el archivo de entrada o escritura.
 */
std::string extQuickSort(const std::string &filename, int M, int a) {
    std::uintmax_t ram = M * 1000000;
    std::uintmax_t fileSize = std::filesystem::file_size(filename);
    size_t numInts = fileSize / sizeof(int);

    std::cout << "\n Procesando archivo: " << filename 
              << " (" << fileSize / (1024 * 1024) << " MB)\n";
    // ----------------------------------------------------------------------------------------
    // Si el tamaño del archivo es menor al de la memoria RAM, se ordena todo en memoria principal
    // ----------------------------------------------------------------------------------------
    if (fileSize < ram) {
        std::vector<int> buffer(numInts);
        std::fstream inputFile(filename, std::ios::in | std::ios::out | std::ios::binary);
        inputFile.read(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int));

        auto quickSort = [](auto&& self, std::vector<int>& arr, int left, int right) -> void {
            if (left >= right) return;
            int pivot = arr[(left + right) / 2];
            int i = left, j = right;
            while (i <= j) {
                while (arr[i] < pivot) ++i;
                while (arr[j] > pivot) --j;
                if (i <= j) std::swap(arr[i++], arr[j--]);
            }
            self(self, arr, left, j);
            self(self, arr, i, right);
        };

        quickSort(quickSort, buffer, 0, numInts - 1);
        inputFile.seekp(0);
        inputFile.write(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int));
        inputFile.close();
        return filename;
    }

    std::vector<int> pivots = selectPivots(filename, a);
    std::vector<std::string> partitions = partitionFileQS(filename, pivots);

    std::vector<std::string> sortedPartitions;
    for (const auto &part : partitions) {
        sortedPartitions.push_back(extQuickSort(part, M, a));
    }

    std::string sortedFile = "../bin/sorted_" + std::filesystem::path(filename).filename().string();
    return concatenateFiles(sortedPartitions, sortedFile);
}



