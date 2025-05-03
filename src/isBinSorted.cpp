#include <iostream>
#include <fstream>
#include <string>

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

int main(){
    bool is_sorted = isBinaryFileSorted("merged11.bin") && isBinaryFileSorted("merged12.bin") && isBinaryFileSorted("merged22.bin");
    
    if(is_sorted == false)
        std::cout << "hay un archivo desordenado\n";
    else
        std::cout << "Los archivos están ordenados :)\n";

    return is_sorted;
}