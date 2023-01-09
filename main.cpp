#include <iostream>
#include <fstream>
#include <sstream>
#include "ParityGame.h"
#include "Parser.h"
#include "Solver.h"

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

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage: parity_game_solver <pg_file>" << endl;
        return 1;
    }

    // Read input files
    string pgInput;
    if (!readFileContents(argv[1], pgInput)) {
        cerr << "Could not read parity game input file" << endl;
        return 1;
    }

    // Parse parity game
    ParityGameParser pgp;
    shared_ptr<ParityGame> pg = pgp.parse(pgInput);

    cout << "maxId = " << pg->maxId << endl;
    for (const auto &n: pg->nodes) {
        cout << "node: " << n->id << " " << n->priority << " " << (uint32_t) n->owner << " ";
        for (auto s: n->successors) {
            cout << s << ",";
        }
        cout << " ";
        if (!n->name.empty()) {
            cout << '"' << n->name << '"';
        }
        cout << endl;
    }

    // Solve it or something
    Solver solver;
    solver.initialize(pg);

    return 0;
}
