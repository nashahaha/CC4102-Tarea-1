#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

size_t B = 1024; // Se asume que el bloque es de tamaño 4KB
int INT_MAX = 99999999;


/**
 * @brief Representa una partición de un archivo binario de enteros.
 *
 * Esta estructura almacena la información necesaria para manejar una partición 
 * individual de un archivo durante el proceso de merge.
 *
 * Campos:
 * - id: Identificador único de la partición (útil para depuración).
 * - filename: Nombre del archivo asociado a esta partición.
 * - fileStr: Puntero al flujo de entrada del archivo.
 * - buffer: Vector de enteros usado como buffer de lectura.
 * - bufferSize: Cantidad de elementos actualmente almacenados en el buffer.
 */
struct Part {
    int id;
    std::string filename;
    std::ifstream* fileStr;
    std::vector<int> buffer;
    int bufferSize;
  };



/**
 * @brief Combina múltiples archivos binarios ordenados en un único archivo ordenado.
 * 
 * Esta función realiza un merge de archivos binarios de enteros previamente ordenados 
 * de forma ascendente. El resultado es un nuevo archivo binario que mantiene el orden.
 * 
 * Se asume que todos los archivos de entrada ya están ordenados.
 * 
 * Una vez completada la combinación, los archivos de entrada son eliminados.
 * 
 * @param partitions Vector con los nombres de los archivos de entrada a combinar.
 * @param outputFileName Nombre del archivo de salida donde se guardará el resultado.
 * 
 * @return Nombre del archivo de salida generado.
 * 
 * @throws Termina la ejecución si no se puede abrir alguno de los archivos de entrada.
 */

std::string mergeFiles(std::vector<std::string> partitions, const std::string &outputFileName){

    std::vector<Part> parts;

    // ----------------------------------------------------------------------------------------
    // Se inicializan todos los archivos por leer en la estructura Part
    // ----------------------------------------------------------------------------------------
    for (int i = 0; i<partitions.size(); i++) { 
        Part p;
        p.id = i;
        p.filename = partitions[i];

        p.fileStr = new std::ifstream();
        p.fileStr->open(p.filename, std::ios::binary);
         
        
        if (!p.fileStr->is_open()) {
            std::cerr << "Error al abrir el archivo " << p.filename <<"\n";
            std::exit(1);
        }

        std::vector<int> buff(B); 
        p.buffer = buff;
        p.bufferSize = 0;
        parts.push_back(p);
    }

    std::vector<Part> partsToRead = parts; // Arreglo con todos los archivos que aún tienen números por leer
    
    std::vector<int> outputBuffer;
    std::ofstream outputFile(outputFileName, std::ios::binary); // Se abre el archivo de salida 
    int outBuffSize=0;  // Tamaño del buffer de salida

    // ----------------------------------------------------------------------------------------
    // Mientras quede por leer más de un archivo se comparan entre ellos para obtener el mínimo
    // ----------------------------------------------------------------------------------------
    while(partsToRead.size()>1){ 

        for ( int i = 0; i<partsToRead.size(); i++){ // Se llenan todos los buffer vacios
            Part &p = partsToRead[i];

            if (p.bufferSize == 0 && p.fileStr->peek()!=EOF) {
                p.buffer.resize(B);
                p.fileStr->read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
                std::streamsize bytesRead1 = p.fileStr->gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
                p.bufferSize = bytesRead1 / sizeof(int);
                p.buffer.resize(p.bufferSize);
            }
        }

        // ----------------------------------------------------------------------------------------
        // Si se terminó de leer el archivo se quita de las comparaciones
        // ----------------------------------------------------------------------------------------
        for (auto it = partsToRead.begin(); it != partsToRead.end(); ) {
            if (it->bufferSize == 0 && it->fileStr->peek()==EOF) 
                it = partsToRead.erase(it);
            else
                it++;
            
        }

        // ----------------------------------------------------------------------------------------
        // Se busca el mínimo entre los buffers de cada archivo de entrada y se agrega al buffer  
        // del archivo de salida
        // ----------------------------------------------------------------------------------------
        bool are_buff_non_empty = true;
        while(are_buff_non_empty){ 
            int min_value = INT_MAX;
            int min_index = -1;

            for (size_t i = 0; i < partsToRead.size(); ++i) {
                if (!partsToRead[i].buffer.empty() && partsToRead[i].buffer.front() < min_value) {
                    min_value = partsToRead[i].buffer.front();
                    min_index = i;
                }
            }

            partsToRead[min_index].buffer.erase(partsToRead[min_index].buffer.begin());
            partsToRead[min_index].bufferSize--;
            
            outputBuffer.push_back(min_value);
            outBuffSize++;

            // Si el buffer del archivo de salida se llena, se escribe en el archivo
            if (outBuffSize == B) {
                outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
                outputBuffer.clear();
                outBuffSize = 0;   
            } 

            // Si algún buffer se vacía detenemos la iteración para volver a llenarlo
            for(auto& part : partsToRead){
                if (part.bufferSize==0){ 
                    are_buff_non_empty = false;
                    break;
                }
            }
        
        }
    
    }

    // ----------------------------------------------------------------------------------------
    // En este punto todos los archivos se leyeron por completo excepto uno
    // Queda agregar todos los elementos del archivo restante al archivo de salida pero 
    // ya no se necesitan comparaciones
    // ----------------------------------------------------------------------------------------

    Part &p = partsToRead.front();

    while (true){
        if (p.bufferSize==0){
            p.buffer.resize(B);
            p.fileStr->read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
            p.bufferSize = p.fileStr->gcount() / sizeof(int);
            p.buffer.resize(p.bufferSize);

            if (p.bufferSize == 0) break; // Si no se ha llenado significa que no quedan más datos que sacar
            
        }

        int k = p.buffer.front(); 
        p.buffer.erase(p.buffer.begin());
        p.bufferSize--;

        outputBuffer.push_back(k);
        outBuffSize++;


        if (outBuffSize == B) {
            outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), B * sizeof(int));
            outputBuffer.clear();
            outBuffSize = 0;
        } 

    }

    // ----------------------------------------------------------------------------------------
    // Agrega el ultimo bloque que queda (que no es necesariamente de tamaño B)
    // ----------------------------------------------------------------------------------------
    if (!outputBuffer.empty()) { 
        outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
        outputBuffer.clear();
    }

    // ----------------------------------------------------------------------------------------
    // Se cierran todos los archivos y se eliminan los archivos de entrada.
    // ----------------------------------------------------------------------------------------
    outputFile.close();
    for(auto& part: parts){ 
        part.fileStr->close();
        std::remove(part.filename.c_str());
    }
    
    return outputFileName;
}



