#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <string>

size_t B_ = 1024; // Tamaño del bloque (número de enteros)
#define INT_MAX 99999999

// Contadores de accesos a disco, tanto para lectura como escritura
size_t disk_reads = 0;
size_t disk_writes = 0;

std::vector<int64_t> selectPivots(const std::string &filename, int a) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo de entrada para seleccionar pivotes\n";
        exit(1);
    }

    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    size_t numInts = fileSize / sizeof(int64_t);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, numInts - 1);

    size_t randomPos = dist(gen);
    size_t blockPos = (randomPos / B_) * B_ * sizeof(int64_t);

    inputFile.seekg(blockPos, std::ios::beg);
    std::vector<int64_t> buffer(B_);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), B_ * sizeof(int64_t));
    disk_reads++;
    size_t readCount = inputFile.gcount() / sizeof(int64_t);
    buffer.resize(readCount);

    std::sort(buffer.begin(), buffer.end());
    std::vector<int64_t> pivots;

    size_t step = buffer.size() / a;
    for (int i = 1; i < a; ++i) {
        if (i * step < buffer.size()) {
            pivots.push_back(buffer[i * step]);
        } else {
            pivots.push_back(buffer.back());
        }
    }

    std::cout << "Pivotes seleccionados: ";
    for (int64_t p : pivots) std::cout << p << " ";
    std::cout << std::endl;

    inputFile.close();
    return pivots;
}

std::vector<std::string> partitionFileQS(const std::string& archivoOriginal, const std::vector<int64_t>& pivots) {
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

    std::vector<int64_t> buffer(B_);
    while (entrada) {
        entrada.read(reinterpret_cast<char*>(buffer.data()), B_ * sizeof(int64_t));
        disk_reads++;
        size_t leidos = entrada.gcount() / sizeof(int64_t);
        if (leidos == 0) break;

        for (size_t i = 0; i < leidos; ++i) {
            int64_t val = buffer[i];
            size_t partIdx = 0;
            while (partIdx < pivots.size() && val > pivots[partIdx]) partIdx++;
            partFiles[partIdx].write(reinterpret_cast<char*>(&val), sizeof(int64_t));
            disk_writes++;
        }
    }

    for (auto& file : partFiles) file.close();
    entrada.close();

    std::cout << "Archivo particionado en " << particiones.size() << " partes.\n";
    return particiones;
}

std::string concatenateFiles(const std::vector<std::string> &partitions, const std::string &outputFileName) {
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error al abrir el archivo de salida " << outputFileName << "\n";
        exit(1);
    }

    std::vector<char> buffer(B_ * sizeof(int64_t));

    for (const auto &partition : partitions) {
        std::ifstream inputFile(partition, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo de partición " << partition << "\n";
            continue;
        }

        while (inputFile) {
            inputFile.read(buffer.data(), buffer.size());
            std::streamsize bytesRead = inputFile.gcount();
            if (bytesRead > 0) {
                disk_reads++;
                outputFile.write(buffer.data(), bytesRead);
                disk_writes++;
            }
        }

        inputFile.close();
        std::filesystem::remove(partition);
    }

    outputFile.close();
    return outputFileName;
}

std::string extQuickSort(const std::string &filename, int M, int a) {
    std::uintmax_t ram = M * 1000000;
    std::uintmax_t fileSize = std::filesystem::file_size(filename);
    size_t numInts = fileSize / sizeof(int64_t);

    if (fileSize < ram) {
        std::vector<int64_t> buffer(numInts);
        std::fstream inputFile(filename, std::ios::in | std::ios::out | std::ios::binary);
        inputFile.read(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int64_t));
        disk_reads++;
        auto quickSort = [](auto&& self, std::vector<int64_t>& arr, int left, int right) -> void {
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
        inputFile.write(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int64_t));
        disk_writes++;
        inputFile.close();
        return filename;
    }

    std::vector<int64_t> pivots = selectPivots(filename, a);
    std::vector<std::string> partitions = partitionFileQS(filename, pivots);

    std::vector<std::string> sortedPartitions;
    for (const auto &part : partitions) {
        sortedPartitions.push_back(extQuickSort(part, M, a));
    }

    std::string sortedFile = "../bin/sorted_" + std::filesystem::path(filename).filename().string();
    return concatenateFiles(sortedPartitions, sortedFile);
}



