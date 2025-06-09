#include "Eval.h"
#include "MoveGenerator.h"

#include <iostream>
#define EQ_BAIAS 120
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
    // return staticEval(board);

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

#include <iostream> // potrzebne do std::cout

Move Eval::FindBestMove(BoardState &board, int depth)
{
    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    Move bestMove = movesList.moves[0];
    int bestEval = minEvalScore;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    std::cout << "Rozważane ruchy dla głębokości " << depth << ":\n";

    for (int i = 0; i < movesList.movesCount; i++)
    {
        Move move = movesList.moves[i];

        board.MakeMove(move);
        int eval = -alphaBeta(board, depth - 1, -beta, -alpha);
        board.UndoMove();

        // Debugowanie ruchów
        // char f1 = 'a' + (move.startingSquere % 8);
        // char r1 = '1' + (move.startingSquere / 8);
        // char f2 = 'a' + (move.destSquere % 8);
        // char r2 = '1' + (move.destSquere / 8);
        // std::cout << "Ruch: " << f1 << r1 << f2 << r2 << "  Wartość: " << eval << "\n";

        if (eval > bestEval)
        {
            bestEval = eval;
            bestMove = move;
        }
        if (eval > alpha)
            alpha = eval;
    }
    char f1 = 'a' + (bestMove.startingSquere % 8);
    char r1 = '1' + (bestMove.startingSquere / 8);
    char f2 = 'a' + (bestMove.destSquere % 8);
    char r2 = '1' + (bestMove.destSquere / 8);
    std::cout << "Ruch: " << f1 << r1 << f2 << r2 << "  Wartość: " << bestEval << "\n";
    return bestMove;
}