/**
 * @brief Particiona un archivo binario de enteros en 'a' partes.
 *
 * Esta función divide un archivo binario que contiene enteros en 'a' archivos
 * de salida, distribuyendo el contenido de forma equitativa. El resto (si no 
 * es divisible exactamente) se agrega a la primera partición.
 * 
 * La lectura y escritura se realiza en bloques, respetando un límite de 
 * memoria principal especificado en megabytes.
 *
 * @param filename Nombre del archivo a particionar.
 * @param a Número de particiones deseadas.
 * @param M Límite de memoria en MB para leer y escribir en bloques.
 *
 * @return Un vector de strings con los nombres de las particiones generadas.
 *
 * @throws Termina el programa si no se puede abrir el archivo de entrada o 
 * si falla la creación de alguna partición de salida.
 */

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

    std::string baseName = std::filesystem::path(filename).stem().string();

    for (int i = 0; i < a; ++i) {
        size_t cantidad = intsPorParte + (i < resto ? 1 : 0);
        std::string nombreParte = "../bin/" + baseName + "_" + std::to_string(i) + ".bin";
        std::ofstream salida(nombreParte, std::ios::binary);

        if (!salida) {
            std::cerr << "No se pudo crear " << nombreParte << "\n";
            exit(1);
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


/**
 * @brief Ordena un archivo binario de enteros utilizando ordenamiento externo.
 *
 * Esta función ordena recursivamente un archivo binario de enteros que no 
 * cabe completamente en memoria principal. Si el archivo es lo 
 * suficientemente pequeño como para cargarse en memoria, se ordena directamente.
 *
 * El algoritmo utiliza una estrategia de ordenamiento externo basada en 
 * particiones, con una aridad configurable.
 *
 * @param filename Nombre del archivo a ordenar.
 * @param M Tamaño de la memoria RAM disponible (en MB).
 * @param a Aridad del algoritmo de ordenamiento externo.
 *
 * @return Nombre del archivo binario resultante con los datos ordenados.
 *
 * @throws Termina la ejecución si no se puede abrir el archivo de entrada.
 */

std::string extMergeSort(const std::string &filename, int M, int a){
    std::uintmax_t ram = M*1000000; // Memoria ram en bytes
    std::uintmax_t fileSize = std::filesystem::file_size(filename); // Se determina el tamaño del archivo
    size_t numInts = fileSize / sizeof(int);    

    // ----------------------------------------------------------------------------------------
    // Si el tamaño del archivo es menor al de la memoria RAM, se ordena todo en memoria principal
    // ----------------------------------------------------------------------------------------
    if(fileSize<ram){
        std::vector<int> buffer(numInts); // Se inicializa un vector del tamaño del archivo
        
        std::fstream inputFile(filename, std::ios::in | std::ios::out | std::ios::binary); 

        if (!inputFile) {
            std::cerr << "Error al abrir el archivo " << filename <<"\n";
            std::exit(1);
        }

        inputFile.read(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int)); // Se lee el archivo completo en el buffer
        
        std::sort (buffer.begin(), buffer.end()); // Se ordena
        
        // Se sobrescribe el arreglo ordenado en el mismo archivo
        inputFile.seekp(0); // Vuelve al principio
        inputFile.write(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int));
        inputFile.close();
        return filename;
    }

    // Si el archivo es más grande que la RAM se particiona 
    std::vector<std::string> particiones = partitionFile(filename, a, M);

    std::vector<std::string> sortedPart;
    for (auto particion : particiones){
        std::string sortedFile = extMergeSort(particion, M, a);
        sortedPart.push_back(sortedFile);
    }

    std::string baseName = std::filesystem::path(filename).stem().string(); // Obtiene solo el nombre del archivo
    std::string orderedFileName = "../bin/" + baseName + "_sorted.bin"; // Crea el nombre del nuevo archivo ordenado, se guarda en el directorio bin
    
    
    std::string orderedFile = mergeFiles(sortedPart, orderedFileName);

    for(auto& part: particiones) std::remove(part.c_str());
    for(auto& part: sortedPart) std::remove(part.c_str());
    
    std::cout << "------------------------Se creó el archivo ordenado " << orderedFileName << ":) ------------------------\n";

    return orderedFile;
}

int main(){

    extMergeSort("../bin/unsorted8.bin", 1, 4);

    return 1;

}



