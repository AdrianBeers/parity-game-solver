#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


bool readFileContents(const char *filename, string &out) {
    // Open filename stream
    ifstream in;
    in.open(filename);

    // Check if filename exists
    if (!in.is_open()) {
        return false;
    }

    // Read filename contents
    stringstream stream;
    stream << in.rdbuf();
    out = stream.str();

    // Close input stream
    in.close();

    return true;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
