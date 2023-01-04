//
// Created by jens on 4-1-2023.
//

#include <stdexcept>
#include "ParityGame.h"

using namespace std;

ParityGame::ParityGame(uint32_t maxId) {
    this->maxId = maxId;
    this->lastId = 0;

    // Resize node list to hold maxId + 1 elements
    this->nodes.resize(maxId + 1);
}

void ParityGame::addNode(shared_ptr<NodeSpec> &node) {
    if (this->maxId > 0) {
        if (this->lastId >= this->maxId) {
            throw invalid_argument("New node exceeds maximum node ID");
        }

        this->nodes[this->lastId] = node;
    } else {
        this->nodes.push_back(node);
    }

    this->lastId++;
}