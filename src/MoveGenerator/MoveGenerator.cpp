#include "MoveGenerator.h"
#include "MagicBitboards/RookMagicBitboards.h"
#include "MagicBitboards/BishopMagicBitboards.h"
#include <queue>

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

    if (flags.uWhiteOnMove)
    {
        u64 notAllay = ~white;
        u64 notOponent = ~black;
        u64 allWithoutKing = all & (~pieces[WhiteKing]);

        // atakowane pola
        oponentPawnsAttacks = GetBlackPawnsAttacksBBs(pieces[BlackPawns]);
        oponentKnightsAttacks = GetKnightsAttacksBBs(pieces[BlackKnights]);
        oponentBishopsAttacks = GetPseudoLegalBishopsBBs(pieces[BlackBishops], notOponent, allWithoutKing);
        oponentRooksAttacks = GetPseudoLegalRooksBBs(pieces[BlackRooks], notOponent, allWithoutKing);
        oponentQueensAttacks = GetPseudoLegalQueensBBs(pieces[BlackQueens], notOponent, allWithoutKing);
        oponentKingAttacks = GetPseudoLegalKingBBs(pieces[BlackKing], notOponent);
        u64 underAttack = oponentPawnsAttacks | oponentKnightsAttacks | oponentBishopsAttacks | oponentRooksAttacks | oponentQueensAttacks | oponentKingAttacks;

        // legalne ruchy
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
                GetLegalKingMovesFromBBs(kingSquare, plKing, moves);
                return;
            }
            else
            {
                int attackerSquare = __builtin_ctzll(attackers);
                u64 posibleMovesMask = BetweenSquaresTable[kingSquare][attackerSquare] | attackers;

                /////////////////////////////RuchyZwiązanychFigur//////////////////////////////////////
                u64f unpinPawns = pieces[WhitePawns];
                u64f unpinBishops = pieces[WhiteBishops];
                u64f unpinRooks = pieces[WhiteRooks];
                u64f unpinQueens = pieces[WhiteQueens];

                u64 horizontalBlockersMask = RookMagicBitboards::GetBlockersMask(kingSquare);
                u64 diagonalBlockersMask = BishopMagicBitboards::GetBlockersMask(kingSquare);
                u64 horizontalPinsMask = RookMagicBitboards::GetMovesMask(kingSquare, black & horizontalBlockersMask);
                u64 diagonalPinsMask = BishopMagicBitboards::GetMovesMask(kingSquare, black & diagonalBlockersMask);
                u64 notUnpinPiecesMask = ~(horizontalPinsMask | diagonalPinsMask);

                u64 temp = horizontalPinsMask & pieces[BlackRooks];
                while (temp)
                {
                    int enemySquare = __builtin_ctzll(temp);
                    temp &= temp - 1;

                    u64 posiblePinLine = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
                    if (std::popcount(posiblePinLine & white) == 1)
                    {
                        u64 pinned_piece;
                        if ((pinned_piece = unpinBishops & posiblePinLine))
                        {
                            // not generating moves for piece becos thera are none
                            unpinBishops ^= pinned_piece;
                        }

                        else if ((pinned_piece = unpinPawns & posiblePinLine))
                        {
                            unpinPawns ^= pinned_piece;
                            GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinLine & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinRooks & posiblePinLine))
                        {
                            unpinRooks ^= pinned_piece;
                            GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinQueens & posiblePinLine))
                        {
                            unpinQueens ^= pinned_piece;
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
                        u64 pinned_piece;
                        if ((pinned_piece = unpinBishops & posiblePinLine))
                        {
                            unpinBishops ^= pinned_piece;
                        }

                        else if ((pinned_piece = unpinPawns & posiblePinLine))
                        {
                            unpinPawns ^= pinned_piece;
                            GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinLine & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinRooks & posiblePinLine))
                        {
                            unpinRooks ^= pinned_piece;
                            GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinQueens & posiblePinLine))
                        {
                            unpinQueens ^= pinned_piece;
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
                        u64 pinned_piece;
                        if ((pinned_piece = unpinRooks & posiblePinDiagonal))
                        {
                            unpinRooks ^= pinned_piece;
                        }

                        else if ((pinned_piece = unpinPawns & posiblePinDiagonal))
                        {
                            unpinPawns ^= pinned_piece;
                            GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinDiagonal & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinBishops & posiblePinDiagonal))
                        {
                            unpinBishops ^= pinned_piece;
                            GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinQueens & posiblePinDiagonal))
                        {
                            unpinQueens ^= pinned_piece;
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
                        u64 pinned_piece;
                        if ((pinned_piece = unpinRooks & posiblePinDiagonal))
                        {
                            unpinRooks ^= pinned_piece;
                        }

                        else if ((pinned_piece = unpinPawns & posiblePinDiagonal))
                        {
                            unpinPawns ^= pinned_piece;
                            GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinDiagonal & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinBishops & posiblePinDiagonal))
                        {
                            unpinBishops ^= pinned_piece;
                            GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
                        }
                        else if ((pinned_piece = unpinQueens & posiblePinDiagonal))
                        {
                            unpinQueens ^= pinned_piece;
                            GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal & posibleMovesMask, moves);
                        }
                    }
                }
                ///////////////////////////////////////////////////////////////////////////////////////

                GetLegalWhitePawnsMoves(unpinPawns, empty, black, pieces[Empassants], posibleMovesMask, moves);
                GetLegalKnightsMoves(pieces[WhiteKnights] & notUnpinPiecesMask, notAllay, posibleMovesMask, moves);
                GetLegalBishopsMoves(unpinBishops, notAllay, all, posibleMovesMask, moves);
                GetLegalRooksMoves(unpinRooks, notAllay, all, posibleMovesMask, moves);
                GetLegalQueensMoves(unpinQueens, notAllay, all, posibleMovesMask, moves);
                GetLegalKingMovesFromBBs(kingSquare, plKing, moves);
            }
        }
        else
        {
            u64 posibleMovesMask = 0xffffffffffffffff;

            /////////////////////////////RuchyZwiązanychFigur//////////////////////////////////////
            u64f unpinPawns = pieces[WhitePawns];
            u64f unpinBishops = pieces[WhiteBishops];
            u64f unpinRooks = pieces[WhiteRooks];
            u64f unpinQueens = pieces[WhiteQueens];

            u64 horizontalBlockersMask = RookMagicBitboards::GetBlockersMask(kingSquare);
            u64 diagonalBlockersMask = BishopMagicBitboards::GetBlockersMask(kingSquare);
            u64 horizontalPinsMask = RookMagicBitboards::GetMovesMask(kingSquare, black & horizontalBlockersMask);
            u64 diagonalPinsMask = BishopMagicBitboards::GetMovesMask(kingSquare, black & diagonalBlockersMask);
            u64 notUnpinPiecesMask = ~(horizontalPinsMask | diagonalPinsMask);

            u64 temp = horizontalPinsMask & pieces[BlackRooks];
            while (temp)
            {
                int enemySquare = __builtin_ctzll(temp);
                temp &= temp - 1;

                u64 posiblePinLine = BetweenSquaresTable[kingSquare][enemySquare] | BitboardsIndecies[enemySquare];
                if (std::popcount(posiblePinLine & white) == 1)
                {
                    u64 pinned_piece;
                    if ((pinned_piece = unpinBishops & posiblePinLine))
                    {
                        // not generating moves for piece becos thera are none
                        unpinBishops ^= pinned_piece;
                    }

                    else if ((pinned_piece = unpinPawns & posiblePinLine))
                    {
                        unpinPawns ^= pinned_piece;
                        GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinLine, moves);
                    }
                    else if ((pinned_piece = unpinRooks & posiblePinLine))
                    {
                        unpinRooks ^= pinned_piece;
                        GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine, moves);
                    }
                    else if ((pinned_piece = unpinQueens & posiblePinLine))
                    {
                        unpinQueens ^= pinned_piece;
                        GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinLine, moves);
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
                    u64 pinned_piece;
                    if ((pinned_piece = unpinBishops & posiblePinLine))
                    {
                        unpinBishops ^= pinned_piece;
                    }

                    else if ((pinned_piece = unpinPawns & posiblePinLine))
                    {
                        unpinPawns ^= pinned_piece;
                        GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinLine, moves);
                    }
                    else if ((pinned_piece = unpinRooks & posiblePinLine))
                    {
                        unpinRooks ^= pinned_piece;
                        GetLegalRooksMoves(pinned_piece, notAllay, all, posiblePinLine, moves);
                    }
                    else if ((pinned_piece = unpinQueens & posiblePinLine))
                    {
                        unpinQueens ^= pinned_piece;
                        GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinLine, moves);
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
                    u64 pinned_piece;
                    if ((pinned_piece = unpinRooks & posiblePinDiagonal))
                    {
                        unpinRooks ^= pinned_piece;
                    }

                    else if ((pinned_piece = unpinPawns & posiblePinDiagonal))
                    {
                        unpinPawns ^= pinned_piece;
                        GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinDiagonal, moves);
                    }
                    else if ((pinned_piece = unpinBishops & posiblePinDiagonal))
                    {
                        unpinBishops ^= pinned_piece;
                        GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal, moves);
                    }
                    else if ((pinned_piece = unpinQueens & posiblePinDiagonal))
                    {
                        unpinQueens ^= pinned_piece;
                        GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal, moves);
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
                    u64 pinned_piece;
                    if ((pinned_piece = unpinRooks & posiblePinDiagonal))
                    {
                        unpinRooks ^= pinned_piece;
                    }

                    else if ((pinned_piece = unpinPawns & posiblePinDiagonal))
                    {
                        unpinPawns ^= pinned_piece;
                        GetLegalWhitePawnsMoves(pinned_piece, empty, black, pieces[Empassants], posiblePinDiagonal, moves);
                    }
                    else if ((pinned_piece = unpinBishops & posiblePinDiagonal))
                    {
                        unpinBishops ^= pinned_piece;
                        GetLegalBishopsMoves(pinned_piece, notAllay, all, posiblePinDiagonal, moves);
                    }
                    else if ((pinned_piece = unpinQueens & posiblePinDiagonal))
                    {
                        unpinQueens ^= pinned_piece;
                        GetLegalQueensMoves(pinned_piece, notAllay, all, posiblePinDiagonal, moves);
                    }
                }
            }
            ///////////////////////////////////////////////////////////////////////////////////////

            GetLegalWhitePawnsMoves(unpinPawns, empty, black, pieces[Empassants], posibleMovesMask, moves);
            GetLegalKnightsMoves(pieces[WhiteKnights] & notUnpinPiecesMask, notAllay, posibleMovesMask, moves);
            GetLegalBishopsMoves(unpinBishops, notAllay, all, posibleMovesMask, moves);
            GetLegalRooksMoves(unpinRooks, notAllay, all, posibleMovesMask, moves);
            GetLegalQueensMoves(unpinQueens, notAllay, all, posibleMovesMask, moves);
            GetLegalKingMovesFromBBs(kingSquare, plKing, moves);
            // TODO: dodanie roszad
        }
    }
    else
    {
        // to samo dla czarnych
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
    return (((NOT_COL_H & knights) << 17) |
            // 2 w górę 1 w lewo
            ((NOT_COL_A & knights) << 15) |
            // 1 w górę 2 w prawo
            ((NOT_COLS_AB & knights) << 10) |
            // 1 w gurę 2 w lewo
            ((NOT_COLS_GH & knights) << 6) |

            // 2 w dół 1 w prawo
            ((NOT_COL_H & knights) >> 17) |
            // 2 w dół 1 w lewo
            ((NOT_COL_A & knights) >> 15) |
            // 1 w dół 2 w prawo
            ((NOT_COLS_AB & knights) >> 10) |
            // 1 w dół 2 w lewo
            ((NOT_COLS_GH & knights) >> 6));
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
    u64 singlePush = (pawns << 8) & empty & posibleMovesMask;
    u64 doublePush = ((singlePush & WHITE_DUBLE_PUSH) << 8) & empty & posibleMovesMask;

    black |= enpassants;
    u64 leftAttack = (pawns << 7) & black & NOT_COL_A & posibleMovesMask;
    u64 rightAttack = (pawns << 9) & black & NOT_COL_H & posibleMovesMask;

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
    u64 singlePush = (pawns >> 8) & empty & posibleMovesMask;
    u64 doublePush = ((singlePush & BLACK_DUBLE_PUSH) >> 8) & empty & posibleMovesMask;

    white |= enpassants;
    u64 leftAttack = (pawns >> 7) & white & NOT_COL_A & posibleMovesMask;
    u64 rightAttack = (pawns >> 9) & white & NOT_COL_H & posibleMovesMask;

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

void MoveGenerator::GetLegalKingMovesFromBBs(int square, u64 movementBBs, std::queue<Move> &moves)
{
    while (movementBBs)
    {
        int dest_square = __builtin_ctzll(movementBBs);
        movementBBs &= movementBBs - 1;
        Move move(square, dest_square);
        moves.push(move);
    }
}

// TODO: Wymagana optymalizacja!
void MoveGenerator::GetLegalKnightsMoves(u64f knights, u64 notAllay, u64 posibleMovesMask, std::queue<Move> &moves)
{
    knights;
    while (knights)
    {
        int square = __builtin_ctzll(knights);
        knights &= knights - 1;

        u64 temp = BitboardsIndecies[square];

        u64 destSquaresBBs = (((NOT_COL_H & temp) << 17) |
                              // 2 w górę 1 w lewo
                              ((NOT_COL_A & temp) << 15) |
                              // 1 w górę 2 w prawo
                              ((NOT_COLS_AB & temp) << 10) |
                              // 1 w gurę 2 w lewo
                              ((NOT_COLS_GH & temp) << 6) |

                              // 2 w dół 1 w prawo
                              ((NOT_COL_H & temp) >> 17) |
                              // 2 w dół 1 w lewo
                              ((NOT_COL_A & temp) >> 15) |
                              // 1 w dół 2 w prawo
                              ((NOT_COLS_AB & temp) >> 10) |
                              // 1 w dół 2 w lewo
                              ((NOT_COLS_GH & temp) >> 6)) &
                             notAllay & posibleMovesMask;

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

        u64 queenMoves = RookMagicBitboards::GetMovesMask(square, blockers & rookBlockersMask);
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