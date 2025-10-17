#pragma once

#include <memory>
#include "BoardUtils.h"
#include "ZobristHash.h"
#include "config.h"

struct TTEntry
{
    u64 zHash = 0ULL;
    int depth = 0;
    int eval = 0;
};

constexpr std::size_t TT_COUNT = MB_TO_BYTES(TT_EVAL_SIZE_MB) / sizeof(TTEntry);

class TT
{
private:
    std::unique_ptr<TTEntry[]> m_EvalTT = std::make_unique<TTEntry[]>(TT_COUNT);

    std::size_t indexFromHash(u64 hash) const;

public:
    void store(u64 hash, int depth, int eval);
    bool probe(u64 hash, int depth, int &eval) const;
    void Clear();
};
