#include "BoardUtils.h"
#include "BoardState.h"

#pragma once

class Eval
{
public:
    static int staticEval(const BoardState &board);
    static int alphaBeta(BoardState &board, int depth, int alpha, int beta, bool maximizingPlayer);
    static Move FindBestMove(BoardState &board, int depth);

private:
    static constexpr int maxEvalScore = 727379969;
    static constexpr int minEvalScore = -727379969;

    static const int PiecesValues[12];

    static const int PiecesOpeningPositionTable[6][64];
    static const int PiecesEndgamePositionTable[6][64];
};