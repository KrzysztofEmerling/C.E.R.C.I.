#include "BoardState.h"
#include "TT.h"

#include <thread>

#pragma once

class Eval
{
public:
    static void PrepareForNewGame();
    static Move FindBestMoveFixedDepth(BoardState &board, int depth);
    static Move FindBestMove_MCTS(BoardState &board, int msToThink);

    static Move FindBestMove(BoardState &board);
    static void StopSearch();

private:
    static std::atomic<bool> m_StopSearch;
    inline static TT m_TT;

    static int staticEval(const BoardState &board);
    static int alphaBeta(BoardState &board, int depth, int alpha, int beta);
    static int quiescenceSearch(BoardState &board, int alpha, int beta, int depth = 0);

    static int scoreMove(const BoardState &board, const Move &move);

    static constexpr int maxEvalScore = 727379969;
    static constexpr int minEvalScore = -727379969;
    static constexpr int matScore = 5000000;

    static const int PiecesOpeningPositionTable[6][64];
    static const int PiecesEndgamePositionTable[6][64];

    static const int PiecesValues[6];
};