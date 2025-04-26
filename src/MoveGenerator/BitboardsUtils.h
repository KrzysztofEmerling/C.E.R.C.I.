#pragma once

#include <array>
#include <cstdint>
#include <string>
using String = std::string;
using u64f = uint_fast64_t;
using u64 = uint64_t;

union Flags
{
    u64 FlagsValue;
    struct
    {
        uint8_t whiteOnMove;

        uint8_t whiteShortCastelRights;
        uint8_t whiteLongCastelRights;
        uint8_t blackShortCastelRights;
        uint8_t blackLongCastelRights;

        uint8_t halfmoveClock;
        uint16_t moves;
    };
};

enum BitBoardsIndecis
{
    WhitePawns,
    WhiteKnights,
    WhiteBishops,
    WhiteRooks,
    WhiteQueens,
    WhiteKing,

    BlackPawns,
    BlackKnights,
    BlackBishops,
    BlackRooks,
    BlackQueens,
    BlackKing,

    Empassants,
};

struct Move
{
    uint8_t uStartingSquere;
    uint8_t uDestSquere;
    uint8_t uPromotionPiece;

    Move(uint8_t start, uint8_t dest, uint8_t promo) : uStartingSquere(start), uDestSquere(dest), uPromotionPiece(promo) {}
    Move(uint8_t start, uint8_t dest) : uStartingSquere(start), uDestSquere(dest), uPromotionPiece(0) {}
};

constexpr std::array<u64, 64> GenerateBitboardTable()
{
    std::array<u64, 64> table{};
    for (int i = 0; i < 64; i++)
    {
        table[i] = 1ULL << i;
    }
    return table;
}
constexpr std::array<u64, 64> BitboardsIndecies = GenerateBitboardTable();