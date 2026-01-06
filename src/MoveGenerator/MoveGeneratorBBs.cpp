#include "MoveGenerator.h"
#include "MagicBitboards/RookMagicBitboards.h"
#include "MagicBitboards/BishopMagicBitboards.h"
#include <queue>


u64 MoveGenerator::GetWhitePawnsAttacksBBs(u64f pawns)
{
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
    // 2 w górę 1 w prawo
    return ((NOT_COL_H & knights) << 17) |   // 2 up, 1 right ok
           ((NOT_COL_A & knights) << 15) |   // 2 up, 1 left ok
           ((NOT_COLS_GH & knights) << 10) | // 1 up, 2 right ok
           ((NOT_COLS_AB & knights) << 6) |  // 1 up, 2 left ok
           ((NOT_COL_H & knights) >> 15) |   // 2 down, 1 right
           ((NOT_COL_A & knights) >> 17) |   // 2 down, 1 left
           ((NOT_COLS_GH & knights) >> 6) |  // 1 down, 2 right
           ((NOT_COLS_AB & knights) >> 10);  // 1 down, 2 left;
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
    u64 moves = 0ULL;
    moves |= (king << 8); // Up
    moves |= (king >> 8); // Down

    moves |= (king << 1) & NOT_COL_A; // Right
    moves |= (king >> 1) & NOT_COL_H; // Left

    moves |= (king << 9) & NOT_COL_A; // Up-Right
    moves |= (king << 7) & NOT_COL_H; // Up-Left
    moves |= (king >> 7) & NOT_COL_A; // Down-Right
    moves |= (king >> 9) & NOT_COL_H; // Down-Left

    return moves;
}
