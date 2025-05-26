#include "Eval.h"
#include "MoveGenerator.h"

int Eval::staticEval(const BoardState &board)
{
    const u64f *pieces = board.GetBBs();
    int eval = 20 * (-1 * board.GetFlags().whiteOnMove);

    int allPiecesCount = 0;
    for (int i = 0; i < 12; i++)
    {
        u64f piece = pieces[i];
        int piecesCount = std::popcount(piece);
        eval += piecesCount * PiecesValues[i];
        allPiecesCount += piecesCount;
    }

    int isEndGame = ((32 - allPiecesCount) * 1024) / 32;
    for (int i = 0; i < 6; i++)
    {
        u64f piece = pieces[i];
        while (piece)
        {
            int index = __builtin_ctzll(piece);
            piece &= piece - 1;

            eval += ((1024 - isEndGame) * PiecesOpeningPositionTable[i][index] + isEndGame * PiecesEndgamePositionTable[i][index]) / 1024;
        }
    }

    for (int i = 6; i < 12; i++) // Pieces table dla czarnych są odwrócone
    {
        u64f piece = pieces[i];
        while (piece)
        {
            int index = __builtin_ctzll(piece);
            piece &= piece - 1;

            eval += ((1024 - isEndGame) * PiecesOpeningPositionTable[i][64 - index] + isEndGame * PiecesEndgamePositionTable[i][64 - index]) / 1024;
        }
    }

    return eval;
}

int Eval::alphaBeta(BoardState &board, int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (depth == 0)
    {
        return Eval::staticEval(board);
    }

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    if (movesList.movesCount == 0)
    {
        if (board.IsCheckmate())
            return maximizingPlayer ? maxEvalScore : minEvalScore;
        else
            return 0;
    }

    if (maximizingPlayer)
    {
        int maxEval = minEvalScore;
        for (const Move &move : movesList.moves)
        {
            board.MakeMove(move);
            int eval = alphaBeta(board, depth - 1, alpha, beta, false);
            board.UndoMove();

            if (eval > maxEval)
                maxEval = eval;
            if (eval > alpha)
                alpha = eval;

            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else
    {
        int minEval = maxEvalScore;
        for (const Move &move : movesList.moves)
        {
            board.MakeMove(move);
            int eval = alphaBeta(board, depth - 1, alpha, beta, true);
            board.UndoMove();

            if (eval < minEval)
                minEval = eval;
            if (eval < beta)
                beta = eval;

            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

Move Eval::FindBestMove(BoardState &board, int depth)
{
    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    Move bestMove = movesList.moves[0];
    int bestEval = minEvalScore;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    for (int i = 0; i < movesList.movesCount; i++)
    {
        Move move = movesList.moves[i];
        board.MakeMove(move);

        int eval = -alphaBeta(board, depth - 1, -beta, -alpha, false);

        board.UndoMove();

        if (eval > bestEval)
        {
            bestEval = eval;
            bestMove = move;
        }
        if (eval > alpha)
            alpha = eval;
    }

    return bestMove;
}
