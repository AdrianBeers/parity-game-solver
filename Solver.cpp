#include "Solver.h"

using namespace std;

void Solver::initialize(shared_ptr<ParityGame> parityGame) {
}

shared_ptr<Measure> Solver::prog(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v, shared_ptr<NodeSpec> &w) {
    return;
}

shared_ptr<ProgressMeasure> Solver::lift(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v) {
    return;
}

bool Solver::isStabilised(shared_ptr<ProgressMeasure> &rho, shared_ptr<ProgressMeasure> &rhoLifted) {
    for (shared_ptr<NodeSpec> node : G->nodes) {
        Measure measureRho = (*(*rho)[node]);
        Measure measureRhoLifted = (*(*rhoLifted)[node]);

        // If measure was lifted to tau, it is not stabilised
        if (measureRho.first != measureRhoLifted.first) {
            return false;
        }

        // If some m was lifted, it is not stabilised
        for (int i = 0; i < d; i++) {
            if (measureRho.second[i] != measureRhoLifted.second[i]) {
                return false;
            }
        }
    }

    return true;
}

shared_ptr<ProgressMeasure> Solver::SPMInputOrder() {
    // Initialize rho with zeroes
    shared_ptr<ProgressMeasure> rho;

    for (shared_ptr<NodeSpec> node : G->nodes) {
        vector<uint32_t> zeroes(d, 0);
        shared_ptr<Measure> measure = make_shared<Measure>(make_pair(false, zeroes));

        (*rho)[node] = measure;
    }

    // Variables to keep track of looping
    uint32_t nodesVisited = 0;
    uint32_t nodesStabilised = 0;
    uint32_t nodesTotal = G->nodes.size();

    while (nodesStabilised < nodesTotal) {
        shared_ptr<ProgressMeasure> rhoLifted = lift(rho, G->nodes[nodesVisited % nodesTotal]);

        while (!isStabilised(rho, rhoLifted)) {
            // If rho was not already stabilised, reset nodesStabilised
            nodesStabilised = 0;

            // Continue lifting rho
            rho = rhoLifted;
            rhoLifted = lift(rho, G->nodes[nodesVisited % nodesTotal]);
        }

        nodesVisited++;
        nodesStabilised++;
    }

    return rho;
}