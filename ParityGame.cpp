//
// Created by jens on 4-1-2023.
//

#include "ParityGame.h"
#include <stdexcept>

using namespace std;

ParityGame::ParityGame(uint32_t maxId) {
    this->maxId = maxId;

    // Resize node list to hold maxId + 1 elements
    this->nodes.resize(maxId + 1);
}

void ParityGame::addNode(shared_ptr<NodeSpec> &node) {
    if (this->maxId > 0) {
        if (node->id > this->maxId) {
            throw invalid_argument("New node exceeds maximum node ID");
        }

        this->nodes[node->id] = node;
    } else {
        // TODO: Remove existing node with specified ID
        this->nodes.push_back(node);
    }
}