#include "BoardState.h"

void BoardState::saveSnapshot()
{
    m_Snapshots[m_HeadIndex].flags = m_Flags;
    m_Snapshots[m_HeadIndex].zHash = m_ZHash;
    for (int i = 0; i < 13; i++)
    {
        m_Snapshots[m_HeadIndex].pieces[i] = m_Pieces[i];
    }

    m_HeadIndex = (m_HeadIndex + 1) % maxHistorySize;
    if (m_SnapshotsCount < maxHistorySize)
    {
        ++m_SnapshotsCount;
    }
}

bool BoardState::UndoMove()
{
    if (m_SnapshotsCount == 0)
        return false;

    // Cofnij head do poprzedniego stanu
    m_HeadIndex = (m_HeadIndex - 1 + maxHistorySize) % maxHistorySize;

    // Przywróć stan z tablicy
    m_Flags = m_Snapshots[m_HeadIndex].flags;
    for (int i = 0; i < 13; i++)
    {
        m_Pieces[i] = m_Snapshots[m_HeadIndex].pieces[i];
    }
    m_ZHash = m_Snapshots[m_HeadIndex].zHash;

    m_SnapshotsCount--;
    return true;
}

int BoardState::findPieceAt(u64 squareBB, bool white)
{
    int offset = white * 6;
    for (int i = 6 - offset; i < 12 - offset; i++)
    {
        if (squareBB & m_Pieces[i])
            return i;
    }
    return -1;
}

void BoardState::handleNormalMove(int figToMove, int startIndex, u64 startingPos, int figToCapture, int destIndex, u64 destPos)
{
    clearEnPassant();

    if (figToMove == WhiteKing)
    {
        m_ZHash.UpdateCastling(m_Flags);
        m_Flags.whiteShortCastelRights = false;
        m_Flags.whiteLongCastelRights = false;
        m_ZHash.UpdateCastling(m_Flags);
    }
    else if (figToMove == WhiteRooks)
    {
        if (startingPos & 0x80)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.whiteShortCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
        else if (startingPos & 0x1)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.whiteLongCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
    }

    else if (figToMove == BlackKing)
    {
        m_ZHash.UpdateCastling(m_Flags);
        m_Flags.blackShortCastelRights = false;
        m_Flags.blackLongCastelRights = false;
        m_ZHash.UpdateCastling(m_Flags);
    }
    else if (figToMove == BlackRooks)
    {
        if (startingPos & 0x8000000000000000)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.blackShortCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
        else if (startingPos & 0x100000000000000)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.blackLongCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
    }

    m_ZHash.UpdatePiece(startIndex, figToMove);
    m_Pieces[figToMove] &= ~startingPos;
    m_Pieces[figToMove] |= destPos;
    m_ZHash.UpdatePiece(destIndex, figToMove);

    if (figToCapture > -1)
    {
        m_Pieces[figToCapture] &= ~destPos;
        m_Flags.halfmoveClock = 0;
        m_ZHash.UpdatePiece(destIndex, figToCapture);
    }
}

void BoardState::handleDoublePush(int figToMove, int startIndex, int destIndex, u64 startingPos, u64 destPos)
{
    // Przesuń pionka
    m_ZHash.UpdatePiece(startIndex, figToMove);
    m_Pieces[figToMove] &= ~startingPos;
    m_Pieces[figToMove] |= destPos;
    m_ZHash.UpdatePiece(destIndex, figToMove);

    // Ustaw pole en passant
    int direction = (figToMove == WhitePawns) ? -8 : 8;
    int epSquare = destIndex + direction;

    // wyczyszczenie poprzedniego enpassanta
    if (m_Pieces[Empassants])
    {
        int file = __builtin_ctzll(m_Pieces[Empassants]) / 8;
        m_ZHash.UpdateEnPassant(file);
    }
    m_Pieces[Empassants] = BitboardsIndecies[epSquare];
    m_ZHash.UpdateEnPassant(epSquare / 8);

    m_Flags.halfmoveClock = 0;
}

