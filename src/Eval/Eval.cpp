#include "Eval.h"
#include "MoveGenerator.h"
#include "MCTS.h"

#include <algorithm>
#include <iostream>
#include <chrono>

// #define EQ_BAIAS 217
#define EQ_BAIAS 0
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
        moveScores[i] = {move, scoreMove(board, move)};
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

        // char f1 = 'a' + (bestMove.startingSquere % 8);
        // char r1 = '1' + (bestMove.startingSquere / 8);
        // char f2 = 'a' + (bestMove.destSquere % 8);
        // char r2 = '1' + (bestMove.destSquere / 8);

        // std::cout << "Ruch: " << f1 << r1 << f2 << r2 << ", Eval: " << bestEval << "\n";
    }
    char f1 = 'a' + (bestMove.startingSquere % 8);
    char r1 = '1' + (bestMove.startingSquere / 8);
    char f2 = 'a' + (bestMove.destSquere % 8);
    char r2 = '1' + (bestMove.destSquere / 8);

    std::cout << "Ruch: " << f1 << r1 << f2 << r2 << ", Eval: " << bestEval << "\n";
    return bestMove;
}

Move Eval::FindBestMove_MCTS(BoardState &board, int msToThink)
{
    using namespace std::chrono;
    auto startTime = high_resolution_clock::now();

    MCTS_node root(nullptr);

    // Pierwsze poziomowe rozwinięcie
    MoveList rootMoves;
    MoveGenerator::GetLegalMoves(board, rootMoves);

    for (int i = 0; i < rootMoves.movesCount; ++i)
    {
        root.AddChild(rootMoves.moves[i]);
    }

    BoardState state = board;
    while (duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() < msToThink)
    {
        MCTS_node *node = &root;

        // 1. Selection
        while (!node->IsLeaf())
        {
            double parentVisits = node->GetVisitCount();
            double bestScore = -1.0;
            MCTS_node *bestChild = nullptr;

            for (int i = 0; i < node->GetChildrenCount(); ++i)
            {
                MCTS_node *child = node->GetChild(i);
                double score = child->GetUCB(parentVisits);
                if (score > bestScore)
                {
                    bestScore = score;
                    bestChild = child;
                }
            }

            node = bestChild;
            state.MakeMove(node->GetMove());
        }

        // 2. Expansion
        MoveList moves;
        MoveGenerator::GetLegalMoves(state, moves);

        if (moves.movesCount > 0 && !state.IsCheckmate())
        {
            // Posortuj ruchy wg heurystyki
            std::pair<Move, int> scoredMoves[218];
            for (int i = 0; i < moves.movesCount; ++i)
                scoredMoves[i] = {moves.moves[i], scoreMove(board, moves.moves[i])};

            std::sort(scoredMoves, scoredMoves + moves.movesCount,
                      [](const auto &a, const auto &b)
                      { return a.second > b.second; });

            int topN = (moves.movesCount < 4) ? moves.movesCount : 4;
            int pick = rand() % topN;
            Move move = scoredMoves[pick].first;

            MCTS_node *newNode = node->AddChild(move);
            node = newNode;
            state.MakeMove(move);
        }

        // 3. Simulation
        int result = quiescenceSearch(state, -matScore, matScore);
        // 4. Backpropagation
        while (node != nullptr)
        {
            node->UpdateStats(result);
            node = node->GetParent();
        }
    }

    // Po zakończeniu: wybierz najczęściej odwiedzany ruch

    // Odwiedziny
    // MCTS_node *bestChild = nullptr;
    // int bestVisits = -1;
    // for (int i = 0; i < root.GetChildrenCount(); ++i)
    // {
    //     MCTS_node *child = root.GetChild(i);
    //     if (child->GetVisitCount() > bestVisits)
    //     {
    //         bestVisits = child->GetVisitCount();
    //         bestChild = child;
    //     }
    // }

    // Evaluacja
    MCTS_node *bestChild = nullptr;
    int bestEval = minEvalScore;
    for (int i = 0; i < root.GetChildrenCount(); ++i)
    {
        MCTS_node *child = root.GetChild(i);
        if (child->GetEvalAverage() > bestEval)
        {
            bestEval = child->GetVisitCount();
            bestChild = child;
        }
    }
    Move move = bestChild->GetMove();
    char f1 = 'a' + (move.startingSquere % 8);
    char r1 = '1' + (move.startingSquere / 8);
    char f2 = 'a' + (move.destSquere % 8);
    char r2 = '1' + (move.destSquere / 8);

    std::cout << "Ruch: " << f1 << r1 << f2 << r2 << "BestEval: " << bestEval << "\n";
    return move;
}

int Eval::scoreMove(const BoardState &board, const Move &move)
{
    int score = 0;

    bool isWhiteMove = board.GetFlags().whiteOnMove;
    int movingPiece = board.FindPieceAt(BitboardsIndecies[move.startingSquere], isWhiteMove) % 6;

    // if (movingPiece == 5) // zachęcenie do roszady
    //     score += (BitboardsIndecies[move.destSquere] & 0xbbffffffffffffbb == 0) ? 1000 : -1000;
    // else // aktywność figór
    if (movingPiece != 5)
        score += (BitboardsIndecies[move.destSquere] & 0xffffc38181c3ffff == 0) ? 200 : 0;

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
    {
        score += 5000;
        int victime = board.FindPieceAt(BitboardsIndecies[move.destSquere], !isWhiteMove) % 6;
        score += victime - movingPiece;
    }

    if (move.flag == Castling)
        score += 3000;

    if (move.categorie & Killer)
        score += 2000;

    if (move.categorie & Quiet)
        score += 1000;

    return score;
}
