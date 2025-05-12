#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

// Estructura para representar una partición de un archivo binario
struct Part {
    int id;
    std::string filename;
    std::ifstream* fileStr;
    std::vector<int64_t> buffer;
    int bufferSize;
};

// Declaración de funciones

/**
 * @brief Combina múltiples particiones ordenadas en un único archivo ordenado.
 * @param partitions Vector con los nombres de los archivos de las particiones.
 * @param outputFileName Nombre del archivo de salida ordenado.
 * @return Nombre del archivo ordenado.
 */
std::string mergeFiles(std::vector<std::string> partitions, const std::string& outputFileName);

/**
 * @brief Divide un archivo binario en múltiples particiones.
 * @param filename Nombre del archivo binario de entrada.
 * @param a Número de particiones (aridad).
 * @param M Memoria disponible en MB.
 * @return Vector con los nombres de los archivos de las particiones.
 */
std::vector<std::string> partitionFile(const std::string& filename, int a, int M);

/**
 * @brief Ordena un archivo binario utilizando MergeSort externo.
 * @param filename Nombre del archivo binario de entrada.
 * @param M Memoria disponible en MB.
 * @param a Aridad (número de particiones a combinar simultáneamente).
 * @return Nombre del archivo ordenado.
 */
std::string extMergeSort(const std::string& filename, int M, int a);