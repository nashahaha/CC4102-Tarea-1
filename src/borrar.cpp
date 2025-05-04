#include <vector>
#include <iostream>
#include <fstream>

/*
Este archivo es para hacer pequeñas pruebas de ciertos pedazos de codigo para verifiacr si funcionan como se espera
*/


int main(){
    
    const std::string &outputFileName = "outputtest.bin"; //CAMBIAR NOMBRE
    std::ofstream outputFile(outputFileName, std::ios::binary); // abre archivo de salida 

    std::vector<int> outputBuffer = {1,2,3,4,5,6};
    int outBuffSize = 6;


    for (size_t i = 0; i < 6; ++i) {
        outputFile.write(reinterpret_cast<const char*>(&outputBuffer[i]), sizeof(int));
        outBuffSize--;
    }

    //vaciar buffer
    outputBuffer.clear();

    outputFile.close();


    std::ifstream inputFile1("outputtest.bin", std::ios::binary); // abre archivo 1
    std::cout <<"hola\n";
    std::vector<int> buffer1(6);


    inputFile1.read(reinterpret_cast<char*>(buffer1.data()), 10 * sizeof(int));
    std::streamsize bytesRead2 = inputFile1.gcount();
    int Buff1Size = bytesRead2 / sizeof(int);
    buffer1.resize(Buff1Size);

    std::cout << "Tamaño: " << Buff1Size<< "\n";
    std::cout << buffer1.size();
    
    std::cout <<"hola2\n";
    for (auto i : buffer1)
            std::cout << i << " ";
    std::cout << "\n";
    std::cout <<"hola3\n";
    return 1;
}


int test_2(){

    int i1 = 0, i2 =0;
    int B = 5;
    int j1;
    int j2 = B-i2;

    while(j1>0 ){ //VER CASO EN QUE TIENEN TAMAÑOS DISTINTOS

        j1 = B-i1;
        std::cout << j1 << "\n";
        i1--;

    }
    return 1;
}


int test_1(){
      
    std::vector<int> buffer1 = {2, 5, 8, 12}; 
    //buffer1.resize(B); // e.g., BUFFER_SIZE = 1024 or whatever you want

    std::vector<int> buffer2 = {1,3,5,9,10,11,12};

    std::vector<int> outputBuffer;

    int i1 =4, i2=7;
    while(i1>0 && i2>0){ 
        int menor;
        if(buffer1.front()<buffer2.front()){
            menor = buffer1.front(); // el primer elemento del buffer 1 es menor
            buffer1.erase(buffer1.begin()); // se elimina del buffer
            i1--;
        }else{
            menor = buffer2.front();
            buffer2.erase(buffer2.begin());
            i2--;
        }
        outputBuffer.push_back(menor);

        std::cout << "El outputbuffer está como sigue:\n";
        for (auto i : outputBuffer)
            std::cout << i << " ";
        std::cout << "\n";
        std::cout <<"----------------------------------------------\n";
    }

    std::cout << "el buffer 1 queda como: ";
    for (auto i : buffer1)
            std::cout << i << " ";
    std::cout << "\n";

    std::cout << "el buffer 2 queda como: ";
    for (auto i : buffer2)
            std::cout << i << " ";
    std::cout << "\n";

    return 1;
}