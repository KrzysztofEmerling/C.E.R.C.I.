#include "Eval.h"
#include "MoveGenerator.h"
#include <algorithm>
#include <iostream>
#include <chrono>

#define EQ_BAIAS 217
std::atomic<bool> Eval::m_StopSearch = false;

int Eval::staticEval(const BoardState &board)
{
    const u64f *pieces = board.GetBBs();
    int eval = 0;
    int allPiecesCount = 0;

    for (int i = 0; i < 5; i++)
    {
        int wCount = std::popcount(pieces[i]);
        int bCount = std::popcount(pieces[i + 6]);

        allPiecesCount += wCount + bCount;
        eval += (wCount - bCount) * PiecesValues[i];
    }
    if (allPiecesCount > 32)
        allPiecesCount = 32;
    int isEndGame = ((32 - allPiecesCount) * 1024) / 32;
    int activityScore = 0;

    for (int i = 0; i < 6; i++)
    {
        u64f wPiece = pieces[i];
        u64f bPiece = pieces[i + 6];

        // Białe
        while (wPiece)
        {
            int index = __builtin_ctzll(wPiece);
            wPiece &= wPiece - 1;
            activityScore += ((1024 - isEndGame) * PiecesOpeningPositionTable[i][index] + isEndGame * PiecesEndgamePositionTable[i][index]) / 2048;
        }
        // Czarne
        while (bPiece)
        {
            int index = __builtin_ctzll(bPiece);
            bPiece &= bPiece - 1;
            int mirroredIndex = index ^ 56;
            activityScore -= ((1024 - isEndGame) * PiecesOpeningPositionTable[i][mirroredIndex] + isEndGame * PiecesEndgamePositionTable[i][mirroredIndex]) / 2048;
        }
    }
    eval += activityScore;

    return board.IsWhiteMove()
               ? eval + EQ_BAIAS
               : -eval + EQ_BAIAS;
}

int Eval::alphaBeta(BoardState &board, int depth, int alpha, int beta)
{
    if (board.IsFiftyMoveRule() || board.IsInsufficientMaterial() || board.IsStalemate() || board.IsThreefoldRepetition())
        return 0;
    if (board.IsCheckmate())
        return -matScore - depth;

    if (depth == 0)
        return quiescenceSearch(board, alpha, beta);

    int bestEval = minEvalScore;
    if (m_TT.probe(board.GetHash(), depth, bestEval))
        return bestEval;

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    for (size_t i = 0; i < movesList.movesCount; i++)
    {
        board.MakeMove(movesList.moves[i]);
        int eval = -alphaBeta(board, depth - 1, -beta, -alpha);
        board.UndoMove();

        if (eval > bestEval)
            bestEval = eval;

        if (eval > alpha)
            alpha = eval;

        if (alpha >= beta) // Beta cutoff
            break;
    }

    m_TT.store(board.GetHash(), depth, bestEval);
    return bestEval;
}
int Eval::quiescenceSearch(BoardState &board, int alpha, int beta, int depth)
{
    if (board.IsCheckmate())
        return -matScore - depth;

    if (m_TT.probe(board.GetHash(), depth, alpha))
        return alpha;

    int qEval = staticEval(board);
    if (qEval >= beta)
        return beta;
    if (qEval > alpha)
        alpha = qEval;

    // zabezpieczenie przed eksplozją głębokości
    if (depth <= -8)
        return alpha;

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    for (size_t i = 0; i < movesList.movesCount; i++)
    {
        if (movesList.moves[i].categorie != Capture)
            continue;

        board.MakeMove(movesList.moves[i]);
        int eval = -quiescenceSearch(board, -beta, -alpha, depth - 1);
        board.UndoMove();

        if (eval >= beta)
            return beta;
        if (eval > alpha)
            alpha = eval;
    }

    m_TT.store(board.GetHash(), depth, alpha);
    return alpha;
}

Move Eval::FindBestMoveFixedDepth(BoardState &board, int depth)
{
    m_StopSearch = false;

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    std::pair<Move, int> moveScores[218];
    int count = movesList.movesCount;

    for (int i = 0; i < count; ++i)
    {
        Move move = movesList.moves[i];
        moveScores[i] = {move, scoreMove(move)};
    }

    std::sort(moveScores, moveScores + count,
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    Move bestMove = moveScores[0].first;
    int bestEval = minEvalScore;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    for (int i = 0; i < count; ++i)
    {
        if (m_StopSearch)
            break;

        const Move &move = moveScores[i].first;
        board.MakeMove(move);
        int eval = -alphaBeta(board, depth - 1, -beta, -alpha);
        board.UndoMove();

        if (eval > bestEval)
        {
            bestEval = eval;
            bestMove = move;
        }

        if (eval > alpha)
            alpha = eval;

        if (alpha >= beta)
            break;
    }

    return bestMove;
}

Move Eval::FindBestMove(BoardState &board, int msToThink)
{
    m_StopSearch = false;
    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    std::pair<Move, int> moveScores[218];
    int count = movesList.movesCount;
    for (int i = 0; i < count; ++i)
    {
        Move move = movesList.moves[i];
        moveScores[i] = {move, scoreMove(move)};
    }

    std::sort(moveScores, moveScores + count,
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    Move bestMove = moveScores[0].first;
    int bestEval = minEvalScore;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    std::thread timerThread([msToThink]()
                            {
        std::this_thread::sleep_for(std::chrono::milliseconds(msToThink));
        m_StopSearch = true; });
    for (int depth = 1; depth <= 100; ++depth)
    {
        Move currentBest = bestMove;
        int currentBestEval = minEvalScore;

        for (int i = 0; i < count; ++i)
        {
            if (m_StopSearch)
                break;

            board.MakeMove(moveScores[i].first);
            int eval = -alphaBeta(board, depth - 1, -beta, -alpha);
            board.UndoMove();

            moveScores[i].second = eval;

            if (eval > currentBestEval)
            {
                currentBestEval = eval;
                currentBest = moveScores[i].first;
            }
            if (eval > alpha)
                alpha = eval;
        }

        if (m_StopSearch)
        {
            bestMove = currentBest;
            bestEval = currentBestEval;

            char f1 = 'a' + (bestMove.startingSquere % 8);
            char r1 = '1' + (bestMove.startingSquere / 8);
            char f2 = 'a' + (bestMove.destSquere % 8);
            char r2 = '1' + (bestMove.destSquere / 8);
            std::cout << "osiągnięta Głębokość: " << depth
                      << "  Najlepszy ruch: " << f1 << r1 << f2 << r2
                      << "  Ocena: " << bestEval << "\n";
            break;
        }

        std::sort(moveScores, moveScores + count,
                  [](const auto &a, const auto &b)
                  { return a.second > b.second; });
    }

    if (timerThread.joinable())
        timerThread.join();

    return bestMove;
}

int Eval::scoreMove(const Move &move)
{
    int score = 0;

    if (move.categorie & Promotion)
    {
        if (move.flag == PromotionQueen)
            score += 9000;
        else if (move.flag == PromotionRook)
            score += 8000;
        else if (move.flag == PromotionBishop)
            score += 7000;
        else if (move.flag == PromotionKnight)
            score += 6000;
    }

    if (move.categorie & Capture)
        score += 5000;

    if (move.flag == Castling)
        score += 3000;

    if (move.categorie & Killer)
        score += 2000;

    if (move.categorie & Quiet)
        score += 1000;

    return score;
}
