//
// Created by jens on 3-1-2023.
//

#include <string>
#include <memory>

using namespace std;

#ifndef PARITY_GAME_SOLVER_PARSER_H
#define PARITY_GAME_SOLVER_PARSER_H

template<class T>
class Parser {
protected:
    string I;
    int i;

    Parser();

    void expect(const string &e);

    void skipWhiteSpace();

    void requireWhiteSpace();

public:
    virtual shared_ptr<T> parse(string input) = 0;
};

#endif //PARITY_GAME_SOLVER_PARSER_H
