#include "Eval.h"
#include "MoveGenerator.h"
#include "DNN.h"
#include "MCTS.h"

#include <algorithm>
#include <iostream>
#include <chrono>

#define EQ_BAIAS 0
std::atomic<bool> Eval::m_StopSearch = false;
void Eval::PrepareForNewGame()
{
    DNN::LoadWeights("/home/krzysztof/Documents/C.E.R.C.I./src/Eval/weightsAgersiveMCTSa.bin");
    m_TT.Clear();
}

int Eval::staticEval(const BoardState &board)
{
    float eval = DNN::FitForward(board) * 5000.0f;

    // // Imperatywna evaluacja
    // const u64f *pieces = board.GetBBs();
    // int eval = 0;
    // int allPiecesCount = 0;
    // for (int i = 0; i < 5; i++)
    // {
    //     int wCount = std::popcount(pieces[i]);
    //     int bCount = std::popcount(pieces[i + 6]);

    //     allPiecesCount += wCount + bCount;
    //     eval += (wCount - bCount) * PiecesValues[i];
    // }
    // bool whiteMaterialAdventage = eval > 0;

    // if (allPiecesCount > 32)
    //     allPiecesCount = 32;
    // int isEndGame = ((32 - allPiecesCount) * 1024) / 32;
    // int activityScore = 0;

    // for (int i = 0; i < 6; i++)
    // {
    //     u64f wPiece = pieces[i];
    //     u64f bPiece = pieces[i + 6];

    //     // Białe
    //     while (wPiece)
    //     {
    //         int index = __builtin_ctzll(wPiece);
    //         wPiece &= wPiece - 1;
    //         activityScore += ((1024 - isEndGame) * PiecesOpeningPositionTable[i][index] + isEndGame * PiecesEndgamePositionTable[i][index]) / 1536;
    //     }
    //     // Czarne
    //     while (bPiece)
    //     {
    //         int index = __builtin_ctzll(bPiece);
    //         bPiece &= bPiece - 1;
    //         int mirroredIndex = index ^ 56;
    //         activityScore -= ((1024 - isEndGame) * PiecesOpeningPositionTable[i][mirroredIndex] + isEndGame * PiecesEndgamePositionTable[i][mirroredIndex]) / 1536;
    //     }
    // }
    // eval += activityScore;
    // u64f wKingBB = pieces[5];
    // u64f bKingBB = pieces[11];

    // // Ading agresive playstyle
    // u64 blockingPawns = pieces[6] | pieces[0];
    // u64 notBlockers = ~blockingPawns;

    // u64 kingDangereZone = MoveGenerator::GetPseudoLegalKingBBs(bKingBB);

    // u64 lightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(pieces[1]);
    // lightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(pieces[2], blockingPawns) & notBlockers;
    // u64 heavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(pieces[3], blockingPawns) & notBlockers;
    // heavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(pieces[4], blockingPawns) & notBlockers;

    // int lightAttackers = std::popcount(kingDangereZone & lightAttackedSquers);
    // int heavyAttackers = std::popcount(kingDangereZone & heavyAttackedSquers);
    // eval += lightAttackers * 45 + heavyAttackers * 60;

    // kingDangereZone = MoveGenerator::GetPseudoLegalKingBBs(wKingBB);

    // lightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(pieces[7]);
    // lightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(pieces[8], blockingPawns) & notBlockers;
    // heavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(pieces[9], blockingPawns) & notBlockers;
    // heavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(pieces[10], blockingPawns) & notBlockers;

    // lightAttackers = std::popcount(kingDangereZone & lightAttackedSquers);
    // heavyAttackers = std::popcount(kingDangereZone & heavyAttackedSquers);
    // eval -= lightAttackers * 45 + heavyAttackers * 60;
    // // end

    // // Adding Positional playstyle
    // if (allPiecesCount < 31)
    // {
    //     u64 allBlockers = pieces[0] | pieces[1] | pieces[2] | pieces[3] | pieces[4] | pieces[6] | pieces[7] | pieces[8] | pieces[9] | pieces[10];
    //     u64 notBlockers = ~allBlockers;

    //     u64 wPawnControled = MoveGenerator::GetWhitePawnsAttacksBBs(pieces[BitBoardsIndecis::WhitePawns]);
    //     u64 wLightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(pieces[BitBoardsIndecis::WhiteKnights]);
    //     wLightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(pieces[BitBoardsIndecis::WhiteBishops], allBlockers) & notBlockers;
    //     u64 wHeavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(pieces[BitBoardsIndecis::WhiteRooks], allBlockers) & notBlockers;
    //     wHeavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(pieces[BitBoardsIndecis::WhiteQueens], allBlockers) & notBlockers;

    //     u64 bPawnControled = MoveGenerator::GetWhitePawnsAttacksBBs(BitBoardsIndecis::BlackPawns);
    //     u64 bLightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(BitBoardsIndecis::BlackKnights);
    //     bLightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(BitBoardsIndecis::BlackBishops, allBlockers) & notBlockers;
    //     u64 bHeavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(BitBoardsIndecis::BlackRooks, allBlockers) & notBlockers;
    //     bHeavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(BitBoardsIndecis::BlackQueens, allBlockers) & notBlockers;

    //     int whiteHeavyControlled = std::popcount(wHeavyAttackedSquers & (~(bLightAttackedSquers | bPawnControled)));
    //     int whiteLightControlled = std::popcount(wLightAttackedSquers & (~(bPawnControled)));
    //     int blackHeavyControlled = std::popcount(bHeavyAttackedSquers & (~(wLightAttackedSquers | wPawnControled)));
    //     int blackLightControlled = std::popcount(bLightAttackedSquers & (~(wPawnControled)));

    //     int wActivityBonus = 6 * whiteHeavyControlled + 12 * whiteLightControlled;
    //     if (wActivityBonus > 350)
    //         wActivityBonus = 350;

    //     int bActivityBonus = 6 * blackHeavyControlled + 12 * blackLightControlled;
    //     if (bActivityBonus > 350)
    //         bActivityBonus = 350;

    //     eval += wActivityBonus;
    //     eval -= bActivityBonus;
    // }
    // // zdublowane piony
    // u64f wPawns = pieces[0];
    // u64f bPawns = pieces[6];
    // for (int i = 0; i < 8; i++)
    // {
    //     if (std::popcount(wPawns & FILE_MASKS[i]) > 1)
    //         eval -= 5;
    //     if (std::popcount(bPawns & FILE_MASKS[i]) > 1)
    //         eval += 5;
    // }
    // //Wyspy pionowe
    // while (wPawns > 0ULL)
    // {
    //     int square = __builtin_ctzll(wPawns);
    //     wPawns &= wPawns - 1;

    //     if (std::popcount(MoveGenerator::GetPseudoLegalKingBBs(BitboardsIndecies[square]) & pieces[BitBoardsIndecis::WhitePawns]) > 1)
    //         eval++;
    // }
    // while (bPawns > 0ULL)
    // {
    //     int square = __builtin_ctzll(wPawns);
    //     bPawns &= bPawns - 1;

    //     if (std::popcount(MoveGenerator::GetPseudoLegalKingBBs(BitboardsIndecies[square]) & pieces[BitBoardsIndecis::BlackPawns]) > 1)
    //         eval--;
    // }
    // // end

    // int wKingIndex = __builtin_ctzll(wKingBB);
    // int bKingIndex = __builtin_ctzll(bKingBB);

    // int wKingX = wKingIndex % 8;
    // int wKingY = wKingIndex / 8;
    // int bKingX = bKingIndex % 8;
    // int bKingY = bKingIndex / 8;

    // if (allPiecesCount < 17)
    // {
    //     int distFromKing = abs(wKingX - bKingX) + abs(wKingY - bKingY);
    //     int wDistToCenter = std::max(3 - wKingX, wKingX - 4) + std::max(3 - wKingY, wKingY - 4);
    //     int bDistToCenter = std::max(3 - bKingX, bKingX - 4) + std::max(3 - bKingY, bKingY - 4);

    //     int matingWeigth = 30 * bDistToCenter - 30 * wDistToCenter;
    //     whiteMaterialAdventage ? matingWeigth += 15 * (14 - distFromKing) : matingWeigth -= 15 * (14 - distFromKing);
    //     matingWeigth = (isEndGame * matingWeigth) / 1024;

    //     eval += matingWeigth;
    // }

    if (board.IsWhiteMove())
        return eval;
    else
        return -eval;
}

