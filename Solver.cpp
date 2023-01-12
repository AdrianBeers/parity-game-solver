#include "Solver.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <stack>
#include <queue>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;


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
    shared_ptr<Measure> &rhow = rho->at(w->id);

    // Initialize m with zeroes
    vector<uint32_t> m(d, 0);

    // If rhow is tau, m will be tau as well
    if (rhow->empty()) {
        m.clear();
        return make_shared<Measure>(m);
    }

    // Distinguish even and odd priority of v
    if (pv % 2 == 0) {
        // For all elements in m up to pv, set it equal to corresponding element in rhow
        for (int i = 0; i <= pv; i++) {
            m[i] = rhow->at(i);
        }
    } else {
        for (int i = 0; i <= pv; i++) {
            if (i < pv) {
                // For all elements in m prior to pv, set it equal to corresponding element in rhow
                m[i] = rhow->at(i);
            } else {
                // For the last element, increment it if possible, otherwise lift m to tau
                if (rhow->at(i) < M[pv]) {
                    m[i] = rhow->at(i) + 1;
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
    if (rho->at(v->id)->empty()) {
        return rho;
    }

    // Store prog for every successor
    vector<shared_ptr<Measure>> progs;
    for (uint32_t successor: v->successors) {
        shared_ptr<NodeSpec> w = G->nodes[successor];
        progs.push_back(prog(rho, v, w));
    }

    // Store resulting prog
    shared_ptr<Measure> progResult = progs[0];
    for (int i = 1; i < progs.size(); i++) {
        shared_ptr<Measure> progNext = progs[i];
        bool resultLessOrEqual = progLessOrEqual(progResult, progNext);

        // MIN for even, MAX for odd
        if ((v->owner == 0 && !resultLessOrEqual) || (v->owner == 1 && resultLessOrEqual)) {
            progResult = progNext;
        }
    }

    // Lift rho with the max between rhov and the resulting prog
    shared_ptr<ProgressMeasure> rhoLifted = make_shared<ProgressMeasure>(*rho);
    if (progLessOrEqual(rho->at(v->id), progResult)) {
        // Lift rho with resulting prog
        (*rhoLifted)[v->id] = progResult;
    }

    return rhoLifted;
}

shared_ptr<ProgressMeasure> Solver::SPM(LiftStrategy strategy) {
    // Initialize rho with zeroes
    shared_ptr<ProgressMeasure> rho = make_shared<ProgressMeasure>();
    for (shared_ptr<NodeSpec> &node: G->nodes) {
        shared_ptr<Measure> measure = make_shared<Measure>(d, 0);
        (*rho)[node->id] = measure;
    }

    // Shuffle the nodes first for random strategy
    vector<shared_ptr<NodeSpec>> nodes = G->nodes;
    if (strategy == LiftStrategy::Random) {
        default_random_engine engine{};
        engine.seed(system_clock::now().time_since_epoch().count());
        shuffle(begin(nodes), end(nodes), engine);
    }

    uint32_t n_lifts = 0;

    // Perform lifting according to strategy
    switch (strategy) {
        case LiftStrategy::Input:
        case LiftStrategy::Random: {
            // Variables to keep track of looping
            uint32_t nodesVisited = 0,
                    nodesStabilised = 0,
                    nodesTotal = nodes.size();

            while (nodesStabilised < nodesTotal) {
                const uint32_t idx = nodesVisited % nodesTotal;
                const shared_ptr<NodeSpec> &node = nodes[idx];

                // Lift node once
                shared_ptr<ProgressMeasure> rhoLifted = lift(rho, nodes[node->id]);
                n_lifts++;

                // Keep on lifting node until it is stabilised
                while ((*rho->at(node->id)) != (*rhoLifted->at(node->id))) {
                    // If rho was not already stabilised, reset nodesStabilised
                    nodesStabilised = 0;

                    // Continue lifting rho
                    rho = rhoLifted;
                    rhoLifted = lift(rho, nodes[nodesVisited % nodesTotal]);
                    n_lifts++;
                }

                nodesVisited++;
                nodesStabilised++;
            }
        }
        case LiftStrategy::Predecessor:
        case LiftStrategy::PredecessorMax: {
            vector<bool> queued(G->nodes.size());
            priority_queue<pair<vector<uint32_t>, uint32_t>> queue;
            stack<uint32_t> stack;
            vector<vector<uint32_t>> predecessors(G->nodes.size());

            for (shared_ptr<NodeSpec> const &node: G->nodes) {
                if (!rho->at(node->id)->empty()) {
                    if (strategy == LiftStrategy::PredecessorMax) {
                        queue.emplace((*rho->at(node->id)), node->id);
                    } else {
                        stack.push(node->id);
                    }
                    queued[node->id] = true;
                }

                // each successor has this node as a predecessor
                // create the list of predecessors
                for (auto successor: node->successors) {
                    predecessors[successor].push_back(node->id);
                }
            }

            while (strategy == LiftStrategy::PredecessorMax ? !queue.empty() : !stack.empty()) {

                uint32_t nodeId = (strategy == LiftStrategy::PredecessorMax ?
                                   queue.top().second : stack.top());

                strategy == LiftStrategy::PredecessorMax ? queue.pop() : stack.pop();

                queued[nodeId] = false;

                shared_ptr<NodeSpec> &node = G->nodes[nodeId];

                shared_ptr<ProgressMeasure> rhoLifted = lift(rho, node);
                n_lifts++;


                if ((*rho->at(node->id)) != (*rhoLifted->at(node->id))) {
                    for (auto predecessor: predecessors[nodeId]) {
                        if (!queued[predecessor] && !rhoLifted->at(G->nodes[predecessor]->id)->empty()) {
                            queued[predecessor] = true;

                            if (strategy == LiftStrategy::PredecessorMax) {
                                queue.emplace(*rho->at(G->nodes[predecessor]->id), predecessor);
                            } else {
                                stack.push(predecessor);
                            }
                        }
                    }

                    (*rho)[node->id] = rhoLifted->at(node->id);
                }
            }
        }
        case LiftStrategy::FocusList: {
            uint32_t phase = 1;
            uint32_t num_attempts = 0;
            uint32_t num_failed = 0;
            uint32_t next_vertex = 0;
            const uint32_t &V = G->nodes.size();
            uint32_t max_size = V;
            queue<pair<uint32_t, uint32_t>> focus_list;

            while (true) {
                num_attempts++;
                shared_ptr<NodeSpec> &node = G->nodes[next_vertex];
                if (phase == 1) {
                    shared_ptr<ProgressMeasure> rhoLifted = lift(rho, node);
                    n_lifts++;

                    if ((*rho->at(node->id)) != (*rhoLifted->at(node->id))) {
                        num_failed = 0;
                        (*rho)[node->id] = rhoLifted->at(node->id);
                        focus_list.emplace(next_vertex, 2);
                    } else {
                        num_failed++;
                    }
                    next_vertex = (next_vertex + 1) % V;

                    if (num_failed == V) {
                        break;
                    }

                    if (num_attempts == V || focus_list.size() == max_size) {
                        phase = 2;
                        num_attempts = 0;
                    }

                } else {
                    pair<uint32_t, uint32_t> node_credit = focus_list.front();
                    focus_list.pop();

                    shared_ptr<NodeSpec> &node2 = G->nodes[node_credit.first];
                    shared_ptr<ProgressMeasure> rhoLifted = lift(rho, node2);
                    n_lifts++;

                    if ((*rho->at(node2->id)) != (*rhoLifted->at(node2->id))) {
                        focus_list.emplace(node2->id, node_credit.second + 2);
                        (*rho)[node2->id] = rhoLifted->at(node2->id);
                    } else if (node_credit.second > 0) {
                        focus_list.emplace(node2->id, node_credit.second / 2);
                    }

                    if (focus_list.empty() || num_attempts == V) {
                        if (!focus_list.empty()) {
                            queue<pair<uint32_t, uint32_t>> empty;
                            swap(focus_list, empty);
                        }
                        phase = 1;
                        num_attempts = 0;
                    }
                }
            }
        }
    }

    cout << "Number of lifts: " << n_lifts << endl;
    return rho;
}