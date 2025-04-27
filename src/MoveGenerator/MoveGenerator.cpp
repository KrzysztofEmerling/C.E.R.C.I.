#include "MoveGenerator.h"
#include "MagicBitboards/RookMagicBitboards.h"
#include "MagicBitboards/BishopMagicBitboards.h"
#include <queue>

#include <iostream>

void MoveGenerator::GetLegalMoves(const BoardState &state, std::queue<Move> &moves)
{
    const u64f *pieces = state.GetBBs();
    const Flags flags = state.GetFlags();

    u64 white = pieces[0] | pieces[1] | pieces[2] |
                pieces[3] | pieces[4] | pieces[5];
    u64 black = pieces[6] | pieces[7] | pieces[8] |
                pieces[9] | pieces[10] | pieces[11];
    u64 all = white | black;
    u64 empty = ~all;

    u64 oponentPawnsAttacks;
    u64 oponentKnightsAttacks;
    u64 oponentBishopsAttacks;
    u64 oponentRooksAttacks;
    u64 oponentQueensAttacks;
    u64 plKing, oponentKingAttacks;

    if (flags.whiteOnMove)
    {
        u64 notAllay = ~white;
        u64 notOponent = ~black;
        u64 allWithoutKing = all & (~pieces[WhiteKing]);

        oponentPawnsAttacks = GetBlackPawnsAttacksBBs(pieces[BlackPawns]);
        oponentKnightsAttacks = GetKnightsAttacksBBs(pieces[BlackKnights]);
        oponentBishopsAttacks = GetPseudoLegalBishopsBBs(pieces[BlackBishops], notOponent, allWithoutKing);
        oponentRooksAttacks = GetPseudoLegalRooksBBs(pieces[BlackRooks], notOponent, allWithoutKing);
        oponentQueensAttacks = GetPseudoLegalQueensBBs(pieces[BlackQueens], notOponent, allWithoutKing);
        oponentKingAttacks = GetPseudoLegalKingBBs(pieces[BlackKing], notOponent);
        u64 underAttack = oponentPawnsAttacks | oponentKnightsAttacks | oponentBishopsAttacks | oponentRooksAttacks | oponentQueensAttacks | oponentKingAttacks;

        plKing = GetPseudoLegalKingBBs(pieces[WhiteKing], notAllay) & (~underAttack);
        int kingSquare = __builtin_ctzll(pieces[WhiteKing]);

        u64 attackers = 0ULL;
        if (pieces[WhiteKing] & underAttack)
        {
            attackers |= GetKnightsAttacksBBs(pieces[WhiteKing]) & pieces[BlackKnights];
            attackers |= GetPseudoLegalBishopsBBs(pieces[WhiteKing], 0xffffffffffffffff, all) & (pieces[BlackBishops] | pieces[BlackQueens]);
            attackers |= GetPseudoLegalRooksBBs(pieces[WhiteKing], 0xffffffffffffffff, all) & (pieces[BlackRooks] | pieces[BlackQueens]);

            if (std::popcount(attackers) > 1)
            {
                GetLegalKingMoves(kingSquare, plKing, moves);
                return;
            }
            else
            {
                int attackerSquare = __builtin_ctzll(attackers);
                u64 posibleMovesMask = BetweenSquaresTable[kingSquare][attackerSquare] | attackers;

                u64f unpinedPieces[4];
                unpinedPieces[PinablePieces::Pawns] = pieces[WhitePawns];
                unpinedPieces[PinablePieces::Bishops] = pieces[WhiteBishops];
                unpinedPieces[PinablePieces::Rooks] = pieces[WhiteRooks];
                unpinedPieces[PinablePieces::Queens] = pieces[WhiteQueens];
                u64 notUnpinPiecesMask = ResolveWhitePinedPieces(pieces, kingSquare, white, black, all, notAllay, empty, posibleMovesMask, unpinedPieces, moves);

                GetLegalWhitePawnsMoves(unpinedPieces[PinablePieces::Pawns], empty, black, pieces[Empassants], posibleMovesMask, moves);
                GetLegalKnightsMoves(pieces[WhiteKnights] & notUnpinPiecesMask, notAllay, posibleMovesMask, moves);
                GetLegalBishopsMoves(unpinedPieces[PinablePieces::Bishops], notAllay, all, posibleMovesMask, moves);
                GetLegalRooksMoves(unpinedPieces[PinablePieces::Rooks], notAllay, all, posibleMovesMask, moves);
                GetLegalQueensMoves(unpinedPieces[PinablePieces::Queens], notAllay, all, posibleMovesMask, moves);
                GetLegalKingMoves(kingSquare, plKing, moves);
            }
        }
        else
        {
            u64 posibleMovesMask = 0xffffffffffffffff;

            u64f unpinedPieces[4];
            unpinedPieces[PinablePieces::Pawns] = pieces[WhitePawns];
            unpinedPieces[PinablePieces::Bishops] = pieces[WhiteBishops];
            unpinedPieces[PinablePieces::Rooks] = pieces[WhiteRooks];
            unpinedPieces[PinablePieces::Queens] = pieces[WhiteQueens];
            u64 notPinPiecesMask = ResolveWhitePinedPieces(pieces, kingSquare, white, black, all, notAllay, empty, posibleMovesMask, unpinedPieces, moves);

            GetLegalWhitePawnsMoves(unpinedPieces[PinablePieces::Pawns], empty, black, pieces[Empassants], posibleMovesMask, moves);
            GetLegalKnightsMoves(pieces[WhiteKnights] & notPinPiecesMask, notAllay, posibleMovesMask, moves);
            GetLegalBishopsMoves(unpinedPieces[PinablePieces::Bishops], notAllay, all, posibleMovesMask, moves);
            GetLegalRooksMoves(unpinedPieces[PinablePieces::Rooks], notAllay, all, posibleMovesMask, moves);
            GetLegalQueensMoves(unpinedPieces[PinablePieces::Queens], notAllay, all, posibleMovesMask, moves);
            GetLegalKingMoves(kingSquare, plKing, moves);

            // TODO: dodanie roszad dla białych

            return;
        }
    }
    else
    {
        u64 notAllay = ~black;
        u64 notOponent = ~white;
        u64 allWithoutKing = all & (~pieces[BlackKing]);

        oponentPawnsAttacks = GetWhitePawnsAttacksBBs(pieces[WhitePawns]);
        oponentKnightsAttacks = GetKnightsAttacksBBs(pieces[WhiteKnights]);
        oponentBishopsAttacks = GetPseudoLegalBishopsBBs(pieces[WhiteBishops], notOponent, allWithoutKing);
        oponentRooksAttacks = GetPseudoLegalRooksBBs(pieces[WhiteRooks], notOponent, allWithoutKing);
        oponentQueensAttacks = GetPseudoLegalQueensBBs(pieces[WhiteQueens], notOponent, allWithoutKing);
        oponentKingAttacks = GetPseudoLegalKingBBs(pieces[WhiteKing], notOponent);
        u64 underAttack = oponentPawnsAttacks | oponentKnightsAttacks | oponentBishopsAttacks | oponentRooksAttacks | oponentQueensAttacks | oponentKingAttacks;

        plKing = GetPseudoLegalKingBBs(pieces[BlackKing], notAllay) & (~underAttack);
        int kingSquare = __builtin_ctzll(pieces[BlackKing]);

        u64 attackers = 0ULL;
        if (pieces[BlackKing] & underAttack)
        {
            attackers |= GetKnightsAttacksBBs(pieces[BlackKing]) & pieces[WhiteKnights];
            attackers |= GetPseudoLegalBishopsBBs(pieces[BlackKing], 0xffffffffffffffff, all) & (pieces[WhiteBishops] | pieces[WhiteQueens]);
            attackers |= GetPseudoLegalRooksBBs(pieces[BlackKing], 0xffffffffffffffff, all) & (pieces[WhiteRooks] | pieces[WhiteQueens]);

            if (std::popcount(attackers) > 1)
            {
                GetLegalKingMoves(kingSquare, plKing, moves);
                return;
            }
            else
            {
                int attackerSquare = __builtin_ctzll(attackers);
                u64 posibleMovesMask = BetweenSquaresTable[kingSquare][attackerSquare] | attackers;

                u64f unpinedPieces[4];
                unpinedPieces[PinablePieces::Pawns] = pieces[BlackPawns];
                unpinedPieces[PinablePieces::Bishops] = pieces[BlackBishops];
                unpinedPieces[PinablePieces::Rooks] = pieces[BlackRooks];
                unpinedPieces[PinablePieces::Queens] = pieces[BlackQueens];
                u64 notUnpinPiecesMask = ResolveBlackPinedPieces(pieces, kingSquare, black, white, all, notAllay, empty, posibleMovesMask, unpinedPieces, moves);

                GetLegalBlackPawnsMoves(unpinedPieces[PinablePieces::Pawns], empty, white, pieces[Empassants], posibleMovesMask, moves);
                GetLegalKnightsMoves(pieces[BlackKnights] & notUnpinPiecesMask, notAllay, posibleMovesMask, moves);
                GetLegalBishopsMoves(unpinedPieces[PinablePieces::Bishops], notAllay, all, posibleMovesMask, moves);
                GetLegalRooksMoves(unpinedPieces[PinablePieces::Rooks], notAllay, all, posibleMovesMask, moves);
                GetLegalQueensMoves(unpinedPieces[PinablePieces::Queens], notAllay, all, posibleMovesMask, moves);
                GetLegalKingMoves(kingSquare, plKing, moves);
            }
        }
        else
        {
            u64 posibleMovesMask = 0xffffffffffffffff;

            u64f unpinedPieces[4];
            unpinedPieces[PinablePieces::Pawns] = pieces[BlackPawns];
            unpinedPieces[PinablePieces::Bishops] = pieces[BlackBishops];
            unpinedPieces[PinablePieces::Rooks] = pieces[BlackRooks];
            unpinedPieces[PinablePieces::Queens] = pieces[BlackQueens];
            u64 notUnpinPiecesMask = ResolveBlackPinedPieces(pieces, kingSquare, black, white, all, notAllay, empty, posibleMovesMask, unpinedPieces, moves);

            GetLegalBlackPawnsMoves(unpinedPieces[PinablePieces::Pawns], empty, white, pieces[Empassants], posibleMovesMask, moves);
            GetLegalKnightsMoves(pieces[BlackKnights] & notUnpinPiecesMask, notAllay, posibleMovesMask, moves);
            GetLegalBishopsMoves(unpinedPieces[PinablePieces::Bishops], notAllay, all, posibleMovesMask, moves);
            GetLegalRooksMoves(unpinedPieces[PinablePieces::Rooks], notAllay, all, posibleMovesMask, moves);
            GetLegalQueensMoves(unpinedPieces[PinablePieces::Queens], notAllay, all, posibleMovesMask, moves);
            GetLegalKingMoves(kingSquare, plKing, moves);
            // TODO: dodanie roszad dla czarnych
            return;
        }
    }
}

