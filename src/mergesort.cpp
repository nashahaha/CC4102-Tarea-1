#include <vector>
#include <iostream>
#include <fstream>

size_t B = 1024; //DETERMINAR BIEN

std::string mergeFiles(const std::string &filename1, const std::string &filename2){
    

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

    const std::string &outputFileName = "outputfile.bin"; //CAMBIAR NOMBRE

    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    int i1 = B, i2 = B, out_i=0; //indice del primer elemento del buffer, sirve para solo vaciar el buffer una vez
    //int f1 = 0, f2 = 0;

    int j1 = B-i1, j2 = B-i2;

    while(!inputFile1.eof() || !inputFile2.eof()){
        
        
        while(j1>=0 && j2>=0){ //VER CASO EN QUE TIENEN TAMAÑOS DISTINTOS


            if (j1 == 0){
                //leer el siguiente bloque
                inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B*sizeof(int)); //preocuparse del caso donde no calzan perfecto
                i1 = 0;
            }   
            if (j2==0){
                //leer el siguiente bloque
                inputFile2.read(reinterpret_cast<char*>(buffer2.data()), B * sizeof(int));
                i2 = 0;
            } 


            int menor;
            if(buffer1.front()<buffer2.front()){
                menor = buffer1.front(); // el primer elemento del buffer 1 es menor
                buffer1.erase(buffer1.begin()); // se elimina del buffer
                i1++;
            }else{
                menor = buffer2.front();
                buffer2.erase(buffer2.begin());
                i2++;
            }
            outputBuffer.push_back(menor);

            out_i++;
            std::cout << "El buffer de salida tiene " << out_i << " elementos\n";        

            if (out_i>=B){ // si el buffer de salida excede su tamaño máximo
                std::cout << "buffer 1 tiene " << i1 << " elementos\n";
                std::cout << "buffer 2 tiene " << i2 << " elementos\n";
                std::cout << "Se está escribiendo en el bin de salida\n";
                //escribir en el archivo de output
                for (size_t i = 0; i < outputBuffer.size(); ++i) {
                    outputFile.write(reinterpret_cast<const char*>(&outputBuffer[i]), sizeof(int));
                    out_i--;
                }

                //vaciar buffer
                outputBuffer.clear();
            }
        }

        while (j1>0){
            
            if (j1 ==0 ){
                //leer el siguiente bloque
                inputFile1.read(reinterpret_cast<char*>(buffer1.data()), B*sizeof(int)); //preocuparse del caso donde no calzan perfecto
                i1 = 0;
            } 

            int k = buffer1.front(); // el primer elemento del buffer 1 es menor
            buffer1.erase(buffer1.begin()); // se elimina del buffer
            i1++;
            outputBuffer.push_back(k);
        }

        while (j2>=0){
            std::cout << "Entro\n";
            if (j2==0){
                //leer el siguiente bloque
                inputFile2.read(reinterpret_cast<char*>(buffer2.data()), B * sizeof(int));
                i2 = 0;
            }
            int k = buffer2.front(); // el primer elemento del buffer 1 es menor
            buffer2.erase(buffer2.begin()); // se elimina del buffer
            i2++;
            outputBuffer.push_back(k);
        }
            
        
    }

    outputFile.close();
    inputFile1.close();
    inputFile2.close();

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

    mergeFiles("sorted1.bin", "sorted2.bin"); // para probar

    return 1;

}



