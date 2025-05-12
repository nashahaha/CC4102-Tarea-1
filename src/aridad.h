#pragma once

#include <string>
#include <cstddef>

/**
 * @brief Clase para contar accesos a disco.
 */
class DiskAccessCounter {
private:
    size_t readAccesses;
    size_t writeAccesses;

public:
    DiskAccessCounter();
    void incrementRead();
    void incrementWrite();
    size_t getReadAccesses() const;
    size_t getWriteAccesses() const;
    size_t getTotalAccesses() const;
    void reset();
};

/**
 * @brief Genera un archivo binario con N enteros desordenados.
 * @param filename Nombre del archivo a generar.
 * @param numIntegers Número de enteros a escribir en el archivo.
 */
void generateTestFile(const std::string& filename, size_t numIntegers);

/**
 * @brief Evalúa una aridad específica y devuelve el número de accesos a disco.
 * @param filename Nombre del archivo binario de entrada.
 * @param arity Aridad a evaluar.
 * @param memoryMB Memoria disponible en MB.
 * @return Número de accesos a disco.
 */
size_t evaluateArity(const std::string& filename, int arity, int memoryMB);

/**
 * @brief Realiza una búsqueda ternaria para encontrar la aridad óptima.
 * @param filename Nombre del archivo binario de entrada.
 * @param low Límite inferior del rango de búsqueda.
 * @param high Límite superior del rango de búsqueda.
 * @param memoryMB Memoria disponible en MB.
 * @return La aridad óptima encontrada.
 */
int ternarySearchOptimalArity(const std::string& filename, int low, int high, int memoryMB);