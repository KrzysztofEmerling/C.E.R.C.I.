#pragma once
#include <queue>
#include <array>
#include "BoardUtils.h"
#include "BoardState.h"

class MoveGenerator
{
    friend class Eval;

public:
    static void GetLegalMoves(const BoardState &state, MoveList &moves);

    static bool IsKingInCheck(bool isWhiteTurn, const u64f (&pieces)[13]);

private:
    enum PinablePieces
    {
        Pawns,
        Bishops,
        Rooks,
        Queens
    };

    static u64 GetWhitePawnsAttacksBBs(u64f powns);
    static u64 GetBlackPawnsAttacksBBs(u64f pawns);

    static u64 GetKnightsAttacksBBs(u64f knights);

    static u64 GetPseudoLegalRooksBBs(u64f rooks, u64 blockers);
    static u64 GetPseudoLegalBishopsBBs(u64f bishops, u64 blockers);
    static u64 GetPseudoLegalQueensBBs(u64f queens, u64 blockers);
    static u64 GetPseudoLegalKingBBs(u64f king);

    static bool EnPassantRevealsCheck(int kingSquare, int from, u64 all, u64 horizontalAttackers, u64 rankMask);
    static void GetLegalWhitePawnsMoves(u64f pawns, u64f king, u64 empty, u64 all, u64 black, u64 enpassants, u64 horizontalAttackers, u64 posibleMovesMask, MoveList &moves);
    static void GetLegalBlackPawnsMoves(u64f pawns, u64f king, u64 empty, u64 all, u64 white, u64 enpassants, u64 horizontalAttackers, u64 posibleMovesMask, MoveList &moves);

    static void GetLegalKingMoves(int square, u64 movementBBs, u64 enemys, MoveList &moves);

    static void GetLegalKnightsMoves(u64f knights, u64 notAllay, u64 posibleMovesMask, u64 enemys, MoveList &moves);

    static void GetLegalRooksMoves(u64f rooks, u64 notAllay, u64 blockers, u64 posibleMovesMask, u64 enemys, MoveList &moves);
    static void GetLegalBishopsMoves(u64f bishops, u64 notAllay, u64 blockers, u64 posibleMovesMask, u64 enemys, MoveList &moves);
    static void GetLegalQueensMoves(u64f queens, u64 notAllay, u64 blockers, u64 posibleMovesMask, u64 enemys, MoveList &moves);

    static void GetLegalWhiteCasels(u64 allay, u64 attacks, bool shortCastelRights, bool longCastelRights, MoveList &moves);
    static void GetLegalBlackCasels(u64 allay, u64 attacks, bool shortCastelRights, bool longCastelRights, MoveList &moves);

    static u64 ResolveWhitePinedPieces(const u64f *pieces, int kingSquare, u64 white, u64 black,
                                       u64 all, u64 notAllay, u64 empty, u64 posibleMovesMask, u64f (&unpinedPieces)[4], MoveList &moves);

    static u64 ResolveBlackPinedPieces(const u64f *pieces, int kingSquare, u64 white, u64 black,
                                       u64 all, u64 notAllay, u64 empty, u64 posibleMovesMask, u64f (&unpinedPieces)[4], MoveList &moves);

    static inline constexpr u64 RANK4 = 0x00000000FF000000;
    static inline constexpr u64 RANK5 = 0x000000FF00000000;

    static inline constexpr u64 WHITE_DUBLE_PUSH = 0x0000000000FF0000;
    static inline constexpr u64 BLACK_DUBLE_PUSH = 0x0000FF0000000000;
    static inline constexpr u64 WHITE_PROMOTION_LINE = 0xFF00000000000000;
    static inline constexpr u64 BLACK_PROMOTION_LINE = 0x00000000000000FF;

    static inline constexpr u64 NOT_COL_H = 0x7F7F7F7F7F7F7F7F;
    static inline constexpr u64 NOT_COL_A = 0xFEFEFEFEFEFEFEFE;
    static inline constexpr u64 NOT_COLS_AB = 0xFCFCFCFCFCFCFCFC;
    static inline constexpr u64 NOT_COLS_GH = 0x3F3F3F3F3F3F3F3F;

