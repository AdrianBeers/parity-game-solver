#include <iostream>
#include <fstream>
#include <sstream>
#include "ParityGame.h"
#include "Parser.h"

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

    // Parse parity game
    string pgText = "parity 4;\n1 3 0 1,3,4 \"Europe\";\n0 6 1 4,2 \"Africa\";\n4 5 1 0 \"Antarctica\";\n1 8 1 2,4,3 \"America\";\n3 6 0 4,2 \"Australia\";\n2 7 0 3,1,0,4 \"Asia\";";
    ParityGameParser pgp;
    shared_ptr<ParityGame> pg = pgp.parse(pgText);

    cout << "maxId = " << pg->maxId << endl;
    for (auto n : pg->nodes) {
        cout << "node: " << n->id << " " << n->priority << " " << (uint32_t) n->owner << " ";
        for (auto s : n->successors) {
            cout << s << ",";
        }
        cout << " ";
        if (!n->name.empty()) {
            cout << '"' << n->name << '"';
        }
        cout << endl;
    }

    return 0;
}
