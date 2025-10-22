#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <thread>

#include "BoardState.h"

#pragma once

class UniversalChessInterface
{
private:
    bool positionIsSet;
    bool engineIsSherching;
    bool stopUCI;

    BoardState chessBoard;

    std::vector<String> cachedMoves;
    String cachedFEN;

    std::thread engineThread;

public:
    UniversalChessInterface();
    void ExecuteInstructionLine(String line);
    inline bool StopUCI() { return stopUCI; }

private:
    void handleSetPosition(std::stringstream &ss);
    void handleSherch(std::stringstream &ss);

    String toAlgebraicNotation(Move move);
};