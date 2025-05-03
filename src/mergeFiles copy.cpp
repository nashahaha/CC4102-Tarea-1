#include <vector>
#include <iostream>
#include <fstream>
#include <string>

size_t B = 1024; //DETERMINAR BIEN
int INT_MAX = 99999999;

struct Part {
    int id;
    std::string filename;
    std::ifstream fileStr;
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

        p.fileStr.open(p.filename, std::ios::binary);
        
        
        if (!p.fileStr.is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
            std::cerr << "Error al abrir el archivo " << p.filename <<"\n";
            std::exit(1);
        }

        std::vector<int> buff(B); // se inicializa su buffer
        p.buffer = buff;

        p.bufferSize = 0;

        parts.push_back(p);

    }

    std::vector<int> outputBuffer;

    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    int outBuffSize=0; // tamaño del buffer de salida

    bool has_file_rech_eof = false;

    while(!has_file_rech_eof){

        for ( int i = 0; i<parts.size(); i++){
            Part &p = parts[i];
            if (p.bufferSize == 0) {
                p.buffer.resize(B);
                p.fileStr.read(reinterpret_cast<char*>(p.buffer.data()), B * sizeof(int));
                std::streamsize bytesRead1 = p.fileStr.gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
                p.bufferSize = bytesRead1 / sizeof(int);
                p.buffer.resize(p.bufferSize);
            }
        }

        bool are_buff_non_empty = true;
        for (auto& part: parts){
            if(part.bufferSize<=0){
                are_buff_non_empty = false;
                break;
            }
        }

        while(are_buff_non_empty){ 
            // Find the buffer with the smallest front element
            //int menor;
            int min_value = INT_MAX;
            int min_index = -1;

            // Iterate through the buffers
            for (size_t i = 0; i < parts.size(); ++i) {
                if (!parts[i].buffer.empty() && parts[i].buffer.front() < min_value) {
                    min_value = parts[i].buffer.front();
                    min_index = i;
                }
            }

            // Update 'menor' with the smallest element
            //menor = min_value;

            // Remove the front element of the chosen buffer
            parts[min_index].buffer.erase(parts[min_index].buffer.begin());
            
            outputBuffer.push_back(min_value);
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

        for( auto& part : parts){
            if (part.fileStr.peek()==EOF){
                has_file_rech_eof = true;
                break;
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