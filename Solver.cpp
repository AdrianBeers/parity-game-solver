#include "Solver.h"

using namespace std;

void Solver::initialize(shared_ptr<ParityGame> parityGame) {
}

shared_ptr<Measure> Solver::prog(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v, shared_ptr<NodeSpec> &w) {
    return;
}

bool Solver::progLessOrEqual(shared_ptr<Measure> progA, shared_ptr<Measure> progB) {
    // If progB is tau, return true
    if ((*progB).first) {
        return true;
    }

    // If some m_i in progA is larger than m_i in progB, return false
    for (int i = 0; i < d; i++) {
        if ((*progA).second[i] > (*progB).second[i]) {
            return false;
        }
    }

    return true;
}

shared_ptr<ProgressMeasure> Solver::lift(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v) {
    // If measure is already tau, no lift is possible
    if ((*(*rho)[v]).first) {
        return rho;
    }

    // Store prog for every successor
    vector<shared_ptr<Measure>> progs;

    for (uint32_t successor : (*v).successors) {
        shared_ptr<NodeSpec> w = G->nodes[successor];
        progs.push_back(prog(rho, v, w));
    }

    // Store resulting prog
    shared_ptr<Measure> progResult = progs[0];

    for (int i = 1; i < progs.size(); i++) {
        shared_ptr<Measure> progNext = progs[i];
        bool resultLessOrEqual = progLessOrEqual(progResult, progNext);

        // MIN for even, MAX for odd
        if (((*v).owner == 0 && !resultLessOrEqual) || ((*v).owner == 1 && resultLessOrEqual)) {
            progResult = progNext;
        }
    }

    // Lift rho with resulting prog
    ProgressMeasure rhoLifted = *rho;
    rhoLifted[v] = progResult;

    return make_shared<ProgressMeasure>(rhoLifted);
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