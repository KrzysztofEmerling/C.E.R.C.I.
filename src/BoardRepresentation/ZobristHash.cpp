#include "ZobristHash.h"

ZobristHash::ZobristHash() : m_CurrentHash(0ULL)
{
}
ZobristHash::ZobristHash(u64 hash) : m_CurrentHash(hash)
{
}
void ZobristHash::Initialize(const Flags &flags, const u64f (&pieces)[13])
{
    m_CurrentHash = 0ULL;

    if (flags.whiteOnMove)
        ToggleSideToMove();

    for (int piece = 0; piece < 12; piece++)
    {
        u64f temp = pieces[piece];
        while (temp)
        {
            int index = __builtin_ctzll(temp);
            temp &= temp - 1;
            UpdatePiece(index, piece);
        }
    }

    if (pieces[12])
    {
        int file = __builtin_ctzll(pieces[12]) / 8;
        UpdateEnPassant(file);
    }

    UpdateCastling(flags);
}

void ZobristHash::UpdatePiece(int square, int pieceType)
{
    m_CurrentHash ^= ZOBRIST_TABLE.pieceSquareKeys[64 * pieceType + square];
}

void ZobristHash::UpdateCastling(int flags)
{
    m_CurrentHash ^= ZOBRIST_TABLE.castelingFlagsKeys[flags];
}
void ZobristHash::UpdateCastling(const Flags &flags)
{
    int state = (flags.whiteShortCastelRights) | (flags.whiteLongCastelRights << 1) | (flags.blackShortCastelRights << 2) | (flags.blackLongCastelRights << 3);

    m_CurrentHash ^= ZOBRIST_TABLE.castelingFlagsKeys[state];
}

void ZobristHash::UpdateEnPassant(int file)
{
    m_CurrentHash ^= ZOBRIST_TABLE.enpassantFileKeys[file];
}

void ZobristHash::ToggleSideToMove()
{
    m_CurrentHash ^= ZOBRIST_TABLE.whiteOnMoveKey;
}