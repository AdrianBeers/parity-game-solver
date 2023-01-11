//
// Created by jens on 4-1-2023.
//

#include <cstdint>
#include <memory>
#include <string>
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
public:
    uint32_t maxId;
    vector<shared_ptr<NodeSpec>> nodes;

    void addNode(shared_ptr<NodeSpec> &node);

    explicit ParityGame();
    explicit ParityGame(uint32_t maxId);
private:
    bool hasMaxId;
};

#endif // PARITY_GAME_SOLVER_PARITYGAME_H
