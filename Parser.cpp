//
// Created by jens on 3-1-2023.
//

#include <stdexcept>
#include <iostream>
#include "Parser.h"

using namespace std;


/// Basic parsing

bool digitFirst(char c) { return c >= '0' && c <= '9'; }

template<class T>
Parser<T>::Parser() {
    I = "";
    i = 0;
}

template<class T>
void Parser<T>::expect(const string &e) {
    if (I.substr(i, e.length()) != e) {
        throw invalid_argument("Parse exception at position " + to_string(i) + ": was expecting '" + e + "'");
    } else {
        i += e.length();
    }
}

template<class T>
void Parser<T>::skipWhiteSpace() {
    bool skipRestOfLine = false;
    while (i < I.length()) {
        // Handle comments
        if (I[i] == '%') {
            skipRestOfLine = true;
            i++;
        }

        // Skip rest of line
        if (skipRestOfLine) {
            if (I[i] == '\n') {
                i++;
                skipRestOfLine = false;
                continue;
            } else {
                i++;
                continue;
            }
        }

        // Skip whitespace
        if (I[i] == ' ' || I[i] == '\t' || I[i] == '\n' || I[i] == '\r') {
            i++;
            continue;
        }

        break;
    }
}

template<class T>
void Parser<T>::requireWhiteSpace() {
    if (i >= I.length() || (I[i] != ' ' && I[i] != '\t')) {
        throw invalid_argument("Parse exception at position " + to_string(i) + ": was expecting whitespace");
    }

    skipWhiteSpace();
}

template<class T>
uint32_t Parser<T>::parseUnsignedInt32() {
    if (!digitFirst(I[i])) {
        throw invalid_argument("Parse exception at position " + to_string(i) + ": was expecting digit");
    }

    string n;
    do {
        n.push_back(I[i]);
        i++;
    } while (digitFirst(I[i]));

    return stoul(n);
}

template<class T>
shared_ptr<T> Parser<T>::parse(string input) {
    I = std::move(input);
    i = 0;
    return nullptr;
}


/// Parity-game parsing

ParityGameParser::ParityGameParser() : Parser<ParityGame>() {}

shared_ptr<ParityGame> ParityGameParser::parse(string input) {
    Parser::parse(input);

    // Skip initial whitespace and comments
    skipWhiteSpace();

    // Read header containing maximum identifier(if present)
    uint32_t maxId = 0;
    if (I[i] == 'p') {
        expect("parity ");
        maxId = parseUnsignedInt32();
        expect(";");
    }

    // Initialize parity game
    shared_ptr<ParityGame> pg = make_shared<ParityGame>(maxId);

    // Read node specification lines
    skipWhiteSpace();
    while (i < I.length()) {
        shared_ptr<NodeSpec> node = make_shared<NodeSpec>();
        node->id = parseUnsignedInt32();

        requireWhiteSpace();
        node->priority = parseUnsignedInt32();

        requireWhiteSpace();
        node->owner = parseUnsignedInt32();
        if (node->owner != 0 && node->owner != 1) {
            throw invalid_argument("Parse exception at position " + to_string(i) = ": invalid node owner value");
        }

        requireWhiteSpace();
        do {
            node->successors.push_back(parseUnsignedInt32());
        } while (I[i] == ',' && i++);

        skipWhiteSpace();
        if (I[i] == '"') {
            i++;
            for (; I[i] != '"'; i++) {
                node->name += I[i];
            }
            i++;
        }

        expect(";");
        skipWhiteSpace();

        pg->addNode(node);
    }

    return pg;
}
