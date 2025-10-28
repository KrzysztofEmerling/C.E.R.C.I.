#include "Eval.h"
#include "MoveGenerator.h"
#include "MCTS.h"

#include <algorithm>
#include <iostream>
#include <chrono>

#define EQ_BAIAS 0
std::atomic<bool> Eval::m_StopSearch = false;
void Eval::PrepareForNewGame()
{
    m_TT.Clear();
}
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
    bool whiteMaterialAdventage = eval > 0;

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
            activityScore += ((1024 - isEndGame) * PiecesOpeningPositionTable[i][index] + isEndGame * PiecesEndgamePositionTable[i][index]) / 1024;
        }
        // Czarne
        while (bPiece)
        {
            int index = __builtin_ctzll(bPiece);
            bPiece &= bPiece - 1;
            int mirroredIndex = index ^ 56;
            activityScore -= ((1024 - isEndGame) * PiecesOpeningPositionTable[i][mirroredIndex] + isEndGame * PiecesEndgamePositionTable[i][mirroredIndex]) / 1024;
        }
    }
    eval += activityScore;
    u64f wKingBB = pieces[5];
    u64f bKingBB = pieces[11];

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

    // // czarne
    // kingDangereZone = MoveGenerator::GetPseudoLegalKingBBs(wKingBB);

    // lightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(pieces[7]);
    // lightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(pieces[8], blockingPawns) & notBlockers;
    // heavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(pieces[9], blockingPawns) & notBlockers;
    // heavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(pieces[10], blockingPawns) & notBlockers;

    // lightAttackers = std::popcount(kingDangereZone & lightAttackedSquers);
    // heavyAttackers = std::popcount(kingDangereZone & heavyAttackedSquers);
    // eval -= lightAttackers * 45 + heavyAttackers * 60;
    //  // end

    // Adding Positional playstyle
    // white
    if (allPiecesCount < 31)
    {
        u64 allBlockers = pieces[0] | pieces[1] | pieces[2] | pieces[3] | pieces[4] | pieces[6] | pieces[7] | pieces[8] | pieces[9] | pieces[10];
        u64 notBlockers = ~allBlockers;

        u64 wPawnControled = MoveGenerator::GetWhitePawnsAttacksBBs(pieces[0]);
        u64 wLightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(pieces[1]);
        wLightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(pieces[2], allBlockers) & notBlockers;
        u64 wHeavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(pieces[3], allBlockers) & notBlockers;
        wHeavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(pieces[4], allBlockers) & notBlockers;

        u64 bPawnControled = MoveGenerator::GetWhitePawnsAttacksBBs(pieces[6]);
        u64 bLightAttackedSquers = MoveGenerator::GetKnightsAttacksBBs(pieces[7]);
        bLightAttackedSquers |= MoveGenerator::GetPseudoLegalBishopsBBs(pieces[8], allBlockers) & notBlockers;
        u64 bHeavyAttackedSquers = MoveGenerator::GetPseudoLegalRooksBBs(pieces[9], allBlockers) & notBlockers;
        bHeavyAttackedSquers |= MoveGenerator::GetPseudoLegalQueensBBs(pieces[10], allBlockers) & notBlockers;

        int whiteHeavyControlled = std::popcount(wHeavyAttackedSquers & (~(bLightAttackedSquers | bPawnControled)));
        int whiteLightControlled = std::popcount(wLightAttackedSquers & (~(bPawnControled)));

        int blackHeavyControlled = std::popcount(bHeavyAttackedSquers & (~(wLightAttackedSquers | wPawnControled)));
        int blackLightControlled = std::popcount(bLightAttackedSquers & (~(wPawnControled)));

        int wActivityBonus = 6 * whiteHeavyControlled + 12 * whiteLightControlled;
        if (wActivityBonus > 350)
            wActivityBonus = 350;

        int bActivityBonus = 6 * blackHeavyControlled + 12 * blackLightControlled;
        if (bActivityBonus > 350)
            bActivityBonus = 350;

        eval += wActivityBonus;
        eval -= bActivityBonus;
    }
    // zdublowane piony
    u64f wPawns = pieces[0];
    u64f bPawns = pieces[6];
    for (int i = 0; i < 8; i++)
    {
        if (std::popcount(wPawns & FILE_MASKS[i]) > 1)
            eval -= 5;
        if (std::popcount(bPawns & FILE_MASKS[i]) > 1)
            eval += 5;
    }
    while (wPawns > 0ULL)
    {
        int square = __builtin_ctzll(wPawns);
        wPawns &= wPawns - 1;

        if (std::popcount(MoveGenerator::GetPseudoLegalKingBBs(BitboardsIndecies[square]) & pieces[0]) > 1)
            eval++;
    }
    while (bPawns > 0ULL)
    {
        int square = __builtin_ctzll(wPawns);
        bPawns &= bPawns - 1;

        if (std::popcount(MoveGenerator::GetPseudoLegalKingBBs(BitboardsIndecies[square]) & pieces[6]) > 1)
            eval--;
    }
    // end

    int wKingIndex = __builtin_ctzll(wKingBB);
    int bKingIndex = __builtin_ctzll(bKingBB);

    int wKingX = wKingIndex % 8;
    int wKingY = wKingIndex / 8;
    int bKingX = bKingIndex % 8;
    int bKingY = bKingIndex / 8;

    int distFromKing = abs(wKingX - bKingX) + abs(wKingY - bKingY);
    if (board.IsWhiteMove())
    {
        if (allPiecesCount < 7)
        {
            int distToCenter = std::max(3 - bKingX, bKingX - 4) + std::max(3 - bKingY, bKingY - 4);

            int matingWeight = (14 - distFromKing) * 80 + distToCenter * 100;

            whiteMaterialAdventage ? eval += matingWeight : eval -= matingWeight;
        }

        return eval;
    }
    else
    {
        if (allPiecesCount < 7)
        {
            int distToCenter = std::max(3 - wKingX, wKingX - 4) + std::max(3 - wKingY, wKingY - 4);

            int matingWeight = (14 - distFromKing) * 80 + distToCenter * 100;

            (!whiteMaterialAdventage) ? eval += matingWeight : eval -= matingWeight;
        }
        return -eval;
    }
}

