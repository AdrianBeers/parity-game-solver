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
    return;
}

shared_ptr<ProgressMeasure> Solver::SPMInputOrder() {
    shared_ptr<ProgressMeasure> rho;

    for (shared_ptr<NodeSpec> node : G->nodes) {
        vector<uint32_t> zeroes(d, 0);
        shared_ptr<Measure> measure = make_shared<Measure>(make_pair(false, zeroes));

        (*rho)[node] = measure;
    }

    uint32_t nodesVisited = 0;
    uint32_t nodesStabilised = 0;
    uint32_t nodesTotal = G->nodes.size();

    while (nodesStabilised < nodesTotal) {
        shared_ptr<ProgressMeasure> rhoLifted = lift(rho, G->nodes[nodesVisited % nodesTotal]);

        while (!isStabilised(rho, rhoLifted)) {
            nodesStabilised = 0;
            rho = rhoLifted;
            rhoLifted = lift(rho, G->nodes[nodesVisited % nodesTotal]);
        }

        nodesVisited++;
        nodesStabilised++;
    }

    return rho;
}