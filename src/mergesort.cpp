#include <vector>
#include <iostream>
#include <fstream>

size_t B = 1024; //DETERMINAR BIEN

std::string mergeFiles(const std::string &filename1, const std::string &filename2, const std::string &outputFileName){
    

    //abrir archivos
    std::ifstream inputFile1(filename1, std::ios::binary); // abre archivo 1
    std::ifstream inputFile2(filename2, std::ios::binary); // abre archivo 2

    if (!inputFile1.is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
        std::cerr << "Error al abrir el archivo" << filename1 <<"\n";
        std::exit(1);
    }


    if (!inputFile2.is_open()) { // Dar mensaje de error si no se pueden abrir los archivos
        std::cerr << "Error al abrir el archivo" << filename2 <<"\n";
        std::exit(1);
    }

    
    std::vector<int> buffer1(B); // se inicializa con 0s

    std::vector<int> buffer2(B);

    std::vector<int> outputBuffer;

    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    int Buff1Size = 0, Buff2Size = 0, outBuffSize=0; // tamaño de los buffer

    while(inputFile1.peek()!=EOF && inputFile2.peek()!=EOF){
        
        if (Buff1Size == 0 && inputFile1) {
            inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B * sizeof(int));
            std::streamsize bytesRead1 = inputFile1.gcount(); // entrega la cantidad de bytes efectivamenete leidos en la última operación
            Buff1Size = bytesRead1 / sizeof(int);
            buffer1.resize(Buff1Size);
        }
        if (Buff2Size == 0 && inputFile2) {
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
            buffer2.resize(Buff2Size); // Resize to actual data read

            if (Buff2Size == 0) break; // No more data
        }
    }

    

    outputFile.close();
    inputFile1.close();
    //inputFile2.close();

    std::cout << "El archivo " << outputFileName << " fue creado\n";

    return outputFileName;
    
    
}



std::string extMergeSort(const std::string &filename1, int M){
    size_t fileSize = 0; // determinar tamaño del archivo
    // si fileSize <M 
    // Se ordena todo en memoria principal

    // sino
    // determinar aridad

    // particionar archivo según la aridad

    // para cada sub-archivo
        // extMergeSort()

    // invocar mergeFiles()

    return ""; //retornar nombre del archivo ordenado
}

int main(){

    mergeFiles("sorted1.bin", "sorted2.bin", "merged12.bin"); // para probar
    mergeFiles("sorted1.bin", "sorted1.bin", "merged11.bin");
    mergeFiles("sorted2.bin", "sorted2.bin", "merged22.bin");

    return 1;

}



