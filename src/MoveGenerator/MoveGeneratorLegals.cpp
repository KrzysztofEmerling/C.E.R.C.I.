#include "MoveGenerator.h"
#include "MagicBitboards/RookMagicBitboards.h"
#include "MagicBitboards/BishopMagicBitboards.h"
#include <queue>

#include <iostream>

// TODO: wymagane sprawdzenie związania po enpassancie
void MoveGenerator::GetLegalWhitePawnsMoves(u64f pawns, u64 empty, u64 black, u64 enpassants, u64 posibleMovesMask, std::queue<Move> &moves)
{

    u64 singlePush = (pawns << 8) & empty;
    u64 doublePush = ((singlePush & WHITE_DUBLE_PUSH) << 8) & empty & posibleMovesMask;
    singlePush &= posibleMovesMask;

    black &= posibleMovesMask;
    enpassants &= posibleMovesMask;
    u64 leftAttack = ((pawns & NOT_COL_A) << 7) & black;
    u64 leftEmpassant = ((pawns & NOT_COL_A) << 7) & enpassants;
    u64 rightAttack = ((pawns & NOT_COL_H) << 9) & black;
    u64 rightEmpassant = ((pawns & NOT_COL_H) << 9) & enpassants;

    while (singlePush)
    {
        int dest = __builtin_ctzll(singlePush);
        singlePush &= singlePush - 1;

        int from = dest - 8;
        if (WHITE_PROMOTION_LINE & BitboardsIndecies[dest])
        {
            moves.push(Move(from, dest, PromotionQueen));
            moves.push(Move(from, dest, PromotionRook));
            moves.push(Move(from, dest, PromotionBishop));
            moves.push(Move(from, dest, PromotionKnight));
        }
        else
            moves.push(Move(from, dest));
    }

    while (doublePush)
    {
        int dest = __builtin_ctzll(doublePush);
        doublePush &= doublePush - 1;

        int from = dest - 16;
        moves.push(Move(from, dest, DoublePush));
    }

    while (leftAttack)
    {
        int dest = __builtin_ctzll(leftAttack);
        leftAttack &= leftAttack - 1;

        int from = dest - 7;
        if (WHITE_PROMOTION_LINE & BitboardsIndecies[dest])
        {
            moves.push(Move(from, dest, PromotionQueen));
            moves.push(Move(from, dest, PromotionRook));
            moves.push(Move(from, dest, PromotionBishop));
            moves.push(Move(from, dest, PromotionKnight));
        }
        else
            moves.push(Move(from, dest));
    }

    while (rightAttack)
    {
        int dest = __builtin_ctzll(rightAttack);
        rightAttack &= rightAttack - 1;

        int from = dest - 9;
        if (WHITE_PROMOTION_LINE & BitboardsIndecies[dest])
        {
            moves.push(Move(from, dest, PromotionQueen));
            moves.push(Move(from, dest, PromotionRook));
            moves.push(Move(from, dest, PromotionBishop));
            moves.push(Move(from, dest, PromotionKnight));
        }
        else
            moves.push(Move(from, dest));
    }

    while (leftEmpassant)
    {
        int dest = __builtin_ctzll(leftEmpassant);
        leftEmpassant &= leftEmpassant - 1;

        int from = dest - 7;
        moves.push(Move(from, dest, EmpassantMove));
    }

    while (rightEmpassant)
    {
        int dest = __builtin_ctzll(rightEmpassant);
        rightEmpassant &= rightEmpassant - 1;

        int from = dest - 9;
        moves.push(Move(from, dest, EmpassantMove));
    }
}
void MoveGenerator::GetLegalBlackPawnsMoves(u64f pawns, u64 empty, u64 white, u64 enpassants, u64 posibleMovesMask, std::queue<Move> &moves)
{
    u64 singlePush = (pawns >> 8) & empty;
    u64 doublePush = ((singlePush & BLACK_DUBLE_PUSH) >> 8) & empty & posibleMovesMask;
    singlePush &= posibleMovesMask;

    white &= posibleMovesMask;
    enpassants &= posibleMovesMask;
    u64 leftAttack = ((pawns & NOT_COL_A) >> 9) & white;
    u64 leftEmpassant = ((pawns & NOT_COL_A) >> 9) & enpassants;
    u64 rightAttack = ((pawns & NOT_COL_H) >> 7) & white;
    u64 rightEmpassant = ((pawns & NOT_COL_H) >> 7) & enpassants;
    // std::cout << "\n--------------------\n left empassant:" << leftEmpassant << ", right empassant:" << rightEmpassant
    //           << "posible move mask:" << posibleMovesMask << ", \npawns:" << pawns << " empassant mask: " << enpassants << "\n--------------------\n";
    while (singlePush)
    {
        int dest = __builtin_ctzll(singlePush);
        singlePush &= singlePush - 1;

        int from = dest + 8;
        if (BLACK_PROMOTION_LINE & BitboardsIndecies[dest])
        {
            moves.push(Move(from, dest, PromotionQueen));
            moves.push(Move(from, dest, PromotionRook));
            moves.push(Move(from, dest, PromotionBishop));
            moves.push(Move(from, dest, PromotionKnight));
        }
        else
            moves.push(Move(from, dest));
    }

    while (doublePush)
    {
        int dest = __builtin_ctzll(doublePush);
        doublePush &= doublePush - 1;

        int from = dest + 16;
        moves.push(Move(from, dest, DoublePush));
    }

    while (leftAttack)
    {
        int dest = __builtin_ctzll(leftAttack);
        leftAttack &= leftAttack - 1;

        int from = dest + 9;
        if (BLACK_PROMOTION_LINE & BitboardsIndecies[dest])
        {
            moves.push(Move(from, dest, PromotionQueen));
            moves.push(Move(from, dest, PromotionRook));
            moves.push(Move(from, dest, PromotionBishop));
            moves.push(Move(from, dest, PromotionKnight));
        }
        else
            moves.push(Move(from, dest));
    }

    while (rightAttack)
    {
        int dest = __builtin_ctzll(rightAttack);
        rightAttack &= rightAttack - 1;

        int from = dest + 7;
        if (BLACK_PROMOTION_LINE & BitboardsIndecies[dest])
        {
            moves.push(Move(from, dest, PromotionQueen));
            moves.push(Move(from, dest, PromotionRook));
            moves.push(Move(from, dest, PromotionBishop));
            moves.push(Move(from, dest, PromotionKnight));
        }
        else
            moves.push(Move(from, dest));
    }

    while (leftEmpassant)
    {
        int dest = __builtin_ctzll(leftEmpassant);
        leftEmpassant &= leftEmpassant - 1;

        int from = dest + 9;
        moves.push(Move(from, dest, EmpassantMove));
    }

    while (rightEmpassant)
    {
        int dest = __builtin_ctzll(rightEmpassant);
        rightEmpassant &= rightEmpassant - 1;

        int from = dest + 7;
        moves.push(Move(from, dest, EmpassantMove));
    }
}

