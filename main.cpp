#include <iostream>
#include <fstream>
#include <sstream>
#include <ratio>
#include <chrono>
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

    cout << "-- Parsing results --" << endl;
    cout << "maxId=" << pg->maxId << endl;
    for (const auto &n: pg->nodes) {
        cout << "node: id=" << n->id << " prio=" << n->priority << " owner=" << (uint32_t) n->owner << " successors=";
        for (auto s: n->successors) {
            cout << s << ",";
        }
        cout << ' ';
        if (!n->name.empty()) {
            cout << "name=\"" << n->name << '"';
        }
        cout << endl;
    }

    // Solve it or something
    Solver solver;
    solver.initialize(pg);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::milli;

    // Solve formula on LTS using naive algorithm
    auto t1 = high_resolution_clock::now();
    const auto r = solver.SPM(LiftStrategy::Random);
    auto t2 = high_resolution_clock::now();
    duration<double, milli> diff = t2 - t1;

    cout << "Algorithm execution time: " << diff.count() << " ms" << endl;


    cout << "-- Solving results --" << endl;
    for (const auto &k: *r) {
        cout << "node " << k.first->id << ": ";
        if (k.second->empty()) {
            cout << "tau";
        } else {
            cout << '(';
            for (int i = 0; i < k.second->size(); i++) {
                if (i > 0) {
                    cout << ",";
                }
                cout << (*k.second)[i];
            }
            cout << ')';
        }
        cout << endl;
    }

    return 0;
}
