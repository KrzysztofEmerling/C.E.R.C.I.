#pragma once
#include <queue>

#include "BitboardsUtils.h"
#include "BoardState.h"

class MoveGenerator
{
public:
    static void GetLegalMoves(const BoardState &state, std::queue<Move> &moves);

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

    static void GetLegalWhitePawnsMoves(u64f pawns, u64 empty, u64 black, u64 enpassants, u64 posibleMovesMask, std::queue<Move> &moves);
    static void GetLegalBlackPawnsMoves(u64f pawns, u64 empty, u64 white, u64 enpassants, u64 posibleMovesMask, std::queue<Move> &moves);

    static void GetLegalKingMoves(int square, u64 movementBBs, std::queue<Move> &moves);

    static void GetLegalKnightsMoves(u64f knights, u64 notAllay, u64 posibleMovesMask, std::queue<Move> &moves);

    static void GetLegalRooksMoves(u64f rooks, u64 notAllay, u64 blockers, u64 posibleMovesMask, std::queue<Move> &moves);
    static void GetLegalBishopsMoves(u64f bishops, u64 notAllay, u64 blockers, u64 posibleMovesMask, std::queue<Move> &moves);
    static void GetLegalQueensMoves(u64f queens, u64 notAllay, u64 blockers, u64 posibleMovesMask, std::queue<Move> &moves);

    static void GetLegalWhiteCasels(u64 allay, u64 attacks, bool shortCastelRights, bool longCastelRights, std::queue<Move> &moves);
    static void GetLegalBlackCasels(u64 allay, u64 attacks, bool shortCastelRights, bool longCastelRights, std::queue<Move> &moves);

    static u64 ResolveWhitePinedPieces(const u64f *pieces, int kingSquare, u64 white, u64 black,
                                       u64 all, u64 notAllay, u64 empty, u64 posibleMovesMask, u64f (&unpinedPieces)[4], std::queue<Move> &moves);

    static u64 ResolveBlackPinedPieces(const u64f *pieces, int kingSquare, u64 white, u64 black,
                                       u64 all, u64 notAllay, u64 empty, u64 posibleMovesMask, u64f (&unpinedPieces)[4], std::queue<Move> &moves);

    static inline constexpr u64 WHITE_DUBLE_PUSH = 0x0000000000FF0000;
    static inline constexpr u64 BLACK_DUBLE_PUSH = 0x0000FF0000000000;
    static inline constexpr u64 WHITE_PROMOTION_LINE = 0xFF00000000000000;
    static inline constexpr u64 BLACK_PROMOTION_LINE = 0x00000000000000FF;

    static inline constexpr u64 NOT_COL_H = 0x7F7F7F7F7F7F7F7F;
    static inline constexpr u64 NOT_COL_A = 0xFEFEFEFEFEFEFEFE;
    static inline constexpr u64 NOT_COLS_AB = 0xFCFCFCFCFCFCFCFC;
    static inline constexpr u64 NOT_COLS_GH = 0x3F3F3F3F3F3F3F3F;

    static inline constexpr u64 WHITE_SHORT_CASTLE_MASK = 0x0000000000000060; // f1, g1
    static inline constexpr u64 WHITE_LONG_CASTLE_MASK = 0x000000000000000E;  // b1, c1, d1
    static inline constexpr u64 WHITE_LONG_ROOK_BLOCK = 0x0000000000000002;   // b1

    static inline constexpr u64 BLACK_SHORT_CASTLE_MASK = 0x6000000000000000; // f8, g8
    static inline constexpr u64 BLACK_LONG_CASTLE_MASK = 0x0E00000000000000;  // b8, c8, d8
    static inline constexpr u64 BLACK_LONG_ROOK_BLOCK = 0x0200000000000000;   // b8
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
