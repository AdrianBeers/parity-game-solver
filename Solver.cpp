#include "Solver.h"
#include <algorithm>
#include <random>

using namespace std;

void Solver::initialize(shared_ptr<ParityGame> parityGame) {
    uint32_t maxPriority = 0;

    for (shared_ptr<NodeSpec> node : G->nodes) {
        uint32_t nextPriority = (*node).priority;

        if (maxPriority < nextPriority) {
            maxPriority = nextPriority;
        }
    }

    d = 1 + maxPriority;

    vector<vector<uint32_t>> M(d);

    for (int i = 0; i < d; i++) {
    }
}

shared_ptr<Measure> Solver::prog(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v, shared_ptr<NodeSpec> &w) {
    uint32_t pv = (*v).priority;
    Measure rhow = (*(*rho)[w]);
    Measure result;

    // Initialize result with zeroes
    vector<uint32_t> result(d, 0);

    // If rhow is tau, the result will be tau as well
    if (rhow.empty()) {
        result.clear();
        return make_shared<Measure>(result);
    }

    // Distinguish even and odd priority of v
    if (pv % 2 == 0) {
        for (int i = 0; i <= pv; i++) {
            result[i] = rhow[i];
        }
    } else {
        for (int i = 0; i <= pv; i++) {
            if (i < pv) {
                // For all elements in the result excluding the last, set it equal to rhow
                result[i] = rhow[i];
            } else {
                for (int j = 0; j < M[pv].size(); j++) {
                    if (rhow[i] == M[pv][j]) {
                        if (j + 1 == M[pv].size()) {
                            // If rho can not be incremented for this priority, the result becomes tau
                            result.clear();
                        } else {
                            // Else, increment rho for this priority
                            result[i] = M[pv][j + 1];
                        }
                    }
                }
            }
        }
    }

    return make_shared<Measure>(result);
}

bool Solver::progLessOrEqual(shared_ptr<Measure> progA, shared_ptr<Measure> progB) {
    // If progB is tau, return true
    if ((*progB).empty()) {
        return true;
    }

    // Else if progA is tau but progB is not, return false
    if ((*progA).empty()) {
        return false;
    }

    // Else if some m_i in progA is larger than m_i in progB, return false
    for (int i = 0; i < d; i++) {
        if ((*progA)[i] > (*progB)[i]) {
            return false;
        }
    }

    // Else, return true
    return true;
}

shared_ptr<ProgressMeasure> Solver::lift(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v) {
    // If measure is already tau, no lift is possible
    if ((*(*rho)[v]).empty()) {
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
        if (!measureRho.empty() && measureRhoLifted.empty()) {
            return false;
        }

        if (!measureRho.empty() && !measureRhoLifted.empty()) {
            for (int i = 0; i < d; i++) {
                // If some m_i was lifted, it is not stabilised
                if (measureRho[i] != measureRhoLifted[i]) {
                    return false;
                }
            }
        }
    }

    // If nothing was lifted, it is stabilised
    return true;
}

shared_ptr<ProgressMeasure> Solver::SPM(Strategy strategy) {
    // Initialize rho with zeroes
    shared_ptr<ProgressMeasure> rho;

    for (shared_ptr<NodeSpec> node : G->nodes) {
        vector<uint32_t> zeroes(d, 0);
        shared_ptr<Measure> measure = make_shared<Measure>(zeroes);

        (*rho)[node] = measure;
    }

    // Perform lifting according to strategy
    switch (strategy) {
    case Strategy::Input: {
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
    case Strategy::Random:
        // Shuffle the nodes first
        vector<shared_ptr<NodeSpec>> nodes = G->nodes;
        shuffle(begin(nodes), end(nodes), default_random_engine{});

        // Variables to keep track of looping
        uint32_t nodesVisited = 0;
        uint32_t nodesStabilised = 0;
        uint32_t nodesTotal = nodes.size();

        while (nodesStabilised < nodesTotal) {
            shared_ptr<ProgressMeasure> rhoLifted = lift(rho, nodes[nodesVisited % nodesTotal]);

            while (!isStabilised(rho, rhoLifted)) {
                // If rho was not already stabilised, reset nodesStabilised
                nodesStabilised = 0;

                // Continue lifting rho
                rho = rhoLifted;
                rhoLifted = lift(rho, nodes[nodesVisited % nodesTotal]);
            }

            nodesVisited++;
            nodesStabilised++;
        }

        return rho;
    }
}