#pragma once
#include<queue>

#include "BitboardsUtils.h"
#include "BoardState.h"

struct Move
{
    uint8_t uStartingSquere;
    uint8_t uDestSquere;
};

// class MoveGenerator
// {
// public:
//     static void GetLegalMovesQueue(const BoardState &state, std::queue<Move> &moves)
//     {
        
//     }
// };