void BoardState::handlePromotion(int figToMove, int startIndex, int figToCapture, int destIndex, u64 startingPos, u64 destPos, int new_piece)
{
    clearEnPassant();

    m_Flags.halfmoveClock = 0;

    m_Pieces[figToMove] &= ~startingPos;
    m_ZHash.UpdatePiece(startIndex, figToMove);

    if (figToCapture > -1)
    {
        m_Pieces[figToCapture] &= ~destPos;
        m_ZHash.UpdatePiece(destIndex, figToCapture);
    }

    m_Pieces[new_piece] |= destPos;
    m_ZHash.UpdatePiece(destIndex, new_piece);

    m_Flags.halfmoveClock = 0;
}

void BoardState::handleEnPassant(int figToMove, int startIndex, int destIndex, u64 startingPos, u64 destPos)
{
    int file = __builtin_ctzll(m_Pieces[Empassants]) / 8;
    m_ZHash.UpdateEnPassant(file);
    m_Pieces[Empassants] = 0ULL;

    m_ZHash.UpdatePiece(startIndex, figToMove);
    m_Pieces[figToMove] &= ~startingPos;
    m_Pieces[figToMove] |= destPos;
    m_ZHash.UpdatePiece(destIndex, figToMove);

    int epSquare = (figToMove == WhitePawns) ? destIndex - 8 : destIndex + 8;
    int capturedPawn = (figToMove == WhitePawns) ? BlackPawns : WhitePawns;
    m_Pieces[capturedPawn] &= ~BitboardsIndecies[epSquare];
    m_ZHash.UpdatePiece(epSquare, capturedPawn);

    m_Flags.halfmoveClock = 0;
}

void BoardState::handleCastling(int startIndex, int destIndex)
{
    clearEnPassant();

    m_ZHash.UpdateCastling(m_Flags);
    if (m_Flags.whiteOnMove)
    {
        m_Flags.whiteShortCastelRights = false;
        m_Flags.whiteLongCastelRights = false;
        m_ZHash.UpdateCastling(m_Flags);

        if (destIndex == 6) // short
        {
            m_ZHash.UpdatePiece(4, WhiteKing);
            m_ZHash.UpdatePiece(6, WhiteKing);
            m_ZHash.UpdatePiece(7, WhiteRooks);
            m_ZHash.UpdatePiece(5, WhiteRooks);

            m_Pieces[WhiteKing] = 0x40;
            m_Pieces[WhiteRooks] &= 0xffffffffffffff7f;
            m_Pieces[WhiteRooks] |= 0x20;
        }
        else
        {
            m_ZHash.UpdatePiece(4, WhiteKing);
            m_ZHash.UpdatePiece(2, WhiteKing);
            m_ZHash.UpdatePiece(0, WhiteRooks);
            m_ZHash.UpdatePiece(3, WhiteRooks);

            m_Pieces[WhiteKing] = 0x4;
            m_Pieces[WhiteRooks] &= 0xfffffffffffffffe;
            m_Pieces[WhiteRooks] |= 0x8;
        }
    }
    else
    {
        m_Flags.blackShortCastelRights = false;
        m_Flags.blackLongCastelRights = false;
        m_ZHash.UpdateCastling(m_Flags);

        if (destIndex == 62) // short
        {
            m_ZHash.UpdatePiece(60, BlackKing);
            m_ZHash.UpdatePiece(62, BlackKing);
            m_ZHash.UpdatePiece(63, BlackRooks);
            m_ZHash.UpdatePiece(61, BlackRooks);

            m_Pieces[BlackKing] = 0x4000000000000000;
            m_Pieces[BlackRooks] &= 0x7fffffffffffffff;
            m_Pieces[BlackRooks] |= 0x2000000000000000;
        }
        else
        {
            m_ZHash.UpdatePiece(60, BlackKing);
            m_ZHash.UpdatePiece(58, BlackKing);
            m_ZHash.UpdatePiece(56, BlackRooks);
            m_ZHash.UpdatePiece(59, BlackRooks);

            m_Pieces[BlackKing] = 0x400000000000000;
            m_Pieces[BlackRooks] &= 0xfeffffffffffffff;
            m_Pieces[BlackRooks] |= 0x800000000000000;
        }
    }
}

void BoardState::clearEnPassant()
{
    if (m_Pieces[Empassants])
    {
        int file = __builtin_ctzll(m_Pieces[Empassants]) / 8;
        m_ZHash.UpdateEnPassant(file);
        m_Pieces[Empassants] = 0ULL;
    }
}