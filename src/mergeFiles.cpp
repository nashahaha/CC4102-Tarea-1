#include <vector>
#include <iostream>
#include <fstream>
#include <string>

size_t B = 1024; //DETERMINAR BIEN

std::string mergeFiles(std::vector<std::string> partitions, const std::string &outputFileName){
    //abrir archivos
    
    std::vector<std::ifstream> fileStreams;

    std::vector<std::vector<int>> bufferList;

    std::vector<int> bufferSizes;

    for (auto part : partitions) {// abre cada archivo de la particion
        std::ifstream inputFile(part, std::ios::binary); 
        
        if (!inputFile.is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
            std::cerr << "Error al abrir el archivo " << part <<"\n";
            std::exit(1);
        }
        fileStreams.push_back(std::move(inputFile)); //agregar a lista de archivos abiertos


        std::vector<int> buffer(B); // se inicializa su buffer
        bufferList.push_back(buffer);

        int buffsize = 0;
        bufferSizes.push_back(buffsize);

    }

    std::vector<int> outputBuffer;

    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    int outBuffSize=0; // tamaño del buffer de salida

    bool has_file_rech_eof = false;

    while(!has_file_rech_eof){

        for ( int i = 0; i<){
            if (size == 0) {
                buffer1.resize(B);
                inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B * sizeof(int));
                std::streamsize bytesRead1 = inputFile1.gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
                Buff1Size = bytesRead1 / sizeof(int);
                buffer1.resize(Buff1Size);
            }
        }
        
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

        for( auto& file : fileStreams){
            if (file.peek()==EOF){
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