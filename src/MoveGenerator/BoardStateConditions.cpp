#include "BoardState.h"
#include "MoveGenerator.h"

bool BoardState::IsThreefoldRepetition() const
{
    u64 hash = m_ZHash.GetHash();
    int repCounter = 0;
    for (int i = 0; i < m_Flags.halfmoveClock; i++)
    {
        if (hash == m_PreviousMovesHashes[i])
        {
            if (++repCounter > 1)
                return true;
        }
    }
    return false;
}
bool BoardState::IsFiftyMoveRule() const
{
    return m_Flags.halfmoveClock >= 100;
}
bool BoardState::IsCheckmate() const
{
    MoveList movesList;
    MoveGenerator::GetLegalMoves(*this, movesList);

    if (movesList.movesCount == 0)
    {
        return MoveGenerator::IsKingInCheck(m_Flags.whiteOnMove, m_Pieces);
    }
    return false;
}
bool BoardState::IsStalemate() const
{
    MoveList movesList;
    MoveGenerator::GetLegalMoves(*this, movesList);

    if (movesList.movesCount == 0)
    {
        return !MoveGenerator::IsKingInCheck(m_Flags.whiteOnMove, m_Pieces);
    }
    return false;
}
bool BoardState::IsInsufficientMaterial() const
{
    // Bez zgodności z 2 gońcami tego samego koloru
    return !((std::popcount(m_Pieces[WhitePawns] | m_Pieces[WhiteRooks] | m_Pieces[WhiteQueens])) |
             (std::popcount(m_Pieces[WhiteBishops]) > 1) |
             (m_Pieces[WhiteBishops] && std::popcount(m_Pieces[WhiteBishops] | m_Pieces[WhiteKnights]) > 1) |
             (std::popcount(m_Pieces[WhiteKnights]) > 2) |

             (std::popcount(m_Pieces[BlackPawns] | m_Pieces[BlackRooks] | m_Pieces[BlackQueens])) |
             (std::popcount(m_Pieces[BlackBishops]) > 1) |
             (m_Pieces[BlackBishops] && std::popcount(m_Pieces[BlackBishops] | m_Pieces[BlackKnights]) > 1) |
             (std::popcount(m_Pieces[BlackKnights]) > 2));
}