//
// Created by jens on 4-1-2023.
//

#include "ParityGame.h"
#include <stdexcept>

using namespace std;

ParityGame::ParityGame() {
    this->maxId = 0;
    this->hasMaxId = false;
}

ParityGame::ParityGame(uint32_t maxId) {
    this->maxId = maxId;
    this->hasMaxId = true;

    // Resize node list to hold maxId + 1 elements
    this->nodes.resize(maxId + 1);
}

void ParityGame::addNode(shared_ptr<NodeSpec> &node) {
    if (this->hasMaxId) {
        if (node->id > this->maxId) {
            throw invalid_argument("New node exceeds maximum node ID");
        }
    } else if (this->nodes.size() < (node->id + 1)) {
        this->nodes.resize(node->id + 1);
    }

    this->nodes[node->id] = node;
}