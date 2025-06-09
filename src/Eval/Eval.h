#include "BoardUtils.h"
#include "BoardState.h"
#include "TT.h"

#pragma once

class Eval
{
public:
    static Move FindBestMove(BoardState &board, int depth);

private:
    inline static TT m_TT;

    static int staticEval(const BoardState &board);
    static int alphaBeta(BoardState &board, int depth, int alpha, int beta);
    static int quiescenceSearch(BoardState &board, int alpha, int beta, int depth = 0);

    static constexpr int maxEvalScore = 727379969;
    static constexpr int minEvalScore = -727379969;
    static constexpr int matScore = 5000000;

    static const int PiecesOpeningPositionTable[6][64];
    static const int PiecesEndgamePositionTable[6][64];

    static const int PiecesValues[6];
};