int Eval::alphaBeta(BoardState &board, int depth, int alpha, int beta)
{

    int bestEval = minEvalScore;
    // if (board.IsFiftyMoveRule() || board.IsInsufficientMaterial() || board.IsStalemate() || board.IsThreefoldRepetition())
    if (board.IsFiftyMoveRule() || board.IsStalemate() || board.IsInsufficientMaterial())
        return 0;
    else if (board.IsCheckmate())
        return -matScore - depth;

    else if (m_TT.probe(board.GetHash(), depth, bestEval))
        return bestEval;

    else if (depth == 0)
        return quiescenceSearch(board, -beta, -alpha, depth - 1);

    else if (m_StopSearch) // szybkie  wyjście z przeszukiwania
        return 0;

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

    for (size_t i = 0; i < count; i++)
    {
        board.MakeMove(moveScores[i].first);
        int eval = -alphaBeta(board, depth - 1, -beta, -alpha);
        board.UndoMove();

        if (m_StopSearch) // szybkie  wyjście z przeszukiwania
            return 0;

        if (eval > bestEval)
            bestEval = eval;

        if (eval > alpha)
            alpha = eval;

        if (alpha >= beta) // Beta cutoff
            break;
    }

    if (m_StopSearch) // szybkie bez zapisywania błędnych wyników
        return 0;
    m_TT.store(board.GetHash(), depth, bestEval);
    return bestEval;
}
int Eval::quiescenceSearch(BoardState &board, int alpha, int beta, int depth)
{
    if (board.IsCheckmate())
        return -matScore - depth;

    else if (m_TT.probe(board.GetHash(), depth, alpha))
        return alpha;

    int qEval = staticEval(board);
    if (qEval >= beta)
        return beta;
    if (qEval > alpha)
        alpha = qEval;

    if (m_StopSearch)
    {
        return 0;
    }

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
    std::sort(moveScores, moveScores + captureCount,
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    for (size_t i = 0; i < captureCount; i++)
    {
        board.MakeMove(moveScores[i].first);
        int eval = -quiescenceSearch(board, -beta, -alpha, depth - 1);
        board.UndoMove();

        if (eval >= beta)
            return beta;
        if (eval > alpha)
            alpha = eval;

        if (m_StopSearch)
            return 0;
    }
    if (m_StopSearch)
        return 0;
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

        // char f1 = 'a' + (move.startingSquere % 8);
        // char r1 = '1' + (move.startingSquere / 8);
        // char f2 = 'a' + (move.destSquere % 8);
        // char r2 = '1' + (move.destSquere / 8);
        // std::cout << "Ruch: " << f1 << r1 << f2 << r2 << ", Eval: " << eval << "\n";

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

    std::sort(moveScores, moveScores + count,
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    Move bestMove = moveScores[0].first;
    int bestEval = minEvalScore;
    int alpha = minEvalScore;
    int beta = maxEvalScore;

    int current_depth = 1;
    while (!m_StopSearch)
    {
        for (int i = 0; i < count; ++i)
        {
            const Move &move = moveScores[i].first;
            board.MakeMove(move);
            int eval = -alphaBeta(board, current_depth - 1, -beta, -alpha);
            moveScores[i].second = eval;

            board.UndoMove();

            if (m_StopSearch)
                break;

            if (eval > bestEval)
            {
                bestEval = eval;
                bestMove = move;
            }

            if (eval > alpha)
                alpha = eval;

            if (alpha >= beta)
                break;

            // char f1 = 'a' + (move.startingSquere % 8);
            // char r1 = '1' + (move.startingSquere / 8);
            // char f2 = 'a' + (move.destSquere % 8);
            // char r2 = '1' + (move.destSquere / 8);
            // std::cout << "[" << current_depth << "] Ruch: " << f1 << r1 << f2 << r2 << ", Eval: " << eval << "\n";
        }

        if (m_StopSearch)
            break;

        std::sort(moveScores, moveScores + count,
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