int Eval::alphaBeta(BoardState &board, int alpha, int beta, int depth, int ref_depth)
{
    if (board.IsFiftyMoveRule() || board.IsStalemate() || board.IsInsufficientMaterial() || board.IsThreefoldRepetition())
        return 0;
    else if (board.IsCheckmate())
        return -(matScore - (1 + (ref_depth - depth)));

    else if (m_TT.probe(board.GetHash(), (ref_depth * 1000 + depth), alpha))
        return alpha;

    else if (depth == 0)
    {
        return quiescenceSearch(board, alpha, beta, depth, ref_depth);
    }

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);
    std::pair<Move, int> moveScores[218];
    int count = movesList.movesCount;
    for (int i = 0; i < count; ++i)
    {
        Move move = movesList.moves[i];
        moveScores[i] = {move, scoreMove(board, move)};
    }
    std::stable_sort(moveScores, moveScores + count,
                     [](const auto &a, const auto &b)
                     { return a.second > b.second; });

    for (size_t i = 0; i < count; i++)
    {
        board.MakeMove(moveScores[i].first);
        int eval = -alphaBeta(board, -beta, -alpha, depth - 1, ref_depth);
        board.UndoMove();

        if (m_StopSearch) // szybkie  wyjście z przeszukiwania
            return 0;

        if (eval >= beta) // za dobry ruch dla przeciwnika
            return beta;

        if (eval > alpha)
            alpha = eval;
    }

    if (m_StopSearch) // szybkie bez zapisywania błędnych wyników
        return 0;

    m_TT.store(board.GetHash(), (ref_depth * 1000 + depth), alpha);
    return alpha;
}
int Eval::quiescenceSearch(BoardState &board, int alpha, int beta, int depth, int ref_depth)
{
    if (board.IsFiftyMoveRule() || board.IsStalemate() || board.IsInsufficientMaterial() || board.IsThreefoldRepetition())
        return 0;
    else if (board.IsCheckmate())
        return -(matScore - (1 + (ref_depth - depth)));

    else if (m_TT.probe(board.GetHash(), (ref_depth * 1000 + depth), alpha))
        return alpha;

    int standPat = staticEval(board);
    if (standPat > alpha)
        alpha = standPat;

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);
    std::pair<Move, int> moveScores[120];
    int count = movesList.movesCount;
    int captureCount = 0;
    for (int i = 0; i < count; ++i)
    {
        if (movesList.moves[i].categorie != Capture)
            continue;

        Move move = movesList.moves[i];
        moveScores[captureCount++] = {move, scoreMove(board, move)};
    }
    std::stable_sort(moveScores, moveScores + captureCount,
                     [](const auto &a, const auto &b)
                     { return a.second > b.second; });

    for (size_t i = 0; i < captureCount; i++)
    {
        board.MakeMove(moveScores[i].first);
        int eval = -quiescenceSearch(board, -beta, -alpha, depth - 1, ref_depth);
        board.UndoMove();

        if (m_StopSearch)
            return 0;

        if (eval >= beta)
            return beta;
        if (eval > alpha)
            alpha = eval;
    }

    if (m_StopSearch)
        return 0;

    m_TT.store(board.GetHash(), (ref_depth * 1000 + depth), alpha);
    return alpha;
}

