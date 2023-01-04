//
// Created by jens on 3-1-2023.
//

#include <stdexcept>
#include "Parser.h"

using namespace std;


/// Basic parsing

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
shared_ptr<T> Parser<T>::parse(string input) {
    I = std::move(input);
    i = 0;
    return nullptr;
}


/// Parity-game parsing

shared_ptr<ParityGame> ParityGameParser::parse(string input) {
    Parser::parse(input);

    // TODO: Read header (if present)
    // TODO: Read node specification lines
}