u64 MoveGenerator::GetWhitePawnsAttacksBBs(u64f pawns)
{
    u64 leftAttack = (pawns << 7) & NOT_COL_A;
    u64 rightAttack = (pawns << 9) & NOT_COL_H;

    return leftAttack | rightAttack;
}
u64 MoveGenerator::GetBlackPawnsAttacksBBs(u64f pawns)
{
    u64 leftAttack = (pawns << 7) & NOT_COL_A;
    u64 rightAttack = (pawns << 9) & NOT_COL_H;

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

u64 MoveGenerator::GetPseudoLegalBishopsBBs(u64f bishops, u64 notAllay, u64 blockers)
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
        moves |= bishopMoves & notAllay;
    }
    return moves;
}
u64 MoveGenerator::GetPseudoLegalRooksBBs(u64f rooks, u64 notAllay, u64 blockers)
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
        moves |= rookMoves & notAllay;
    }
    return moves;
}
u64 MoveGenerator::GetPseudoLegalQueensBBs(u64f queens, u64 notAllay, u64 blockers)
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
        moves |= queenMoves & notAllay;
    }
    return moves;
}

u64 MoveGenerator::GetPseudoLegalKingBBs(u64f king, u64 notAllay)
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

    return moves & notAllay;
}

// TODO: wymagane sprawdzenie związania po enpassancie
void MoveGenerator::GetLegalWhitePawnsMoves(u64f pawns, u64 empty, u64 black, u64 enpassants, u64 posibleMovesMask, std::queue<Move> &moves)
{
    pawns &= posibleMovesMask;
    u64 singlePush = (pawns << 8) & empty;
    u64 doublePush = ((singlePush & WHITE_DUBLE_PUSH) << 8) & empty;

    black |= enpassants;
    u64 leftAttack = ((pawns & NOT_COL_A) << 7) & black & posibleMovesMask;
    u64 rightAttack = ((pawns & NOT_COL_H) << 9) & black & posibleMovesMask;

    while (singlePush)
    {
        int dest = __builtin_ctzll(singlePush);
        singlePush &= singlePush - 1;

        int from = dest - 8;
        moves.push(Move(from, dest));
    }

    while (doublePush)
    {
        int dest = __builtin_ctzll(doublePush);
        doublePush &= doublePush - 1;

        int from = dest - 16;
        moves.push(Move(from, dest));
    }

    while (leftAttack)
    {
        int dest = __builtin_ctzll(leftAttack);
        leftAttack &= leftAttack - 1;

        int from = dest - 7;
        moves.push(Move(from, dest));
    }

    while (rightAttack)
    {
        int dest = __builtin_ctzll(rightAttack);
        rightAttack &= rightAttack - 1;

        int from = dest - 9;
        moves.push(Move(from, dest));
    }
}
void MoveGenerator::GetLegalBlackPawnsMoves(u64f pawns, u64 empty, u64 white, u64 enpassants, u64 posibleMovesMask, std::queue<Move> &moves)
{
    pawns &= posibleMovesMask;
    u64 singlePush = (pawns >> 8) & empty;
    u64 doublePush = ((singlePush & BLACK_DUBLE_PUSH) >> 8) & empty;

    white |= enpassants;
    u64 leftAttack = ((pawns & NOT_COL_A) >> 9) & white & posibleMovesMask;
    u64 rightAttack = ((pawns & NOT_COL_H) >> 7) & white & posibleMovesMask;

    while (singlePush)
    {
        int dest = __builtin_ctzll(singlePush);
        singlePush &= singlePush - 1;

        int from = dest + 8;
        moves.push(Move(from, dest));
    }

    while (doublePush)
    {
        int dest = __builtin_ctzll(doublePush);
        doublePush &= doublePush - 1;

        int from = dest + 16;
        moves.push(Move(from, dest));
    }

    while (leftAttack)
    {
        int dest = __builtin_ctzll(leftAttack);
        leftAttack &= leftAttack - 1;

        int from = dest + 9;
        moves.push(Move(from, dest));
    }

    while (rightAttack)
    {
        int dest = __builtin_ctzll(rightAttack);
        rightAttack &= rightAttack - 1;

        int from = dest + 7;
        moves.push(Move(from, dest));
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

u64 MoveGenerator::ResolveWhitePinedPieces(const u64f *pieces, int kingSquare, u64 white, u64 black,
                                           u64 all, u64 notAllay, u64 empty, u64 posibleMovesMask, u64f (&unpinedPieces)[4], std::queue<Move> &moves)
{
    u64 horizontalBlockersMask = RookMagicBitboards::GetBlockersMask(kingSquare);
    u64 diagonalBlockersMask = BishopMagicBitboards::GetBlockersMask(kingSquare);
    u64 horizontalPinsMask = RookMagicBitboards::GetMovesMask(kingSquare, black & horizontalBlockersMask);
    u64 diagonalPinsMask = BishopMagicBitboards::GetMovesMask(kingSquare, black & diagonalBlockersMask);
    u64 notPinningLinesMask = ~(horizontalPinsMask | diagonalPinsMask);
    u64 notPinnedPiecesMask = 0xffffffffffffffff;

    u64 temp = horizontalPinsMask & pieces[BlackRooks];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinLine = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinLine & white) == 1)
        {
            notPinnedPiecesMask ^= posiblePinLine;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinLine))
            {
                // not generating moves for piece becos thera are none
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
        }
    }

    temp = horizontalPinsMask & pieces[BlackQueens];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinLine = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinLine & white) == 1)
        {
            notPinnedPiecesMask ^= posiblePinLine;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
        }
    }

    temp = diagonalPinsMask & pieces[BlackBishops];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinDiagonal = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinDiagonal & white) == 1)
        {
            notPinnedPiecesMask ^= posiblePinDiagonal;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
        }
    }

    temp = diagonalPinsMask & pieces[BlackQueens];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinDiagonal = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinDiagonal & white) == 1)
        {
            notPinnedPiecesMask ^= posiblePinDiagonal;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
        }
    }

    return notPinnedPiecesMask;
}

