#include <vector>
#include <iostream>

int main(){
      
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