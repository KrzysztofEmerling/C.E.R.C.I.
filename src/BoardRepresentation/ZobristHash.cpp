#include "ZobristHash.h"

ZobristHash::ZobristHash() : m_CurrentHash(0ULL)
{
}
ZobristHash::ZobristHash(u64 hash) : m_CurrentHash(hash)
{
}

void ZobristHash::UpdatePiece(int square, int pieceType)
{
    m_CurrentHash ^= ZOBRIST_TABLE.pieceSquareKeys[64 * pieceType + square];
}

void ZobristHash::UpdateCastling(int flags)
{
    m_CurrentHash ^= ZOBRIST_TABLE.castelingFlagsKeys[flags];
}
void ZobristHash::UpdateCastling(Flags flags)
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