void MoveGenerator::GetLegalKingMoves(int square, u64 movementBBs, std::queue<Move> &moves)
{
    while (movementBBs)
    {
        int dest_square = __builtin_ctzll(movementBBs);
        movementBBs &= movementBBs - 1;
        Move move(square, dest_square);
        moves.push(move);
    }
}

void MoveGenerator::GetLegalKnightsMoves(u64f knights, u64 notAllay, u64 posibleMovesMask, std::queue<Move> &moves)
{
    while (knights)
    {
        int square = __builtin_ctzll(knights);
        knights &= knights - 1;

        u64 temp = BitboardsIndecies[square];

        u64 destSquaresBBs = GetKnightsAttacksBBs(temp) & notAllay & posibleMovesMask;

        while (destSquaresBBs)
        {
            int dest_square = __builtin_ctzll(destSquaresBBs);
            destSquaresBBs &= destSquaresBBs - 1;
            Move move(square, dest_square);
            moves.push(move);
        }
    }
}

void MoveGenerator::GetLegalBishopsMoves(u64f bishops, u64 notAllay, u64 blockers, u64 posibleMovesMask, std::queue<Move> &moves)
{
    u64 tempBishops = bishops;

    while (tempBishops)
    {
        int square = __builtin_ctzll(tempBishops);
        tempBishops &= tempBishops - 1;

        u64 blockersMask = BishopMagicBitboards::GetBlockersMask(square);
        u64 relevantBlockers = blockers & blockersMask;

        u64 bishopMoves = BishopMagicBitboards::GetMovesMask(square, relevantBlockers) & notAllay & posibleMovesMask;
        while (bishopMoves)
        {
            int dest_square = __builtin_ctzll(bishopMoves);
            bishopMoves &= bishopMoves - 1;
            Move move(square, dest_square);
            moves.push(move);
        }
    }
}
void MoveGenerator::GetLegalRooksMoves(u64f rooks, u64 notAllay, u64 blockers, u64 posibleMovesMask, std::queue<Move> &moves)
{
    u64 tempRooks = rooks;

    while (tempRooks)
    {
        int square = __builtin_ctzll(tempRooks);
        tempRooks &= tempRooks - 1;

        u64 blockersMask = RookMagicBitboards::GetBlockersMask(square);
        u64 relevantBlockers = blockers & blockersMask;

        u64 rookMoves = RookMagicBitboards::GetMovesMask(square, relevantBlockers) & notAllay & posibleMovesMask;

        while (rookMoves)
        {
            int dest_square = __builtin_ctzll(rookMoves);
            rookMoves &= rookMoves - 1;
            Move move(square, dest_square);
            moves.push(move);
        }
    }
}
void MoveGenerator::GetLegalQueensMoves(u64f queens, u64 notAllay, u64 blockers, u64 posibleMovesMask, std::queue<Move> &moves)
{
    u64 tempQueens = queens;

    while (tempQueens)
    {
        int square = __builtin_ctzll(tempQueens);
        tempQueens &= tempQueens - 1;

        u64 rookBlockersMask = RookMagicBitboards::GetBlockersMask(square);
        u64 bishopBlockersMask = BishopMagicBitboards::GetBlockersMask(square);

        u64 queenMoves = RookMagicBitboards::GetMovesMask(square, blockers & rookBlockersMask) & notAllay & posibleMovesMask;
        queenMoves |= BishopMagicBitboards::GetMovesMask(square, blockers & bishopBlockersMask) & notAllay & posibleMovesMask;

        while (queenMoves)
        {
            int dest_square = __builtin_ctzll(queenMoves);
            queenMoves &= queenMoves - 1;
            Move move(square, dest_square);
            moves.push(move);
        }
    }
}

