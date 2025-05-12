#include "BoardUtils.h"
#include <array>
#include <cstdint>

#pragma once

constexpr u64 xorshift64(u64 &state)
{
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 0x676e696c72656d65ULL;
}

struct ZobristHashTable
{
    std::array<u64, 64 * 12> pieceSquareKeys;
    std::array<u64, 16> castelingFlagsKeys;
    std::array<u64, 8> enpassantFileKeys;
    u64 whiteOnMoveKey;

    constexpr ZobristHashTable() : pieceSquareKeys{}, castelingFlagsKeys{}, enpassantFileKeys{}
    {
        u64 seed = 0x6b656973797a726bULL;

        for (int piece = 0; piece < 12; piece++)
        {
            for (int square = 0; square < 64; square++)
            {
                pieceSquareKeys[piece * 64 + square] = xorshift64(seed);
            }
        }

        for (int castelings = 0; castelings < 16; castelings++)
        {
            castelingFlagsKeys[castelings] = xorshift64(seed);
        }

        for (int file = 0; file < 8; file++)
        {
            enpassantFileKeys[file] = xorshift64(seed);
        }
        whiteOnMoveKey = xorshift64(seed);
    }
};
inline constexpr ZobristHashTable ZOBRIST_TABLE{};

class ZobristHash
{
private:
    u64 m_CurrentHash;

public:
    ZobristHash();
    ZobristHash(u64 hash);

    inline u64 GetHash() const { return m_CurrentHash; }

    void UpdatePiece(int square, int pieceType);
    void UpdateCastling(int flags);
    void UpdateCastling(Flags flags);
    void UpdateEnPassant(int file);
    void ToggleSideToMove();
};