    static inline constexpr u64 WHITE_SHORT_CASTLE_MASK = 0x0000000000000060; // f1, g1
    static inline constexpr u64 WHITE_LONG_CASTLE_MASK = 0x000000000000000C;  // c1, d1
    static inline constexpr u64 WHITE_LONG_ROOK_BLOCK = 0x000000000000000E;   // b1, c1, d1

    static inline constexpr u64 BLACK_SHORT_CASTLE_MASK = 0x6000000000000000; // f8, g8
    static inline constexpr u64 BLACK_LONG_CASTLE_MASK = 0x0C00000000000000;  // c8, d8
    static inline constexpr u64 BLACK_LONG_ROOK_BLOCK = 0x0E00000000000000;   // b8, c8, d8
};

constexpr u64 GetBetweenSquaresConstexpr(int sq1, int sq2)
{
    if (sq1 == sq2)
        return 0ULL;

    int file1 = sq1 % 8;
    int rank1 = sq1 / 8;
    int file2 = sq2 % 8;
    int rank2 = sq2 / 8;

    int df = (file2 > file1) ? 1 : (file2 < file1) ? -1
                                                   : 0;
    int dr = (rank2 > rank1) ? 1 : (rank2 < rank1) ? -1
                                                   : 0;

    // Sprawdzenie, czy pola leżą na linii prostej lub diagonalnej
    if (df != 0 && dr != 0 && (abs(file2 - file1) != abs(rank2 - rank1)))
        return 0ULL;

    u64 between = 0ULL;
    int f = file1 + df;
    int r = rank1 + dr;

    while (f != file2 || r != rank2)
    {
        between |= (1ULL << (r * 8 + f));
        f += df;
        r += dr;
    }

    // Usuń pole atakującego (ostatnie pole)
    between &= ~(1ULL << sq2);

    return between;
}
constexpr auto GenerateBetweenSquaresTable()
{
    std::array<std::array<u64, 64>, 64> table{};

    for (int sq1 = 0; sq1 < 64; ++sq1)
    {
        for (int sq2 = 0; sq2 < 64; ++sq2)
        {
            table[sq1][sq2] = GetBetweenSquaresConstexpr(sq1, sq2);
        }
    }

    return table;
}
constexpr auto BetweenSquaresTable = GenerateBetweenSquaresTable();

template <u64 (*Func)(int)>
constexpr auto generate_lut()
{
    std::array<u64, 64> table{};
    for (int sq = 0; sq < 64; sq++)
        table[sq] = Func(sq);
    return table;
}
constexpr u64 whitePawnAttacks(int sq)
{
    u64 p = 1ULL << sq;
    return ((p & NOT_COL_A) << 7) |
           ((p & NOT_COL_H) << 9);
}
constexpr u64 blackPawnAttacks(int sq)
{
    u64 p = 1ULL << sq;
    return ((p & NOT_COL_A) >> 9) |
           ((p & NOT_COL_H) >> 7);
}
constexpr u64 knightAttacks(int sq)
{
    u64 k = 1ULL << sq;
    return ((NOT_COL_H & k) << 17) |
           ((NOT_COL_A & k) << 15) |
           ((NOT_COLS_GH & k) << 10) |
           ((NOT_COLS_AB & k) << 6)  |
           ((NOT_COL_H & k) >> 15) |
           ((NOT_COL_A & k) >> 17) |
           ((NOT_COLS_GH & k) >> 6) |
           ((NOT_COLS_AB & k) >> 10);
}
constexpr u64 knightAttacks(int sq)
{
    u64 k = 1ULL << sq;
    return ((NOT_COL_H & k) << 17) |
           ((NOT_COL_A & k) << 15) |
           ((NOT_COLS_GH & k) << 10) |
           ((NOT_COLS_AB & k) << 6)  |
           ((NOT_COL_H & k) >> 15) |
           ((NOT_COL_A & k) >> 17) |
           ((NOT_COLS_GH & k) >> 6) |
           ((NOT_COLS_AB & k) >> 10);
}

constexpr auto WhitePawnAttacksLUT = generate_lut<white_pawn_attacks>();
constexpr auto BlackPawnAttacksLUT = generate_lut<black_pawn_attacks>();
constexpr auto KnightAttacksLUT = generate_lut<knight_attacks>();
constexpr auto KingAttacksLUT = generate_lut<king_attacks>();