#include "TT.h"
#include <iostream>

std::size_t TT::indexFromHash(u64 hash) const
{
    return (hash) % TT_COUNT;
}

void TT::store(u64 hash, int depth, int eval)
{
    std::size_t index = indexFromHash(hash);

    TTEntry entry;
    entry.zHash = hash;
    entry.depth = depth;
    entry.eval = eval;

    m_EvalTT[index] = entry;
}

bool TT::probe(u64 hash, int depth, int &eval) const
{
    std::size_t index = indexFromHash(hash);
    const TTEntry &entry = m_EvalTT[index];

    if (entry.zHash == hash && entry.depth == depth)
    {
        eval = entry.eval;
        return true;
    }

    return false;
}
