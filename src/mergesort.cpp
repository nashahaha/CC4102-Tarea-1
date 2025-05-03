#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

size_t B = 1024; //DETERMINAR BIEN

void readBinFile(const std::string &filename){
    std::ifstream inputFile(filename, std::ios::binary); // abre archivo 1
    
    if (!inputFile) {
        std::cerr << "Error: could not open file '" << filename << "' for reading.\n";
        exit(1);
    }

    // Get file size
    inputFile.seekg(0, std::ios::end);
    std::streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    // Determine number of ints
    size_t numInts = fileSize / sizeof(int);
    
    std::vector<int> buffer(numInts);

    // Read data
    if (!inputFile.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "Error reading file\n";
        return;
    }

    // Print contents
    for (int val : buffer) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

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
            buffer1.resize(B);
            inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B * sizeof(int));
            std::streamsize bytesRead1 = inputFile1.gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
            Buff1Size = bytesRead1 / sizeof(int);
            buffer1.resize(Buff1Size);
        }
        if (Buff2Size == 0) {
            buffer2.resize(B);
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

                // para escribir todo el vector de una (en vez de elemento por elemento)
                // no lo voy a usar todavía
                outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));

                //for (int val : outputBuffer) // 
                //    outputFile.write(reinterpret_cast<const char*>(&val), sizeof(int));
                
                outputBuffer.clear();
                outBuffSize = 0;
            } 
        }


    }

    // En este punto alguno de los archivos ya se leyó por completo
    // Queda agregar todos los elementos del archivo restante al outputFile

    while (Buff1Size>0 || inputFile1.peek()!=EOF){
        int k = buffer1.front(); // el primer elemento del buffer 1 es menor
        buffer1.erase(buffer1.begin()); // se elimina del buffer
        Buff1Size--;

        outputBuffer.push_back(k);
        outBuffSize++;

        if (outBuffSize == B) {
            outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
            
            outputBuffer.clear();
            outBuffSize = 0;
        } 

        if (Buff1Size==0){
            buffer1.resize(B);
            inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B * sizeof(int));
            Buff1Size = inputFile1.gcount() / sizeof(int);
            buffer1.resize(Buff1Size); // Resize to actual data read

            if (Buff1Size == 0) break; // Si no se ha llenado significa que no quedan más datos que sacar
        }
    }

    while (Buff2Size>0 || inputFile2.peek()!=EOF){
        int k = buffer2.front(); // el primer elemento del buffer 1 es menor
        buffer2.erase(buffer2.begin()); // se elimina del buffer
        Buff2Size--;

        outputBuffer.push_back(k);
        outBuffSize++;

        if (outBuffSize == B) {
            outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
            
            outputBuffer.clear();
            outBuffSize = 0;
        }  


        if (Buff2Size==0 && !inputFile2){
            buffer2.resize(B);
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


std::vector<std::string> partitionFile(const std::string& archivoOriginal, size_t bloqueSize) {
    std::ifstream entrada(archivoOriginal, std::ios::binary);
    std::vector<std::string> particiones;

    if (!entrada) {
        std::cerr << "No se pudo abrir el archivo de entrada\n";
        exit(1);
    }

    std::vector<int> buffer(bloqueSize);
    int parte = 0;

    while (entrada) {
        entrada.read(reinterpret_cast<char*>(buffer.data()), bloqueSize * sizeof(int));
        size_t leidos = entrada.gcount() / sizeof(int);
        if (leidos == 0) break;  // Nada más que leer

        std::string nombreParte = "parte_" + std::to_string(parte++) + ".bin";
        std::ofstream salida(nombreParte, std::ios::binary);
        if (!salida) {
            std::cerr << "No se pudo crear " << nombreParte << "\n";
            break;
        }

        salida.write(reinterpret_cast<char*>(buffer.data()), leidos * sizeof(int));
        salida.close();

        particiones.push_back(nombreParte);  // Agregar el nombre a la lista
    }

    entrada.close();
    return particiones;
}


// asumo que M (el tamaño de la memoria ram) se entrega en Megas
std::string extMergeSort(const std::string &filename, int M, int a){
    std::uintmax_t ram = M*1000000; // memoria ram en bytes
    std::uintmax_t fileSize = std::filesystem::file_size(filename); // determinar tamaño del archivo
    // si fileSize <M 
    // Se ordena todo en memoria principal

    if(fileSize<=ram){
        std::vector<int> buffer(B); //inicializo un vector
        std::ifstream inputFile(filename, std::ios::binary); //abro el archivo
        inputFile.read(reinterpret_cast<char*>(buffer.data()), B * sizeof(int)); //lo leo en el buffer
        std::sort (buffer.begin(), buffer.end()); // se ordena
        return filename; //retorna
    }

    // sino
    // determinar aridad --> voy a asumir que me entregan la aridad

    // particionar archivo según la aridad
    std::vector particiones = partitionFile(filename, a);

    for (auto particion : particiones){
        extMergeSort(particion, M, a);
    }

    // invocar mergeFiles()
    std::string orderedFile = mergeFiles(particiones, "outpun.bin");

    return orderedFile; //retornar nombre del archivo ordenado
}

int main(){

    //mergeFiles("../bin/sorted1.bin", "../bin/sorted2.bin", "../bin/merged12.bin"); // para probar
    //mergeFiles("../bin/sorted1.bin", "../bin/sorted1.bin", "../bin/merged11.bin");
    //mergeFiles("../bin/sorted2.bin", "../bin/sorted2.bin", "../bin/merged22.bin");

    mergeFiles("../bin/sorted1T1.bin", "../bin/sorted2T1.bin", "../bin/mergedT1.bin");

    return 1;

}



