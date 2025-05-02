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
        u64 allWithoutKing = all & (~pieces[WhiteKing]);
        u64 horizontalAttackers = pieces[BlackQueens] | pieces[BlackRooks];

        oponentPawnsAttacks = GetBlackPawnsAttacksBBs(pieces[BlackPawns]);
        oponentKnightsAttacks = GetKnightsAttacksBBs(pieces[BlackKnights]);
        oponentBishopsAttacks = GetPseudoLegalBishopsBBs(pieces[BlackBishops], allWithoutKing);
        oponentRooksAttacks = GetPseudoLegalRooksBBs(pieces[BlackRooks], allWithoutKing);
        oponentQueensAttacks = GetPseudoLegalQueensBBs(pieces[BlackQueens], allWithoutKing);
        oponentKingAttacks = GetPseudoLegalKingBBs(pieces[BlackKing]);
        u64 underAttack = oponentPawnsAttacks | oponentKnightsAttacks | oponentBishopsAttacks | oponentRooksAttacks | oponentQueensAttacks | oponentKingAttacks;

        plKing = GetPseudoLegalKingBBs(pieces[WhiteKing]) & notAllay & (~underAttack);
        int kingSquare = __builtin_ctzll(pieces[WhiteKing]);

        u64 attackers = 0ULL;
        if (pieces[WhiteKing] & underAttack)
        {
            attackers |= GetWhitePawnsAttacksBBs(pieces[WhiteKing]) & pieces[BlackPawns];
            attackers |= GetKnightsAttacksBBs(pieces[WhiteKing]) & pieces[BlackKnights];
            attackers |= GetPseudoLegalBishopsBBs(pieces[WhiteKing], all) & (pieces[BlackBishops] | pieces[BlackQueens]);
            attackers |= GetPseudoLegalRooksBBs(pieces[WhiteKing], all) & (pieces[BlackRooks] | pieces[BlackQueens]);

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

                GetLegalWhitePawnsMoves(unpinedPieces[PinablePieces::Pawns], pieces[WhiteKing],
                                        empty, all, black, pieces[Empassants], horizontalAttackers, posibleMovesMask, moves);

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

            GetLegalWhitePawnsMoves(unpinedPieces[PinablePieces::Pawns], pieces[WhiteKing],
                                    empty, all, black, pieces[Empassants], horizontalAttackers, posibleMovesMask, moves);
            GetLegalKnightsMoves(pieces[WhiteKnights] & notPinPiecesMask, notAllay, posibleMovesMask, moves);
            GetLegalBishopsMoves(unpinedPieces[PinablePieces::Bishops], notAllay, all, posibleMovesMask, moves);
            GetLegalRooksMoves(unpinedPieces[PinablePieces::Rooks], notAllay, all, posibleMovesMask, moves);
            GetLegalQueensMoves(unpinedPieces[PinablePieces::Queens], notAllay, all, posibleMovesMask, moves);
            GetLegalKingMoves(kingSquare, plKing, moves);

            GetLegalWhiteCasels(all, underAttack, flags.whiteShortCastelRights, flags.whiteLongCastelRights, moves);
            return;
        }
    }
    else
    {
        u64 notAllay = ~black;
        u64 allWithoutKing = all & (~pieces[BlackKing]);
        u64 horizontalAttackers = pieces[WhiteQueens] | pieces[WhiteRooks];

        oponentPawnsAttacks = GetWhitePawnsAttacksBBs(pieces[WhitePawns]);
        oponentKnightsAttacks = GetKnightsAttacksBBs(pieces[WhiteKnights]);
        oponentBishopsAttacks = GetPseudoLegalBishopsBBs(pieces[WhiteBishops], allWithoutKing);
        oponentRooksAttacks = GetPseudoLegalRooksBBs(pieces[WhiteRooks], allWithoutKing);
        oponentQueensAttacks = GetPseudoLegalQueensBBs(pieces[WhiteQueens], allWithoutKing);
        oponentKingAttacks = GetPseudoLegalKingBBs(pieces[WhiteKing]);
        u64 underAttack = oponentPawnsAttacks | oponentKnightsAttacks | oponentBishopsAttacks | oponentRooksAttacks | oponentQueensAttacks | oponentKingAttacks;

        plKing = GetPseudoLegalKingBBs(pieces[BlackKing]) & notAllay & (~underAttack);
        int kingSquare = __builtin_ctzll(pieces[BlackKing]);

        u64 attackers = 0ULL;
        if (pieces[BlackKing] & underAttack)
        {
            attackers |= GetBlackPawnsAttacksBBs(pieces[BlackKing]) & pieces[WhitePawns];
            attackers |= GetKnightsAttacksBBs(pieces[BlackKing]) & pieces[WhiteKnights];
            attackers |= GetPseudoLegalBishopsBBs(pieces[BlackKing], all) & (pieces[WhiteBishops] | pieces[WhiteQueens]);
            attackers |= GetPseudoLegalRooksBBs(pieces[BlackKing], all) & (pieces[WhiteRooks] | pieces[WhiteQueens]);

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
                u64 notUnpinPiecesMask = ResolveBlackPinedPieces(pieces, kingSquare, white, black, all, notAllay, empty, posibleMovesMask, unpinedPieces, moves);

                GetLegalBlackPawnsMoves(unpinedPieces[PinablePieces::Pawns], pieces[BlackKing],
                                        empty, all, white, pieces[Empassants], horizontalAttackers, posibleMovesMask, moves);
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
            u64 notUnpinPiecesMask = ResolveBlackPinedPieces(pieces, kingSquare, white, black, all, notAllay, empty, posibleMovesMask, unpinedPieces, moves);

            GetLegalBlackPawnsMoves(unpinedPieces[PinablePieces::Pawns], pieces[BlackKing],
                                    empty, all, white, pieces[Empassants], horizontalAttackers, posibleMovesMask, moves);
            GetLegalKnightsMoves(pieces[BlackKnights] & notUnpinPiecesMask, notAllay, posibleMovesMask, moves);
            GetLegalBishopsMoves(unpinedPieces[PinablePieces::Bishops], notAllay, all, posibleMovesMask, moves);
            GetLegalRooksMoves(unpinedPieces[PinablePieces::Rooks], notAllay, all, posibleMovesMask, moves);
            GetLegalQueensMoves(unpinedPieces[PinablePieces::Queens], notAllay, all, posibleMovesMask, moves);

            GetLegalKingMoves(kingSquare, plKing, moves);
            GetLegalBlackCasels(all, underAttack, flags.blackShortCastelRights, flags.blackLongCastelRights, moves);
            return;
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

    u64 temp = horizontalPinsMask & (pieces[BlackRooks] | pieces[BlackQueens]);
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
                u64 singlePush = (pinned_piece << 8) & empty & posiblePinLine;
                u64 doublePush = ((singlePush & WHITE_DUBLE_PUSH) << 8) & empty;

                if (singlePush & posibleMovesMask)
                {
                    int dest = __builtin_ctzll(singlePush);
                    int from = dest - 8;
                    moves.push(Move(from, dest));
                }

                if (doublePush & posibleMovesMask)
                {
                    int dest = __builtin_ctzll(doublePush);
                    int from = dest - 16;
                    moves.push(Move(from, dest, DoublePush));
                }
            }

            else if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
        }
    }

    temp = diagonalPinsMask & (pieces[BlackBishops] | pieces[BlackQueens]);
    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;
        u64 pinnerBB = BitboardsIndecies[enemySquare];
        u64 posiblePinDiagonal = BetweenSquaresTable[kingSquare][enemySquare] | pinnerBB;
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

                if (GetWhitePawnsAttacksBBs(pinned_piece) & pinnerBB & posibleMovesMask)
                {
                    moves.push(Move(__builtin_ctzll(pinned_piece), enemySquare));
                }
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
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

    u64 temp = horizontalPinsMask & (pieces[WhiteRooks] | pieces[WhiteQueens]);
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
                u64 singlePush = (pinned_piece >> 8) & empty & posiblePinLine;
                u64 doublePush = ((singlePush & BLACK_DUBLE_PUSH) >> 8) & empty;
                if (singlePush & posibleMovesMask)
                {
                    int dest = __builtin_ctzll(singlePush);
                    int from = dest + 8;
                    moves.push(Move(from, dest));
                }

                if (doublePush & posibleMovesMask)
                {
                    int dest = __builtin_ctzll(doublePush);
                    int from = dest + 16;
                    moves.push(Move(from, dest, DoublePush));
                }
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Rooks] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Rooks] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinLine))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
            }
        }
    }

    temp = diagonalPinsMask & (pieces[WhiteBishops] | pieces[WhiteQueens]);

    while (temp)
    {
        int enemySquare = __builtin_ctzll(temp);
        temp &= temp - 1;
        u64 pinnerBB = BitboardsIndecies[enemySquare];
        u64 posiblePinDiagonal = BetweenSquaresTable[kingSquare][enemySquare] | pinnerBB;

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
                if (GetBlackPawnsAttacksBBs(pinned_piece) & pinnerBB & posibleMovesMask)
                {
                    moves.push(Move(__builtin_ctzll(pinned_piece), enemySquare));
                }
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Bishops] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Bishops] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
            else if ((pinned_piece = unpinedPieces[PinablePieces::Queens] & posiblePinDiagonal))
            {
                unpinedPieces[PinablePieces::Queens] ^= pinned_piece;
                GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
            }
        }
    }

    return notPinnedPiecesMask;
}
