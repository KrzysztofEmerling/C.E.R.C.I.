#include "FENParcer.h"
#include "BoardState.h"
#include "MoveGenerator.h"

#include <iostream>

BoardState::BoardState(String FENnotation)
{
    m_HeadIndex = 0;
    m_Flags = ParseFEN(FENnotation, m_Pieces);
    m_ZHash.Initialize(m_Flags, m_Pieces);
}

BoardState::BoardState(Flags flags, u64f (&pieces)[13], ZobristHash zHash) : m_Flags(flags), m_ZHash(zHash)
{
    m_HeadIndex = 0;
    for (int i = 0; i < 13; i++)
    {
        m_Pieces[i] = pieces[i];
    }
}

void BoardState::DrawBoard() const
{

    std::cout << "     a   b   c   d   e   f   g   h\n";
    std::cout << "   +---+---+---+---+---+---+---+---+\n";

    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << " " << (rank + 1) << " |";
        for (int file = 0; file < 8; file++)
        {
            int index = rank * 8 + file;
            const char *piece = " ";

            for (int i = 0; i < 12; i++)
            {
                if (m_Pieces[i] & (1ULL << index))
                {
                    piece = m_UnicodePieces[i];
                    break;
                }
            }
            std::cout << " " << piece << " |";
        }
        std::cout << " " << (rank + 1);

        if (rank == 7)
            std::cout << "\t\tNazwa Silnika: C.E.R.C.I\n";
        else if (rank == 6)
            std::cout << "\t\tChess Engine with Reinforcement-based\n";
        else if (rank == 5)
            std::cout << "\n";
        else if (rank == 4)
            std::cout << "\n";
        else if (rank == 3)
            std::cout << "\t\tBiałe na ruchu: " << (IsWhiteMove() ? "TAK (♚)\n" : "NIE (♔)\n");
        else if (rank == 2)
            std::cout << "\t\tPrawa do roszady: "
                      << (CanWhiteShortCastel() ? "K" : "-")
                      << (CanWhiteLongCastel() ? "Q " : "- ")
                      << (CanBlackShortCastel() ? "k" : "-")
                      << (CanBlackLongCastel() ? "q\n" : "-\n");

        else if (rank == 1)
            std::cout << "\t\tPółruchy: " << static_cast<int>(GetHalfMoves()) << "\n";
        else if (rank == 0)
            std::cout << "\t\tNumer ruchu: " << GetMoves() << "\n";
        else
            std::cout << "\n";

        if (rank == 6)
            std::cout << "   +---+---+---+---+---+---+---+---+\t\tCognitive Identities\n";
        else
            std::cout << "   +---+---+---+---+---+---+---+---+\n";
    }

    std::cout << "     a   b   c   d   e   f   g   h   " << std::endl;
}

void BoardState::MakeMove(Move move)
{
    saveSnapshot();
    m_PreviousMovesHashes[m_Flags.halfmoveClock] = m_ZHash.GetHash();

    u64 startingPos = BitboardsIndecies[move.startingSquere];
    u64 destPos = BitboardsIndecies[move.destSquere];
    int figToMove = FindPieceAt(startingPos, m_Flags.whiteOnMove);
    int figToCapture = FindPieceAt(destPos, !m_Flags.whiteOnMove);

    switch (move.flag)
    {
    case NormalMove:
        handleNormalMove(figToMove, move.startingSquere, startingPos, figToCapture, move.destSquere, destPos);
        break;

    case DoublePush:
        handleDoublePush(figToMove, move.startingSquere, move.destSquere, startingPos, destPos);
        break;

    case PromotionQueen:
        handlePromotion(figToMove, move.startingSquere, figToCapture, move.destSquere, startingPos, destPos,
                        m_Flags.whiteOnMove ? WhiteQueens : BlackQueens);
        break;

    case PromotionRook:
        handlePromotion(figToMove, move.startingSquere, figToCapture, move.destSquere, startingPos, destPos,
                        m_Flags.whiteOnMove ? WhiteRooks : BlackRooks);
        break;

    case PromotionKnight:
        handlePromotion(figToMove, move.startingSquere, figToCapture, move.destSquere, startingPos, destPos,
                        m_Flags.whiteOnMove ? WhiteKnights : BlackKnights);
        break;

    case PromotionBishop:
        handlePromotion(figToMove, move.startingSquere, figToCapture, move.destSquere, startingPos, destPos,
                        m_Flags.whiteOnMove ? WhiteBishops : BlackBishops);
        break;

    case EmpassantMove:
        handleEnPassant(figToMove, move.startingSquere, move.destSquere, startingPos, destPos);
        break;

    case Castling:
        handleCastling(move.startingSquere, move.destSquere);
        break;
    }

    // sprawdzenie zbicia wierzy w przypadku praw roszad
    if (figToCapture == WhiteRooks)
    {
        if (destPos & 0x80)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.whiteShortCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
        else if (destPos & 0x1)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.whiteLongCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
    }
    else if (figToCapture == BlackRooks)
    {
        if (destPos & 0x8000000000000000)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.blackShortCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
        else if (destPos & 0x100000000000000)
        {
            m_ZHash.UpdateCastling(m_Flags);
            m_Flags.blackLongCastelRights = false;
            m_ZHash.UpdateCastling(m_Flags);
        }
    }

    // Aktualizacja ruchu pełnego i zmiana strony
    if (!m_Flags.whiteOnMove)
        m_Flags.moves += 1;

    if (figToMove != WhitePawns && figToCapture == -1)
        m_Flags.halfmoveClock += 1;

    else if (figToMove != BlackPawns && figToCapture == -1)
        m_Flags.halfmoveClock += 1;

    m_Flags.whiteOnMove = !m_Flags.whiteOnMove;

    m_ZHash.ToggleSideToMove();
}

bool BoardState::MakeMove(const char *move_notation)
{
    // Parsowanie z pełną obsługą promocji i flag
    int from = SquareIndex[move_notation[0] - 'a'][move_notation[1] - '1'];
    int to = SquareIndex[move_notation[2] - 'a'][move_notation[3] - '1'];

    // Walidacja
    MoveList movesList;
    MoveGenerator::GetLegalMoves(*this, movesList);

    for (int i = 0; i < movesList.movesCount; i++)
    {
        Move move = movesList.moves[i];

        if (move.startingSquere == from && move.destSquere == to)
        {
            MakeMove(move);
            return true;
        }
    }
    return false;
}

u64 BoardState::GetRefHash()
{
    ZobristHash tempHash;
    tempHash.Initialize(m_Flags, m_Pieces);
    return tempHash.GetHash();
}