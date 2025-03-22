#pragma once

#include <array>
#include <cstdint>
#include <string>
using String = std::string;
using u64f = uint_fast64_t;
using u64 = uint64_t;

union Flags
{
    u64 uFlagsValue;  //do wykorzystania w switch
    struct
    {
        uint8_t uWhiteOnMove;
    
        uint8_t uWhiteShortCastelRights;
        uint8_t uWhiteLongCastelRights;
        uint8_t uBlackShortCastelRights;
        uint8_t uBlackLongCastelRights;

        uint8_t uHalfmoveClock; 
        uint16_t uMoves;        
    };
};

enum BitBoardsIndecis
{
    WhitePowns,
    WhiteKnights,
    WhiteBishops,
    WhiteRooks,
    WhiteQueens,
    WhiteKing,

    BlackPowns,
    BlackKnights,
    BlackBishops,
    BlackRooks,
    BlackQueens,
    BlackKing,

    Empassants,
};

constexpr std::array<u64, 64> generateBitboardTable() 
{
    std::array<u64, 64> table{};
    for (int i = 0; i < 64; i++) {
        table[i] = 1ULL << i;
    }
    return table;
}
constexpr std::array<u64, 64> BitboardsIndecies = generateBitboardTable();