//
// Created by jens on 3-1-2023.
//

#include <string>
#include <memory>
#include "ParityGame.h"

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

    uint32_t parseUnsignedInt32();

public:
    virtual shared_ptr<T> parse(string input);
};

class ParityGameParser : Parser<ParityGame> {
public:
    ParityGameParser();
    shared_ptr<ParityGame> parse(string input) override;
};

#endif //PARITY_GAME_SOLVER_PARSER_H
