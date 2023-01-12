#include <iostream>
#include <fstream>
#include <sstream>
#include <ratio>
#include <chrono>
#include "ParityGame.h"
#include "Parser.h"
#include "Solver.h"

#define PRINT_PARSING false
#define CREATE_SUMMARY true

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

void pprintMeasure(shared_ptr<Measure> &m) {
    if (m->empty()) {
        cout << "tau";
    } else {
        cout << '(';
        for (int i = 0; i < m->size(); i++) {
            if (i > 0) {
                cout << ",";
            }
            cout << m->at(i);
        }
        cout << ')';
    }
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
#if (PRINT_PARSING)
    cout << "maxId=" << pg->maxId << endl;
    for (const auto &n: pg->nodes) {
        cout << "node: id=" << n->id << " prio=" << n->priority << " owner=" << (uint32_t) n->owner
             << " successors=";
        for (int i = 0; i < n->successors.size(); i++) {
            if (i > 0) {
                cout << ",";
            }
            cout << i;
        }
        cout << ' ';
        if (!n->name.empty()) {
            cout << "name=\"" << n->name << '"';
        }
        cout << endl;
    }
#else
    cout << "#nodes=" << pg->nodes.size() << endl;
#endif

    // Solve it or something
    cout << "-- Solving results --" << endl;
    Solver solver;
    solver.initialize(pg);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::milli;

    // Solve formula on LTS using naive algorithm
    auto t1 = high_resolution_clock::now();
    const auto r = solver.SPM(LiftStrategy::Predecessor);
    auto t2 = high_resolution_clock::now();
    duration<double, milli> diff = t2 - t1;

    cout << "Execution time: " << diff.count() << " ms" << endl;
#if (CREATE_SUMMARY)
    if (!r->empty()) {
        cout << "node 0: ";
        pprintMeasure(r->at(0));
        cout << endl;
    }
    int nrVerticesDiamond = 0, nrVerticesBox = 0;
    for (const auto &p : *r) {
        if (p.second->empty()) {
            nrVerticesBox++;
        } else {
            nrVerticesDiamond++;
        }
    }
    cout << "nrVerticesDiamond=" << nrVerticesDiamond << endl;
    cout << "nrVerticesBox=" << nrVerticesBox << endl;
#else
    for (const auto &k: *r) {
        cout << "node " << k.first->id << ": ";
        pprintMeasure((shared_ptr<Measure> &) k.second);
        cout << endl;
    }
#endif

    return 0;
}