void MoveGenerator::GetLegalWhiteCasels(u64 all, u64 attacks, bool shortCastleRights, bool longCastleRights, std::queue<Move> &moves)
{
    u64 occupiedOrAttacked = all | attacks;

    if (shortCastleRights && !(WHITE_SHORT_CASTLE_MASK & occupiedOrAttacked))
    {
        moves.push(Move(4, 6, Castling)); // e1 -> g1
    }

    if (longCastleRights && !(WHITE_LONG_CASTLE_MASK & attacks) && !(WHITE_LONG_ROOK_BLOCK & all))
    {
        moves.push(Move(4, 2, Castling)); // e1 -> c1
    }
}

void MoveGenerator::GetLegalBlackCasels(u64 all, u64 attacks, bool shortCastleRights, bool longCastleRights, std::queue<Move> &moves)
{
    u64 occupiedOrAttacked = all | attacks;

    if (shortCastleRights && !(BLACK_SHORT_CASTLE_MASK & occupiedOrAttacked))
    {
        moves.push(Move(60, 62, Castling)); // e8 -> g8
    }

    if (longCastleRights && !(BLACK_LONG_CASTLE_MASK & attacks) && !(BLACK_LONG_ROOK_BLOCK & all))
    {
        moves.push(Move(60, 58, Castling)); // e8 -> c8
    }
}