Move Eval::FindBestMoveFixedDepth(BoardState &board, int depth)
{
    Eval::m_StopSearch.store(false, std::memory_order_relaxed);

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    std::pair<Move, int> moveScores[218];
    int count = movesList.movesCount;

    for (int i = 0; i < count; ++i)
    {
        Move move = movesList.moves[i];
        moveScores[i] = {move, scoreMove(board, move)};
    }

    std::stable_sort(moveScores, moveScores + count,
                     [](const auto &a, const auto &b)
                     { return a.second > b.second; });

    Move bestMove = moveScores[0].first;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    for (int i = 0; i < count; ++i)
    {
        const Move &move = moveScores[i].first;
        board.MakeMove(move);
        int eval = -alphaBeta(board, -beta, -alpha, depth - 1, depth);
        moveScores[i].second = eval;

        board.UndoMove();

        if (m_StopSearch)
            break;

        if (eval > alpha)
        {
            alpha = eval;
            bestMove = move;
        }

        char f1 = 'a' + (move.startingSquere % 8);
        char r1 = '1' + (move.startingSquere / 8);
        char f2 = 'a' + (move.destSquere % 8);
        char r2 = '1' + (move.destSquere / 8);

        int toWhitePerspective = -1;
        if (board.IsWhiteMove())
            toWhitePerspective = 1;

        std::cout << f1 << r1 << f2 << r2 << ":" << (eval * toWhitePerspective) << ";";
    }
    std::cout << std::endl;
    return bestMove;
}

