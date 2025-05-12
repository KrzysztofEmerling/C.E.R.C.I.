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
            if (++repCounter > 2)
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
    std::queue<Move> moves;
    MoveGenerator::GetLegalMoves(*this, moves);

    if (moves.empty())
    {
        return MoveGenerator::IsKingInCheck(m_Flags.whiteOnMove, m_Pieces);
    }
    return false;
}
bool BoardState::IsStalemate() const
{
    std::queue<Move> moves;
    MoveGenerator::GetLegalMoves(*this, moves);

    if (moves.empty())
    {
        return !MoveGenerator::IsKingInCheck(m_Flags.whiteOnMove, m_Pieces);
    }
    return false;
}
bool BoardState::IsInsufficientMaterial() const
{
    if (m_Flags.whiteOnMove)
    {
        if (std::popcount(m_Pieces[WhitePawns] | m_Pieces[WhiteRooks] | m_Pieces[WhiteQueens]))
            return false;

        if (std::popcount(m_Pieces[WhiteBishops]) > 1)
            return false;

        if (m_Pieces[WhiteBishops] && std::popcount(m_Pieces[WhiteBishops] | m_Pieces[WhiteKnights]) > 1)
            return false;

        if (std::popcount(m_Pieces[WhiteKnights]) > 2)
            return false;
    }
    else
    {
        if (std::popcount(m_Pieces[BlackPawns] | m_Pieces[BlackRooks] | m_Pieces[BlackQueens]))
            return false;

        if (std::popcount(m_Pieces[BlackBishops]) > 1)
            return false;

        if (m_Pieces[BlackBishops] && std::popcount(m_Pieces[BlackBishops] | m_Pieces[BlackKnights]) > 1)
            return false;

        if (std::popcount(m_Pieces[BlackKnights]) > 2)
            return false;
    }

    return true;
}