u64 MoveGenerator::ResolveBlackPinedPieces(const u64f *pieces, int kingSquare, u64 white, u64 black,
                                           u64 all, u64 notAllay, u64 empty, u64 posibleMovesMask, u64f (&unpinedPieces)[4], std::queue<Move> &moves)
{
    u64 horizontalBlockersMask = RookMagicBitboards::GetBlockersMask(kingSquare);
    u64 diagonalBlockersMask = BishopMagicBitboards::GetBlockersMask(kingSquare);
    u64 horizontalPinsMask = RookMagicBitboards::GetMovesMask(kingSquare, white & horizontalBlockersMask);
    u64 diagonalPinsMask = BishopMagicBitboards::GetMovesMask(kingSquare, white & diagonalBlockersMask);
    u64 notPinningLinesMask = ~(horizontalPinsMask | diagonalPinsMask);
    u64 notPinnedPiecesMask = 0xffffffffffffffff;

    u64 temp = horizontalPinsMask & pieces[WhiteRooks];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinLine = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinLine & black) == 1)
        {
            notPinnedPiecesMask ^= posiblePinLine;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalBlackPawnsMoves(pinned_piece, empty, white, pieces[Empassants], posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
        }
    }

    temp = horizontalPinsMask & pieces[WhiteQueens];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinLine = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinLine & black) == 1)
        {
            notPinnedPiecesMask ^= posiblePinLine;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalBlackPawnsMoves(pinned_piece, empty, white, pieces[Empassants], posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
        }
    }

    temp = diagonalPinsMask & pieces[WhiteBishops];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinDiagonal = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinDiagonal & black) == 1)
        {
            notPinnedPiecesMask ^= posiblePinDiagonal;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalBlackPawnsMoves(pinned_piece, empty, white, pieces[Empassants], posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
        }
    }

    temp = diagonalPinsMask & pieces[WhiteQueens];
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;

        u64 posiblePinDiagonal = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
        if (std::popcount(posiblePinDiagonal & black) == 1)
        {
            notPinnedPiecesMask ^= posiblePinDiagonal;
            u64 pinned_piece;
            if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Pawns] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Pawns] ^= pinned_piece;
                GetLegalBlackPawnsMoves(pinned_piece, empty, white, pieces[Empassants], posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
        }
    }

    return notPinnedPiecesMask;
}
