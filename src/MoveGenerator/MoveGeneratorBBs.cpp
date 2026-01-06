#include "MoveGenerator.h"
#include "MagicBitboards/RookMagicBitboards.h"
#include "MagicBitboards/BishopMagicBitboards.h"
#include <queue>


u64 MoveGenerator::GetWhitePawnsAttacksBBs(u64f pawns)
{
    //TODO dodać też optymalizację dla pionów i sprawdzić czy szybsza
    u64 leftAttack = (pawns & NOT_COL_A) << 7;
    u64 rightAttack = (pawns & NOT_COL_H) << 9;

    return leftAttack | rightAttack;
}
u64 MoveGenerator::GetBlackPawnsAttacksBBs(u64f pawns)
{
    u64 leftAttack = (pawns & NOT_COL_A) >> 9;
    u64 rightAttack = (pawns & NOT_COL_H) >> 7;

    return leftAttack | rightAttack;
}

u64 MoveGenerator::GetKnightsAttacksBBs(u64f knights)
{
    u64 attacks = 0ULL;
    while (knights)
    {
        int square = __builtin_ctzll(knights);
        knights &= knights - 1;
        attacks |= KnightAttacksLUT[square];
    }
    return attacks;
}

u64 MoveGenerator::GetPseudoLegalBishopsBBs(u64f bishops, u64 blockers)
{
    u64 moves = 0ULL;
    u64 tempBishops = bishops;

    while (tempBishops)
    {
        int square = __builtin_ctzll(tempBishops);
        tempBishops &= tempBishops - 1;

        u64 blockersMask = BishopMagicBitboards::GetBlockersMask(square);
        u64 relevantBlockers = blockers & blockersMask;

        u64 bishopMoves = BishopMagicBitboards::GetMovesMask(square, relevantBlockers);
        moves |= bishopMoves;
    }
    return moves;
}
u64 MoveGenerator::GetPseudoLegalRooksBBs(u64f rooks, u64 blockers)
{
    u64 moves = 0ULL;
    u64 tempRooks = rooks;

    while (tempRooks)
    {
        int square = __builtin_ctzll(tempRooks);
        tempRooks &= tempRooks - 1;

        u64 blockersMask = RookMagicBitboards::GetBlockersMask(square);
        u64 relevantBlockers = blockers & blockersMask;

        u64 rookMoves = RookMagicBitboards::GetMovesMask(square, relevantBlockers);
        moves |= rookMoves;
    }
    return moves;
}
u64 MoveGenerator::GetPseudoLegalQueensBBs(u64f queens, u64 blockers)
{
    u64 moves = 0ULL;
    u64 tempQueens = queens;

    while (tempQueens)
    {
        int square = __builtin_ctzll(tempQueens);
        tempQueens &= tempQueens - 1;

        u64 rookBlockersMask = RookMagicBitboards::GetBlockersMask(square);
        u64 bishopBlockersMask = BishopMagicBitboards::GetBlockersMask(square);

        u64 queenMoves = RookMagicBitboards::GetMovesMask(square, blockers & rookBlockersMask);
        queenMoves |= BishopMagicBitboards::GetMovesMask(square, blockers & bishopBlockersMask);
        moves |= queenMoves;
    }
    return moves;
}

u64 MoveGenerator::GetPseudoLegalKingBBs(u64f king)
{
    return KingAttacksLUT[__builtin_ctzll(king)];
}