void Eval::StopSearch()
{
    Eval::m_StopSearch.store(true, std::memory_order_relaxed);
}
int Eval::CalculateTimeToSearch(const int moveNr, const int wtime, const int btime, const int winc, const int binc, const bool isWhiteTurn)
{
    int temp = 120 - moveNr;

    if (isWhiteTurn)
    {
        if (moveNr < 7)
            return wtime / (temp + 30) + winc;
        else if (temp > 5)
            return (wtime + 5000) / temp + winc;
        else
            return wtime / 50 + winc;
    }
    else
    {
        if (moveNr < 7)
            return btime / (temp + 30) + binc;
        else if (temp > 5)
            return (btime + 5000) / temp + binc;
        else
            return btime / 50 + binc;
    }
}

Move Eval::FindBestMove(BoardState &board)
{
    Eval::m_StopSearch.store(false, std::memory_order_relaxed);

    MoveList movesList;
    MoveGenerator::GetLegalMoves(board, movesList);

    std::pair<Move, int> moveScores[218];
    int count = movesList.movesCount;
    for (int i = 0; i < count; ++i)
    {
        Move move = movesList.moves[i];
        moveScores[i] = {move, scoreMove(board, move)};
    }
    std::stable_sort(moveScores, moveScores + count,
                     [](const auto &a, const auto &b)
                     { return a.second > b.second; });

    Move bestMove = moveScores[0].first;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    int current_depth = 1;
    while (current_depth < 100 && !m_StopSearch)
    {
        for (int i = 0; i < count; ++i)
        {
            const Move &move = moveScores[i].first;
            board.MakeMove(move);
            int eval = -alphaBeta(board, -beta, -alpha, current_depth - 1, current_depth);
            moveScores[i].second = eval;

            board.UndoMove();

            if (m_StopSearch)
                break;

            if (eval > alpha)
            {
                alpha = eval;
                bestMove = move;
            }

            // char f1 = 'a' + (move.startingSquere % 8);
            // char r1 = '1' + (move.startingSquere / 8);
            // char f2 = 'a' + (move.destSquere % 8);
            // char r2 = '1' + (move.destSquere / 8);
            // std::cout << "[" << current_depth << "] Ruch: " << f1 << r1 << f2 << r2 << ", Eval: " << eval << "\n";
        }

        if (m_StopSearch)
            break;

        std::stable_sort(moveScores, moveScores + count,
                         [](const auto &a, const auto &b)
                         { return a.second > b.second; });
        ++current_depth;
    }

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

            std::stable_sort(scoredMoves, scoredMoves + moves.movesCount,
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
        int result = quiescenceSearch(state, -matScore, matScore, 0, 1);
        // 4. Backpropagation
        while (node != nullptr)
        {
            node->UpdateStats(result);
            node = node->GetParent();
        }
    }

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
    // char f1 = 'a' + (move.startingSquere % 8);
    // char r1 = '1' + (move.startingSquere / 8);
    // char f2 = 'a' + (move.destSquere % 8);
    // char r2 = '1' + (move.destSquere / 8);

    // std::cout << "Ruch: " << f1 << r1 << f2 << r2 << "BestEval: " << bestEval << "\n";
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
