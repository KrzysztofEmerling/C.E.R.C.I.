#include <memory>

#include "BoardUtils.h"
#include "ZobristHash.h"
#include "config.h"

struct TTMovegenEntry
{
    u64 zHash = 0;  // domyślnie pusty hash
    MoveList moves; // lista ruchów
};

constexpr std::size_t TT_MOVEGEN_ENTRIES_COUNT = MB_TO_BYTES(TT_MOVEGEN_SIZE_MB) / sizeof(TTMovegenEntry);

class MoveGenTT
{
private:
    std::unique_ptr<TTMovegenEntry[]> m_MovegenTT = std::make_unique<TTMovegenEntry[]>(TT_MOVEGEN_ENTRIES_COUNT);

    std::size_t indexFromHash(u64 hash) const
    {
        return hash % TT_MOVEGEN_ENTRIES_COUNT;
    }

public:
    void store(u64 hash, const MoveList &moves)
    {
        std::size_t index = indexFromHash(hash);
        std::size_t movesCount = moves.movesCount;
        m_MovegenTT[index].zHash = hash;
        m_MovegenTT[index].moves.movesCount = movesCount;
        for (size_t i = 0; i < movesCount; ++i)
            m_MovegenTT[index].moves.moves[i] = moves.moves[i];
    }

    bool probe(u64 hash, MoveList &outMoves) const
    {
        std::size_t index = indexFromHash(hash);
        const TTMovegenEntry &entry = m_MovegenTT[index];

        if (entry.zHash == hash)
        {
            outMoves = entry.moves;
            return true;
        }

        return false;
    }
};
