#include <vector>
#include <iostream>
#include <fstream>
#include <string>

size_t B = 1024; //DETERMINAR BIEN
int INT_MAX = 99999999;

struct Part {
    int id;
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

        std::cout << p.filename << "\n";

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

    //bool has_file_rech_eof = false;

    while(partes.size()>1){

        for ( int i = 0; i<partes.size(); i++){ // Se llenan todos los buffer vacios
            Part &p = partes[i];
            
            if (p.bufferSize == 0) {
                p.buffer.resize(B);
                p.fileStr->read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
                std::streamsize bytesRead1 = p.fileStr->gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
                p.bufferSize = bytesRead1 / sizeof(int);
                p.buffer.resize(p.bufferSize);
            }
        }

        
        for (auto it = partes.begin(); it != partes.end(); ) {
            if (it->bufferSize == 0 ) {
                it = partes.erase(it);  // si despues de llenar los buffer sigue vacío lo eliminamos
            } else {
                ++it;
            }
        }

        bool are_buff_non_empty = true;

        std::cout << "hola1\n";

        while(are_buff_non_empty){ 
            // Find the buffer with the smallest front element
            int menor;
            int min_value = INT_MAX;
            int min_index = -1;

            // Iterate through the buffers
            for (size_t i = 0; i < partes.size(); ++i) {
                if (!partes[i].buffer.empty() && partes[i].buffer.front() < min_value) {
                    min_value = partes[i].buffer.front();
                    min_index = i;
                    //std::cout << "hola6\n";
                }
            }

            //if (min_index == -1) {
            //    are_buff_non_empty = false;
            //    std::cout << "hola5\n";
            //    break;
            //}

            // Update 'menor' with the smallest element
            menor = min_value;

            // Remove the front element of the chosen buffer
            partes[min_index].buffer.erase(partes[min_index].buffer.begin());
            partes[min_index].bufferSize--;
            
            outputBuffer.push_back(menor);
            outBuffSize++;

            if (outBuffSize == B) {
                outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
                
                outputBuffer.clear();
                outBuffSize = 0;   
                std::cout << "Se escribio en el archivo de salida\n";
                std::cout << "Quedan "<< partes[0].bufferSize <<" en 1\n";
                std::cout << "Quedan "<< partes[1].bufferSize <<" en 2\n";
            } 

            for(auto& part : partes){
                if (part.bufferSize==0){
                    are_buff_non_empty = false;
                    break;
                }
            }
        
        }
        std::cout << "hola3\n";
    }

    std::cout << "hola2\n";

    // En este punto todos los archivos ya se leyeron por completo excepto uno
    // Queda agregar todos los elementos del archivo restante al outputFile

    Part &p = partes.front();
    std::cout << "quedan "<< partes.size() << " archivos por leer\n";
    while (p.bufferSize>0){
        int k = p.buffer.front(); // el primer elemento del buffer 1 es menor
        p.buffer.erase(p.buffer.begin()); // se elimina del buffer
        p.bufferSize--;

        outputBuffer.push_back(k);
        outBuffSize++;
        std::cout << "hola6\n";

        if (outBuffSize == B) {
            outputFile.write(reinterpret_cast<char*>(outputBuffer.data()), outputBuffer.size() * sizeof(int));
            
            outputBuffer.clear();
            outBuffSize = 0;
        } 
        std::cout << "hola7\n";

        if (p.bufferSize==0){
            p.buffer.resize(B);
            p.fileStr->read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
            p.bufferSize = p.fileStr->gcount() / sizeof(int);
            p.buffer.resize(p.bufferSize); // Resize to actual data read

            if (p.bufferSize == 0) break; // Si no se ha llenado significa que no quedan más datos que sacar
        }

        std::cout << "hola8\n";
    }

    outputFile.close();
    
    for(auto& part: parts){
        part.fileStr->close();
    }

    std::cout << "El archivo " << outputFileName << " fue creado\n";

    return outputFileName;
}

int main(){
    std::vector<std::string> fileNames = {"../bin/sorted1.bin", "../bin/sorted2.bin"};
    
    mergeFiles(fileNames, "../bin/pruebaNewMerge.bin");

    std::vector<std::string> fileNames2 = {"../bin/sorted1T1.bin", "../bin/sorted2T1.bin"};
    
    mergeFiles(fileNames2, "../bin/pruebaNewMergeT1.bin");

    std::vector<std::string> fileNames3 = {"../bin/sorted1T1.bin", "../bin/sorted2T1.bin", "../bin/sorted3.bin"};
    
    mergeFiles(fileNames3, "../bin/pruebaNewMergeT2.bin");
}