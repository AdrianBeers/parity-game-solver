//
// Created by jens on 4-1-2023.
//

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

using namespace std;

#ifndef PARITY_GAME_SOLVER_PARITYGAME_H
#define PARITY_GAME_SOLVER_PARITYGAME_H

struct NodeSpec {
    uint32_t id;
    uint32_t priority;
    uint8_t owner;
    vector<uint32_t> successors;
    string name;
};

class ParityGame {
    uint32_t maxId;
    vector<shared_ptr<NodeSpec>> nodes;

    void addNode(shared_ptr<NodeSpec> &node);

    explicit ParityGame(uint32_t maxId = 0);

private:
    uint32_t lastId;
};


#endif //PARITY_GAME_SOLVER_PARITYGAME_H
