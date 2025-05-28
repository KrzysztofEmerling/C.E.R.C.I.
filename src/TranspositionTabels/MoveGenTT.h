#pragma once

#include <memory>
#include "BoardUtils.h"
#include "ZobristHash.h"
#include "config.h"

struct TTMovegenEntry
{
    int depth = 0;
    u64 zHash = 0;
    MoveList moves;
};

constexpr std::size_t TT_MOVEGEN_ENTRIES_COUNT = MB_TO_BYTES(TT_MOVEGEN_SIZE_MB) / sizeof(TTMovegenEntry);

class MoveGenTT
{
private:
    std::unique_ptr<TTMovegenEntry[]> m_MovegenTT = std::make_unique<TTMovegenEntry[]>(TT_MOVEGEN_ENTRIES_COUNT);

    std::size_t indexFromHash(u64 hash) const;

public:
    void store(u64 hash, int depth, const MoveList &moves);
    bool probe(u64 hash, int depth, MoveList &outMoves) const;
};
