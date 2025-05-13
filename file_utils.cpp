#include <fstream>
#include <iostream>

#include "file_utils.h"

namespace game {

std::string LoadTextFile(const char *filename) {

    // Open file
    std::ifstream f;
    f.open(filename);
    if (f.fail()) {
        throw(std::ios_base::failure(std::string("Error opening file ") + std::string(filename)));
    }

    // Read file into a string
    std::string content;
    std::string line;
    while (std::getline(f, line)) {
        content += line + "\n";
    }

    // Close file
    f.close();

    return content;
}

} // namespace game
