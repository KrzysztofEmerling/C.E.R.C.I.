#pragma once
#include<queue>

#include "BitboardsUtils.h"
#include "BoardState.h"

struct Move
{
    uint8_t uStartingSquere;
    uint8_t uDestSquere;
};

class MoveGenerator
{
public:
    static void GetLegalMovesBBs(const BoardState &state, u64f &LegalMovesBBs[6]);
private:
    static u64 GetPseudoLegalWhitePawnsMoves(const u64f powns, const u64 empty, const u64 black_with_enpassants);
    static u64 GetPseudoLegalWhitePawnsMoves(const u64f powns, const u64 empty, const u64 black_with_enpassants, u64 &pownsAttacks);
    static u64 GetPseudoLegalBlackPawnsMoves(const u64f pawns, const u64 empty, const u64 white_with_enpassants);
    static u64 GetPseudoLegalBlackPawnsMoves(const u64f pawns, const u64 empty, const u64 white_with_enpassants, u64 &pownsAttacks);
    
    static u64 GetPseudoLegalKnightsMoves(const u64f knights, const u64 allay);
    static u64 GetKnightsAttacks(const u64f knights);
    
    static u64 GetPseudoLegalRooksMoves(const u64f rooks, const u64 allay, const u64 blockers);
    static u64 GetPseudoLegalBishopsMoves(const u64f bishops, const u64 allay, const u64 blockers);
    static u64 GetPseudoLegalQueensMoves(const u64f queens, const u64 allay, const u64 blockers);

    static u64 GetPseudoLegalKingMoves(const u64f king, const u64 allay);

    static inline constexpr u64 NOT_RANKS_1_2 = 0xFFFFFFFFFFFF0000;
    static inline constexpr u64 NOT_RANK_1 =    0xFFFFFFFFFFFFFF00;
    static inline constexpr u64 NOT_RANKS_7_8 = 0x0000FFFFFFFFFFFF;
    static inline constexpr u64 NOT_RANK_8 =    0x00FFFFFFFFFFFFFF;

    static inline constexpr u64 NOT_COL_H =     0x7F7F7F7F7F7F7F7F;
    static inline constexpr u64 NOT_COL_A =     0xFEFEFEFEFEFEFEFE;
    static inline constexpr u64 NOT_COLS_AB =   0xFCFCFCFCFCFCFCFC;
    static inline constexpr u64 NOT_COLS_GH =   0x3F3F3F3F3F3F3F3F;
};


constexpr u64 GetBetweenSquaresConstexpr(int sq1, int sq2)
{
    if (sq1 == sq2) return 0ULL;

    int file1 = sq1 % 8;
    int rank1 = sq1 / 8;
    int file2 = sq2 % 8;
    int rank2 = sq2 / 8;

    int df = (file2 > file1) ? 1 : (file2 < file1) ? -1 : 0;
    int dr = (rank2 > rank1) ? 1 : (rank2 < rank1) ? -1 : 0;

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
