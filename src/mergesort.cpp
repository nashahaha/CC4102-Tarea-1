#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

size_t B = 1024; //DETERMINAR BIEN
int INT_MAX = 99999999;

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


struct Part {
    int id; //util para debuggear
    std::string filename;
    std::ifstream* fileStr;
    std::vector<int> buffer;
    int bufferSize;
  };

std::string mergeFiles(std::vector<std::string> partitions, const std::string &outputFileName){
    //abrir archivos

    std::vector<Part> parts;

    for (int i = 0; i<partitions.size(); i++) { //inicializamos los valores para cada archivo
        Part p;
        p.id = i;
        p.filename = partitions[i];

        p.fileStr = new std::ifstream();
        p.fileStr->open(p.filename, std::ios::binary);
        
        
        if (!p.fileStr->is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
            std::cerr << "Error al abrir el archivo " << p.filename <<"\n";
            std::exit(1);
        }

        std::vector<int> buff(B); // se inicializa su buffer
        p.buffer = buff;

        p.bufferSize = 0;

        parts.push_back(p);
        
        std::cout << "Se abrió "<< p.filename << "\n";
    }

    std::vector<Part> partes = parts;

    std::vector<int> outputBuffer;

    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    int outBuffSize=0; // tamaño del buffer de salida


    while(partes.size()>1){

        for ( int i = 0; i<partes.size(); i++){ // Se llenan todos los buffer vacios
            Part &p = partes[i];
            
            if (p.bufferSize == 0 && p.fileStr->peek()!=EOF) {
                p.buffer.resize(B);
                p.fileStr->read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
                std::streamsize bytesRead1 = p.fileStr->gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
                p.bufferSize = bytesRead1 / sizeof(int);
                p.buffer.resize(p.bufferSize);
            }

        }

        
        for (auto it = partes.begin(); it != partes.end(); ) {
            if (it->bufferSize == 0 && it->fileStr->peek()==EOF) {
                it = partes.erase(it);  // si despues de llenar los buffer sigue vacío lo eliminamos
                
            } else {
                it++;
            }
        }

        bool are_buff_non_empty = true;

        while(are_buff_non_empty){ 
            int menor;
            int min_value = INT_MAX;
            int min_index = -1;

            // Iterate through the buffers
            for (size_t i = 0; i < partes.size(); ++i) {
                if (!partes[i].buffer.empty() && partes[i].buffer.front() < min_value) {
                    min_value = partes[i].buffer.front();
                    min_index = i;
                }
            }


            //menor = min_value;

            // Remove the front element of the chosen buffer
            partes[min_index].buffer.erase(partes[min_index].buffer.begin());
            partes[min_index].bufferSize--;
            
            outputBuffer.push_back(min_value);
            outBuffSize++;

            if (outBuffSize == B) {
                outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
                
                outputBuffer.clear();
                outBuffSize = 0;   
            } 

            for(auto& part : partes){
                if (part.bufferSize==0){ // si algún buffer se vacía detenemos la iteración para volver a llenarlo
                    are_buff_non_empty = false;
                    break;
                }
            }
        
        }
    
    }

    // En este punto todos los archivos ya se leyeron por completo excepto uno
    // Queda agregar todos los elementos del archivo restante al outputFile

    Part &p = partes.front(); // archivo restante (no se debe comparar con nadie)

    while (true){
        if (p.bufferSize==0){
            p.buffer.resize(B);
            p.fileStr->read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
            p.bufferSize = p.fileStr->gcount() / sizeof(int);
            p.buffer.resize(p.bufferSize); // Resize to actual data read

            if (p.bufferSize == 0) break; // Si no se ha llenado significa que no quedan más datos que sacar
            
        }

        int k = p.buffer.front(); // el primer elemento del buffer 1 es menor
        p.buffer.erase(p.buffer.begin()); // se elimina del buffer
        p.bufferSize--;

        outputBuffer.push_back(k);
        outBuffSize++;


        if (outBuffSize == B) {
            outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), B * sizeof(int));
            
            outputBuffer.clear();
            outBuffSize = 0;
        } 

    }

    if (!outputBuffer.empty()) { //Agrega el ultimo bloque que queda (que no es necesariamente de tamaño B)
        outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
        outputBuffer.clear();
    }

    outputFile.close();
    
    for(auto& part: parts){ // se cierran todos los archivos
        part.fileStr->close();
    }

    std::cout << "El archivo ordenado " << outputFileName << " fue creado :)\n";
    
    return outputFileName;
}


std::vector<std::string> partitionFile(const std::string& archivoOriginal) {
    std::ifstream entrada(archivoOriginal, std::ios::binary);
    std::vector<std::string> particiones;

    if (!entrada) {
        std::cerr << "No se pudo abrir el archivo de entrada\n";
        exit(1);
    }

    std::vector<int> buffer(B);
    int parte = 0;

    while (entrada) {
        entrada.read(reinterpret_cast<char*>(buffer.data()), B * sizeof(int));
        size_t leidos = entrada.gcount() / sizeof(int);
        if (leidos == 0) break;  // Nada más que leer

        std::string fileName = std::filesystem::path(archivoOriginal).stem().string(); // obtiene solo el nombre del archivo
        std::string nombreParte = "../bin/" + fileName + "_parte_" + std::to_string(parte++) + ".bin";
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
    size_t numInts = fileSize / sizeof(int);
    // si fileSize <M 
    // Se ordena todo en memoria principal

    if(fileSize<ram){
        std::vector<int> buffer(numInts); //inicializo un vector del tamaño del archivo
        
        std::fstream inputFile(filename, std::ios::in | std::ios::out | std::ios::binary); //abro el archivo
        inputFile.read(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int)); //lo leo en el buffer
        
        std::sort (buffer.begin(), buffer.end()); // se ordena
        
        //escribir en el archivo
        inputFile.seekp(0); // vuelve al principio del archivo para sobreescribirlo
        inputFile.write(reinterpret_cast<char*>(buffer.data()), numInts * sizeof(int));

        return filename; //retorna
    }

    // sino
    // determinar aridad --> voy a asumir que me entregan la aridad

    // particionar archivo según la aridad
    std::vector<std::string> particiones = partitionFile(filename);

    for (auto particion : particiones){
        extMergeSort(particion, M, a);
    }

    // invocar mergeFiles()
    std::string ordFileName = std::filesystem::path(filename).stem().string(); // obtiene solo el nombre del archivo
    std::string orderedFileName = "../bin/" + ordFileName + "_sorted.bin"; // crea el nombre del nuevo archivo ordenado, se guarda en el directorio bin
    std::string orderedFile = mergeFiles(particiones, orderedFileName);

    std::cout << "Se creó el archivo ordenado " << orderedFileName << "\n";

    return orderedFile; //retornar nombre del archivo ordenado
}

int main(){

    //mergeFiles({"../bin/sorted1.bin", "../bin/sorted2.bin"}, "../bin/merged12.bin"); // para probar
    //mergeFiles({"../bin/sorted1.bin", "../bin/sorted1.bin"}, "../bin/merged11.bin");
    //mergeFiles({"../bin/sorted2.bin", "../bin/sorted2.bin"}, "../bin/merged22.bin");

    //mergeFiles({"../bin/sorted1T1.bin", "../bin/sorted2T1.bin", "../bin/sorted3.bin"}, "../bin/mergedT1.bin");

    extMergeSort("../bin/unsorted4.bin", 1, 5);

    return 1;

}



