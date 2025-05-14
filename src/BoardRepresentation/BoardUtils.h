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
        uint8_t halfmoveClock;
        uint16_t moves;

        uint8_t whiteShortCastelRights;
        uint8_t whiteLongCastelRights;
        uint8_t blackShortCastelRights;
        uint8_t blackLongCastelRights;
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

enum MovesFlags
{
    NormalMove,
    DoublePush,
    PromotionQueen,
    PromotionRook,
    PromotionKnight,
    PromotionBishop,
    EmpassantMove,
    Castling
};

enum MovesCategories
{
    Quiet = 1,
    Capture = 2,
    Promotion = 4,
    Killer = 8
};

struct Move
{
    uint8_t startingSquere;
    uint8_t destSquere;
    uint8_t flag;
    uint8_t categorie;

    Move() : startingSquere(0), destSquere(0), flag(NormalMove) {}
    Move(uint8_t start, uint8_t dest, uint8_t flag, uint8_t categorie) : startingSquere(start), destSquere(dest), flag(flag), categorie(categorie) {}
    Move(uint8_t start, uint8_t dest, uint8_t categorie) : startingSquere(start), destSquere(dest), flag(NormalMove), categorie(categorie) {}
};

struct MoveList
{
    std::size_t movesCount;
    Move moves[218];

    MoveList() : movesCount(0) {}

    void push(const Move move)
    {
        moves[movesCount++] = move;
    }
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