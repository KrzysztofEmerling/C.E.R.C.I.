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

    static int CalculateTimeToSearch(const int moveNr, const int wtime, const int btime, const int winc, const int binc, const bool isWhiteTurn);

private:
    static std::atomic<bool> m_StopSearch;
    inline static TT m_TT;

    static int staticEval(const BoardState &board);

    static int alphaBeta(BoardState &board, int alpha, int beta, int depth, int ref_depth = 1);
    static int quiescenceSearch(BoardState &board, int alpha, int beta, int depth, int ref_depth);

    static int scoreMove(const BoardState &board, const Move &move);

    static constexpr int maxEvalScore = 727379969;
    static constexpr int minEvalScore = -727379969;
    static constexpr int matScore = 5000000;
    static constexpr u64 FILE_MASKS[8] = {
        0x0101010101010101ULL,
        0x0202020202020202ULL,
        0x0404040404040404ULL,
        0x0808080808080808ULL,
        0x1010101010101010ULL,
        0x2020202020202020ULL,
        0x4040404040404040ULL,
        0x8080808080808080ULL};

    static const int PiecesOpeningPositionTable[6][64];
    static const int PiecesEndgamePositionTable[6][64];
    static const int PiecesValues[6];
};