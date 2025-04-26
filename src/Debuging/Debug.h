#pragma once

#include "MoveGenerator/BoardState.h"
#include "MoveGenerator/MoveGenerator.h"

class Debug
{

public:
    static u64 Perft(BoardState &state, int depth);

    static u64 PerftCount(BoardState &state, int depth);
};