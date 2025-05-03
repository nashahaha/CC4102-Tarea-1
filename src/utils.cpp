// Se incluyen todas las funciones para hacer los test para merge

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../include/mergetest.hpp"

bool isBinaryFileSorted(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: could not open file '" << filename << "' for reading.\n";
        return false;
    }

    int prev, current;
    if (!infile.read(reinterpret_cast<char*>(&prev), sizeof(int))) {
        // File is empty or unreadable → consider it sorted
        return true;
    }

    while (infile.read(reinterpret_cast<char*>(&current), sizeof(int))) {
        if (current < prev) {
            return false;  // Not sorted
        }
        prev = current;
    }

    return true;
}


void generateSortedBinaryFile(const std::string& filename, size_t sizeKB, int start = 0, int step = 1) {
    size_t totalBytes = sizeKB * 1024;
    size_t numIntegers = totalBytes / sizeof(int);

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: could not open file for writing.\n";
        return;
    }

    int current = start;
    for (size_t i = 0; i < numIntegers; ++i) {
        outfile.write(reinterpret_cast<const char*>(&current), sizeof(int));
        current += step;
    }

    outfile.close();
    std::cout << "Sorted file '" << filename << "' created with " << numIntegers << " integers (" << sizeKB << " KB).\n";
}

size_t B = 1024;


std::string mergeFiles(const std::string &filename1, const std::string &filename2, const std::string &outputFileName){

    //abrir archivos
    std::ifstream inputFile1(filename1, std::ios::binary); // abre archivo 1
    std::ifstream inputFile2(filename2, std::ios::binary); // abre archivo 2

    if (!inputFile1.is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
        std::cerr << "Error al abrir el archivo " << filename1 <<"\n";
        std::exit(1);
    }


    if (!inputFile2.is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
        std::cerr << "Error al abrir el archivo " << filename2 <<"\n";
        std::exit(1);
    }

    std::vector<int> buffer1(B); // se inicializa con 0s
    std::vector<int> buffer2(B);

    std::vector<int> outputBuffer;

    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    int Buff1Size = 0, Buff2Size = 0, outBuffSize=0; // tamaño de los buffer

    while(inputFile1.peek()!=EOF && inputFile2.peek()!=EOF){
        
        if (Buff1Size == 0) {
            inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B * sizeof(int));
            std::streamsize bytesRead1 = inputFile1.gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
            Buff1Size = bytesRead1 / sizeof(int);
            buffer1.resize(Buff1Size);
        }
        if (Buff2Size == 0) {
            inputFile2.read(reinterpret_cast<char*>(buffer2.data()), B * sizeof(int));
            std::streamsize bytesRead2 = inputFile2.gcount();
            Buff2Size = bytesRead2 / sizeof(int);
            buffer2.resize(Buff2Size);
        }

        while(Buff1Size>0 && Buff2Size>0){ 

            int menor;
            if(buffer1.front()<buffer2.front()){
                menor = buffer1.front(); // el primer elemento del buffer 1 es menor
                buffer1.erase(buffer1.begin()); // se elimina del buffer
                Buff1Size--;

            }else{
                menor = buffer2.front();
                buffer2.erase(buffer2.begin());
                Buff2Size--;
            }
            outputBuffer.push_back(menor);
            outBuffSize++;

            if (outBuffSize == B) {

                for (int val : outputBuffer)
                    outputFile.write(reinterpret_cast<const char*>(&val), sizeof(int));
                
                outputBuffer.clear();
                outBuffSize = 0;
            } 
        }
    }

    // En este punto alguno de los archivos ya se leyó por completo
    // Queda agregar todos los elementos del archivo restante al outputFile

    while (Buff1Size>0 || inputFile1.peek()!=EOF){
        if (Buff1Size==0){
            inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B * sizeof(int));
            Buff1Size = inputFile1.gcount() / sizeof(int);
            buffer1.resize(Buff1Size); // Resize to actual data read

            if (Buff1Size == 0) break; // No more data
        }
        int k = buffer1.front(); // el primer elemento del buffer 1 es menor
        buffer1.erase(buffer1.begin()); // se elimina del buffer
        Buff1Size--;

        outputBuffer.push_back(k);
        outBuffSize++;

        if (outBuffSize == B) {
            for (int val : outputBuffer)
                outputFile.write(reinterpret_cast<const char*>(&val), sizeof(int));
            
            outputBuffer.clear();
            outBuffSize = 0;
        }  
    }

    while (Buff2Size>0 || inputFile2.peek()!=EOF){
        int k = buffer2.front(); // el primer elemento del buffer 1 es menor
        buffer2.erase(buffer2.begin()); // se elimina del buffer
        Buff2Size--;

        outputBuffer.push_back(k);
        outBuffSize++;

        if (outBuffSize == B) {
            for (int val : outputBuffer)
                outputFile.write(reinterpret_cast<const char*>(&val), sizeof(int));
            
            outputBuffer.clear();
            outBuffSize = 0;
        }  


        if (Buff2Size==0 && !inputFile2){
            inputFile2.read(reinterpret_cast<char*>(buffer2.data()), B * sizeof(int));
            Buff2Size = inputFile2.gcount() / sizeof(int);
            buffer2.resize(Buff2Size);

            if (Buff2Size == 0) break; 
        }
    }

    outputFile.close();
    inputFile1.close();
    inputFile2.close();

    std::cout << "El archivo " << outputFileName << " fue creado\n";

    return outputFileName;
    
    
}