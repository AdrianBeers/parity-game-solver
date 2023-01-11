#include "Solver.h"
#include <algorithm>
#include <random>

using namespace std;

void Solver::initialize(shared_ptr<ParityGame> &parityGame) {
    G = parityGame;

    // Find maximum node priority of all nodes
    uint32_t maxPriority = 0;
    for (auto &node: G->nodes) {
        uint32_t nextPriority = node->priority;
        if (maxPriority < nextPriority) {
            maxPriority = nextPriority;
        }
    }

    // Set value d
    d = 1 + maxPriority;

    // Find maximum value of M, by counting the number of nodes with a given priority
    Measure maxM(d, 0);
    for (int i = 0; i < d; i++) {
        // Skip even positions
        if (i % 2 == 0) {
            continue;
        }

        // Find all nodes with priority i
        for (auto &node: G->nodes) {
            if (node->priority == i) {
                maxM[i]++;
            }
        }
    }

    M = maxM;
}

shared_ptr<Measure> Solver::prog(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v, shared_ptr<NodeSpec> &w) {
    uint32_t pv = v->priority;
    Measure rhow = (*(*rho)[w]);

    // Initialize m with zeroes
    vector<uint32_t> m(d, 0);

    // If rhow is tau, m will be tau as well
    if (rhow.empty()) {
        m.clear();
        return make_shared<Measure>(m);
    }

    // Distinguish even and odd priority of v
    if (pv % 2 == 0) {
        // For all elements in m up to pv, set it equal to corresponding element in rhow
        for (int i = 0; i <= pv; i++) {
            m[i] = rhow[i];
        }
    } else {
        for (int i = 0; i <= pv; i++) {
            if (i < pv) {
                // For all elements in m prior to pv, set it equal to corresponding element in rhow
                m[i] = rhow[i];
            } else {
                // For the last element, increment it if possible, otherwise lift m to tau
                if (rhow[i] < M[pv]) {
                    m[i] = rhow[i] + 1;
                } else {
                    m.clear();
                }
            }
        }
    }

    return make_shared<Measure>(m);
}

bool Solver::progLessOrEqual(shared_ptr<Measure> &progA, shared_ptr<Measure> &progB) const {
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

    for (uint32_t successor: (*v).successors) {
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
    for (shared_ptr<NodeSpec> &node: G->nodes) {
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

shared_ptr<ProgressMeasure> Solver::SPM(LiftStrategy strategy) {
    // Initialize rho with zeroes
    shared_ptr<ProgressMeasure> rho = make_shared<ProgressMeasure>();
    for (shared_ptr<NodeSpec> &node: G->nodes) {
        shared_ptr<Measure> measure = make_shared<Measure>(d, 0);
        (*rho)[node] = measure;
    }

    // Shuffle the nodes first for random strategy
    vector<shared_ptr<NodeSpec>> nodes = G->nodes;
    if (strategy == LiftStrategy::Random) {
        shuffle(begin(nodes), end(nodes), default_random_engine{});
    }

    // Perform lifting according to strategy
    switch (strategy) {
        case LiftStrategy::Input:
        case LiftStrategy::Random: {
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
        default:
            return rho;
    }
}