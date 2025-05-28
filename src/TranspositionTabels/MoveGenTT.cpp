#include "MoveGenTT.h"
#include <iostream>

std::size_t MoveGenTT::indexFromHash(u64 hash) const
{
    return (hash) % TT_MOVEGEN_ENTRIES_COUNT;
}

void MoveGenTT::store(u64 hash, int depth, const MoveList &moves)
{
    std::size_t index = indexFromHash(hash);
    std::size_t movesCount = moves.movesCount;
    m_MovegenTT[index].depth = depth;
    m_MovegenTT[index].zHash = hash;
    m_MovegenTT[index].moves.movesCount = movesCount;
    for (size_t i = 0; i < movesCount; ++i)
        m_MovegenTT[index].moves.moves[i] = moves.moves[i];
}

bool MoveGenTT::probe(u64 hash, int depth, MoveList &outMoves) const
{
    std::size_t index = indexFromHash(hash);
    const TTMovegenEntry &entry = m_MovegenTT[index];

    if (entry.zHash == hash && entry.depth == depth)
    {
        outMoves = entry.moves;
        return true;
    }

    return false;
}
